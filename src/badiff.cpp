#include <badiff/badiff.hpp>
#include <badiff/op.hpp>

#include <badiff/defaults.hpp>

#include <badiff/alg/edit_graph.hpp>
#include <badiff/alg/graph.hpp>
#include <badiff/alg/inertial_graph.hpp>

#include <badiff/q/chunking_op_queue.hpp>
#include <badiff/q/coalescing_op_queue.hpp>
#include <badiff/q/compacting_op_queue.hpp>
#include <badiff/q/graph_op_queue.hpp>
#include <badiff/q/minimize_op_queue.hpp>
#include <badiff/q/op_queue.hpp>
#include <badiff/q/replace_op_queue.hpp>
#include <badiff/q/rop_queue.hpp>
#include <badiff/q/rreplace_op_queue.hpp>
#include <badiff/q/rstream_replace_op_queue.hpp>
#include <badiff/q/stream_replace_op_queue.hpp>

#include <istream>
#include <map>
#include <ostream>
#include <sstream>

namespace badiff {
bool CONSOLE_OUTPUT = false;

namespace {
std::unique_ptr<q::OpQueue> ComputeDiff(std::unique_ptr<q::OpQueue> op_queue) {

  op_queue.reset(
      new q::GraphOpQueue(std::move(op_queue),
                          std::unique_ptr<alg::Graph>(new alg::InertialGraph)));

  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  op_queue.reset(new q::CompactingOpQueue(std::move(op_queue)));

  op_queue.reset(new q::ChunkingOpQueue(std::move(op_queue)));

  op_queue.reset(
      new q::GraphOpQueue(std::move(op_queue),
                          std::unique_ptr<alg::Graph>(new alg::InertialGraph)));

  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  op_queue.reset(new q::CompactingOpQueue(std::move(op_queue)));

  op_queue.reset(new q::MinimizeOpQueue(std::move(op_queue)));

  return op_queue;
}
} // namespace

std::unique_ptr<Delta> Delta::Make(const char *original, int original_len,
                                   const char *target, int target_len) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::ReplaceOpQueue(original, original_len, target, target_len));
  std::unique_ptr<q::OpQueue> rop_queue(
      new q::RReplaceOpQueue(original, original_len, target, target_len));

  op_queue = ComputeDiff(std::move(op_queue));
  rop_queue = ComputeDiff(std::move(rop_queue));
  rop_queue.reset(new q::ROpQueue(std::move(rop_queue)));

  // Merge the original and reverse diffs
  int original_pos = 0, target_pos = 0;
  int original_rpos = original_len, target_rpos = target_len;

  std::vector<std::unique_ptr<Op>> diff_ops, rdiff_ops;

  std::unique_ptr<Op> next_op, rnext_op;
  while (true) {
    if (!next_op && !op_queue->IsEmpty()) {
      next_op = op_queue->Pop();
    }
    if (!rnext_op && !rop_queue->IsEmpty()) {
      rnext_op = rop_queue->Pop();
    }
    if (!next_op && !rnext_op)
      break;
    int next_original_pos = original_pos, next_original_rpos = original_rpos;
    int next_target_pos = target_pos, next_target_rpos = target_rpos;

    if (next_op) {
      if (next_op->GetType() != Op::INSERT)
        next_original_pos += next_op->GetLength();
      if (next_op->GetType() != Op::DELETE)
        next_target_pos += next_op->GetLength();
    }
    if (rnext_op) {
      if (rnext_op->GetType() != Op::INSERT)
        next_original_rpos -= rnext_op->GetLength();
      if (rnext_op->GetType() != Op::DELETE)
        next_target_rpos -= rnext_op->GetLength();
    }

    if (next_original_pos >= next_original_rpos)
      break;
    if (next_target_pos >= next_target_rpos)
      break;

    if (next_op && !rnext_op) {
      diff_ops.insert(diff_ops.end(), std::move(next_op));
      original_pos = next_original_pos;
      target_pos = next_target_pos;
    } else if (!next_op && rnext_op) {
      rdiff_ops.insert(rdiff_ops.begin(), std::move(rnext_op));
      original_rpos = next_original_rpos;
      target_rpos = next_target_rpos;
    } else if (next_op && rnext_op) {
      int next_len = 0;
      if (next_op->GetType() == Op::INSERT)
        next_len = next_op->GetLength();
      int rnext_len = 0;
      if (rnext_op->GetType() == Op::INSERT)
        rnext_len = rnext_op->GetLength();
      if (next_len <= rnext_len) {
        diff_ops.insert(diff_ops.end(), std::move(next_op));
        original_pos = next_original_pos;
        target_pos = next_target_pos;
      } else {
        rdiff_ops.insert(rdiff_ops.begin(), std::move(rnext_op));
        original_rpos = next_original_rpos;
        target_rpos = next_target_rpos;
      }
    }
  }

  std::unique_ptr<q::OpQueue> middle(new q::OpQueue);

  if (original_pos < original_rpos) {
    middle->Push(Op(Op::DELETE, original_rpos - original_pos));
  }
  if (target_pos < target_rpos) {
    int len = target_rpos - target_pos;
    std::unique_ptr<char[]> value(new char[len]);
    std::copy(target + target_pos, target + target_rpos, value.get());
    middle->Push(Op(Op::INSERT, len, std::move(value)));
  }

  middle.reset(new q::GraphOpQueue(
      std::move(middle), std::unique_ptr<alg::Graph>(new alg::InertialGraph)));
  middle.reset(new q::MinimizeOpQueue(std::move(middle)));

  op_queue.reset(new q::OpQueue);
  for (auto &op : diff_ops) {
    op_queue->Push(*std::move(op));
  }

  while (!middle->IsEmpty()) {
    op_queue->Push(*middle->Pop());
  }

  for (auto &op : rdiff_ops) {
    op_queue->Push(*std::move(op));
  }

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Delta> diff(new Delta);
  diff->original_len_ = original_len;
  diff->target_len_ = target_len;
  diff->delta_len_ = str.size();
  diff->delta_.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->delta_.get());

  return diff;
}

std::unique_ptr<Delta> Delta::Make(std::istream &original, int original_len,
                                   std::istream &target, int target_len) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::StreamReplaceOpQueue(original, original_len, target, target_len));
  std::unique_ptr<q::OpQueue> rop_queue(
      new q::RStreamReplaceOpQueue(original, original_len, target, target_len));

  op_queue = ComputeDiff(std::move(op_queue));
  rop_queue = ComputeDiff(std::move(rop_queue));
  rop_queue.reset(new q::ROpQueue(std::move(rop_queue)));

  // Merge the original and reverse diffs
  int original_pos = 0, target_pos = 0;
  int original_rpos = original_len, target_rpos = target_len;

  std::vector<std::unique_ptr<Op>> diff_ops, rdiff_ops;

  std::unique_ptr<Op> next_op, rnext_op;
  while (true) {
    if (!next_op && !op_queue->IsEmpty()) {
      next_op = op_queue->Pop();
    }
    if (!rnext_op && !rop_queue->IsEmpty()) {
      rnext_op = rop_queue->Pop();
    }
    if (!next_op && !rnext_op)
      break;
    int next_original_pos = original_pos, next_original_rpos = original_rpos;
    int next_target_pos = target_pos, next_target_rpos = target_rpos;

    if (next_op) {
      if (next_op->GetType() != Op::INSERT)
        next_original_pos += next_op->GetLength();
      if (next_op->GetType() != Op::DELETE)
        next_target_pos += next_op->GetLength();
    }
    if (rnext_op) {
      if (rnext_op->GetType() != Op::INSERT)
        next_original_rpos -= rnext_op->GetLength();
      if (rnext_op->GetType() != Op::DELETE)
        next_target_rpos -= rnext_op->GetLength();
    }

    if (next_original_pos >= next_original_rpos)
      break;
    if (next_target_pos >= next_target_rpos)
      break;

    if (next_op && !rnext_op) {
      diff_ops.insert(diff_ops.end(), std::move(next_op));
      original_pos = next_original_pos;
      target_pos = next_target_pos;
    } else if (!next_op && rnext_op) {
      rdiff_ops.insert(rdiff_ops.begin(), std::move(rnext_op));
      original_rpos = next_original_rpos;
      target_rpos = next_target_rpos;
    } else if (next_op && rnext_op) {
      int next_len = 0;
      if (next_op->GetType() == Op::INSERT)
        next_len = next_op->GetLength();
      int rnext_len = 0;
      if (rnext_op->GetType() == Op::INSERT)
        rnext_len = rnext_op->GetLength();
      if (next_len <= rnext_len) {
        diff_ops.insert(diff_ops.end(), std::move(next_op));
        original_pos = next_original_pos;
        target_pos = next_target_pos;
      } else {
        rdiff_ops.insert(rdiff_ops.begin(), std::move(rnext_op));
        original_rpos = next_original_rpos;
        target_rpos = next_target_rpos;
      }
    }
  }

  std::unique_ptr<q::OpQueue> middle(new q::OpQueue);

  if (original_pos < original_rpos) {
    middle->Push(Op(Op::DELETE, original_rpos - original_pos));
  }
  if (target_pos < target_rpos) {
    int len = target_rpos - target_pos;
    std::unique_ptr<char[]> value(new char[len]);
    target.seekg(target_pos);
    for (int i = 0; i < len; ++i) {
      value[i] = target.get();
    }
    middle->Push(Op(Op::INSERT, len, std::move(value)));
  }

  middle.reset(new q::GraphOpQueue(
      std::move(middle), std::unique_ptr<alg::Graph>(new alg::InertialGraph)));
  middle.reset(new q::MinimizeOpQueue(std::move(middle)));

  op_queue.reset(new q::OpQueue);
  for (auto &op : diff_ops) {
    op_queue->Push(*std::move(op));
  }

  while (!middle->IsEmpty()) {
    op_queue->Push(*middle->Pop());
  }

  for (auto &op : rdiff_ops) {
    op_queue->Push(*std::move(op));
  }

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Delta> diff(new Delta);
  diff->original_len_ = original_len;
  diff->target_len_ = target_len;
  diff->delta_len_ = str.size();
  diff->delta_.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->delta_.get());

  return diff;
}

void Delta::Apply(std::istream &original, std::ostream &target) {
  std::istringstream in(std::string(delta_.get(), delta_len_));
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);

  op_queue->Deserialize(in);
  op_queue->Apply(original, target);
}

void Delta::Apply(const char *original, char *target) {
  std::basic_stringbuf<char> target_buf;
  target_buf.pubsetbuf(target, target_len_);

  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);

  std::istringstream diff_stream(std::string(delta_.get(), delta_len_));
  std::istringstream original_stream(std::string(original, original_len_));
  std::ostream target_stream(&target_buf);

  op_queue->Deserialize(diff_stream);
  op_queue->Apply(original_stream, target_stream);
}

static const char *MAGIC = "\xBA\xD1\xFF";
static const char FORMAT_VERSION_MAJOR = 1;
static const char FORMAT_VERSION_MINOR = 0;

static const char *ORIGINAL_LEN_FIELD = "original_len";
static const char *TARGET_LEN_FIELD = "target_len";
static const char *DELTA_LEN_FIELD = "delta_len";

static void WriteFieldLength(std::ostream &out, int len) {
  out.put(len >> 8);
  out.put(len);
}

static void ReadFieldLength(std::istream &in, int *len) {
  *len = in.get() << 8;
  *len += in.get();
}

static void WriteFieldString(std::ostream &out, const std::string &string) {
  WriteFieldLength(out, string.size());
  out.write(string.c_str(), string.size());
}

static void ReadFieldString(std::istream &in, std::string *string) {
  int size;
  ReadFieldLength(in, &size);
  char buf[size];
  in.read(buf, size);
  *string = std::string(buf, buf + size);
}

void Delta::Serialize(std::ostream &out) const {
  out.write(MAGIC, sizeof(MAGIC));
  out.put(FORMAT_VERSION_MAJOR);
  out.put(FORMAT_VERSION_MINOR);

  std::map<std::string, std::string> fields;
  fields[ORIGINAL_LEN_FIELD] = std::to_string(original_len_);
  fields[TARGET_LEN_FIELD] = std::to_string(target_len_);
  fields[DELTA_LEN_FIELD] = std::to_string(delta_len_);

  WriteFieldLength(out, fields.size());
  for (auto &field : fields) {
    WriteFieldString(out, field.first);
    WriteFieldString(out, field.second);
  }

  out.write(delta_.get(), delta_len_);
}

bool Delta::Deserialize(std::istream &in) {
  char magic[sizeof(MAGIC)];
  in.read(magic, sizeof(MAGIC));
  if (!std::equal(magic, magic + sizeof(MAGIC), MAGIC))
    return false;
  char major = in.get();
  if (major != FORMAT_VERSION_MAJOR)
    return false;
  char minor = in.get();
  if (minor > FORMAT_VERSION_MINOR)
    return false;

  int num_fields;
  ReadFieldLength(in, &num_fields);
  std::map<std::string, std::string> fields;
  for (int i = 0; i < num_fields; ++i) {
    std::string key;
    ReadFieldString(in, &key);
    std::string value;
    ReadFieldString(in, &value);
    fields[key] = value;
  }

  original_len_ = atoi(fields[ORIGINAL_LEN_FIELD].c_str());
  target_len_ = atoi(fields[TARGET_LEN_FIELD].c_str());
  delta_len_ = atoi(fields[DELTA_LEN_FIELD].c_str());

  delta_.reset(new char[delta_len_]);
  in.read(delta_.get(), delta_len_);
  return true;
}

} // namespace badiff
