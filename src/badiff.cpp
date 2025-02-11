/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
#include <badiff/q/rewinding_op_queue.hpp>
#include <badiff/q/rop_queue.hpp>
#include <badiff/q/rreplace_op_queue.hpp>
#include <badiff/q/rstream_replace_op_queue.hpp>
#include <badiff/q/stream_replace_op_queue.hpp>

#include <fstream>
#include <functional>
#include <istream>
#include <map>
#include <ostream>
#include <sstream>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>

namespace badiff {
bool CONSOLE_OUTPUT = false;

namespace {
/**
 * \brief Diifs and optimizes an OpQueue..
 *
 * Argument must be chunked enough for GraphOpQueue to perform the initial
 * diffing.
 */
std::unique_ptr<q::OpQueue> ComputeDiff(std::unique_ptr<q::OpQueue> op_queue) {

  // Perform the initial diffing operation.
  op_queue.reset(new q::GraphOpQueue(
      std::move(op_queue), std::unique_ptr<alg::Graph>(new alg::InertialGraph),
      q::GraphOpQueue::DELETE_THEN_INSERT));
  op_queue.reset(new q::RewindingOpQueue(std::move(op_queue)));

  // Post-process the raw diff.
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));

  // Diff again, but swap the chunk ordering.
  op_queue.reset(new q::GraphOpQueue(
      std::move(op_queue), std::unique_ptr<alg::Graph>(new alg::InertialGraph),
      q::GraphOpQueue::INSERT_THEN_DELETE));
  op_queue.reset(new q::RewindingOpQueue(std::move(op_queue)));

  // Post-process the secondary diff.
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  op_queue.reset(new q::CompactingOpQueue(std::move(op_queue)));

  // Strip the value arrays from DELETE and NEXT operations.
  op_queue.reset(new q::MinimizeOpQueue(std::move(op_queue)));

  return op_queue;
}

struct MakeParameters {
  std::unique_ptr<q::OpQueue> forward_op_queue_;
  std::unique_ptr<q::OpQueue> reverse_op_queue_;
  std::function<char(int)> get_original_;
  std::function<char(int)> get_target_;
  int original_len_;
  int target_len_;
};

std::unique_ptr<Delta> MakeParameterized(MakeParameters parameters) {
  // OpQueues for the byte arrays, in order and in reverse order.
  std::unique_ptr<q::OpQueue> op_queue(std::move(parameters.forward_op_queue_));
  std::unique_ptr<q::OpQueue> rop_queue(
      std::move(parameters.reverse_op_queue_));
  int original_len = parameters.original_len_;
  int target_len = parameters.target_len_;

  op_queue = ComputeDiff(std::move(op_queue));   // forward diff
  rop_queue = ComputeDiff(std::move(rop_queue)); // backward diff
  rop_queue.reset(new q::ROpQueue(
      std::move(rop_queue))); // Make the individual data values forward.

  /*
   * Merge the original and reverse diffs.
   *
   * Builds a combined diff by popping off elements of the forward and backward
   * diffs, selecting the element that has the smallest size.  When it is no
   * longer possible to select an operation without overlapping on the other
   * diff, the middle section is diffed itself.
   */

  int original_pos = 0, target_pos = 0; // position in forward diff
  int original_rpos = original_len,
      target_rpos = target_len; // position in backwards diff

  std::vector<std::unique_ptr<Op>> diff_ops,
      rdiff_ops; // Selected forward and backward operations.

  std::unique_ptr<Op> next_op, rnext_op;
  while (true) {
    // Pop an op if needed and available.
    if (!next_op && !op_queue->IsEmpty()) {
      next_op = op_queue->Pop();
    }
    if (!rnext_op && !rop_queue->IsEmpty()) {
      rnext_op = rop_queue->Pop();
    }
    // If there are no ops left we are done meeting in the middle.
    if (!next_op && !rnext_op)
      break;
    // positions if the ops were selected
    int next_original_pos = original_pos, next_original_rpos = original_rpos;
    int next_target_pos = target_pos, next_target_rpos = target_rpos;

    if (next_op) {
      if (next_op->GetType() != Op::INSERT) // NEXT and DELETE consume original
        next_original_pos += next_op->GetLength();
      if (next_op->GetType() != Op::DELETE) // NEXT and INSERT consume target
        next_target_pos += next_op->GetLength();
    }
    if (rnext_op) {
      if (rnext_op->GetType() != Op::INSERT) // NEXT and DELETE consume original
        next_original_rpos -= rnext_op->GetLength();
      if (rnext_op->GetType() != Op::DELETE) // NEXT and INSERT consume target
        next_target_rpos -= rnext_op->GetLength();
    }

    // Check for overlap
    if (next_original_pos >= next_original_rpos)
      break; // Overlap, done meeting in the middle.
    if (next_target_pos >= next_target_rpos)
      break; // Overlap, done meeting in the middle.

    if (next_op && !rnext_op) {
      // If forward op is the only one
      diff_ops.insert(diff_ops.end(), std::move(next_op));
      original_pos = next_original_pos;
      target_pos = next_target_pos;
    } else if (!next_op && rnext_op) {
      // If reverse op is the only one
      rdiff_ops.insert(rdiff_ops.begin(), std::move(rnext_op));
      original_rpos = next_original_rpos;
      target_rpos = next_target_rpos;
    } else if (next_op && rnext_op) {
      // Choose the op based on heuristic for length.
      // Prefer NEXT > DELETE > INSERT.
      int next_len = 0; // delete counts as zero length in the heuristic
      if (next_op->GetType() == Op::INSERT)
        next_len = next_op->GetLength(); // insert is positive length
      if (next_op->GetType() == Op::NEXT)
        next_len = -next_op->GetLength(); // NEXT is negative length

      // Heuristic for reverse op, same as forward op
      int rnext_len = 0; // 0 for DELETE
      if (rnext_op->GetType() == Op::INSERT)
        rnext_len = rnext_op->GetLength(); // positive for INSERT
      if (rnext_op->GetType() == Op::NEXT)
        rnext_len = -rnext_op->GetLength(); // negative for NEXT

      // Choose the shortest according to the heuristic.
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

  // Middle queue to be graphed.
  std::unique_ptr<q::OpQueue> middle(new q::OpQueue);

  // Delete the original middle region.
  if (original_pos < original_rpos) {
    int len = original_rpos - original_pos;
    std::unique_ptr<char[]> value(new char[len]);
    for (int pos = original_pos; pos < original_rpos; ++pos) {
      value[pos - original_pos] = parameters.get_original_(pos);
    }
    middle->Push(Op(Op::DELETE, len, std::move(value)));
  }
  // Insert the target middle region.
  if (target_pos < target_rpos) {
    int len = target_rpos - target_pos;
    std::unique_ptr<char[]> value(new char[len]);
    for (int pos = target_pos; pos < target_rpos; ++pos) {
      value[pos - target_pos] = parameters.get_target_(pos);
    }
    middle->Push(Op(Op::INSERT, len, std::move(value)));
  }

  middle.reset(new q::ChunkingOpQueue(std::move(middle)));
  middle = ComputeDiff(std::move(middle));

  // Combine the selected forward ops, middle ops, and backwards ops.

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

  // Stats
  std::unique_ptr<Delta> diff(new Delta);
  diff->original_len_ = original_len;
  diff->target_len_ = target_len;
  diff->delta_len_ = str.size();
  diff->delta_.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->delta_.get());

  return diff;
}

} // namespace

std::unique_ptr<Delta> Delta::Make(const char *original, int original_len,
                                   const char *target, int target_len) {
  // OpQueues for the byte arrays, in order and in reverse order.
  std::unique_ptr<q::OpQueue> op_queue(
      new q::ReplaceOpQueue(original, original_len, target, target_len));
  std::unique_ptr<q::OpQueue> rop_queue(
      new q::RReplaceOpQueue(original, original_len, target, target_len));

  MakeParameters parameters;
  parameters.forward_op_queue_ = std::move(op_queue);
  parameters.reverse_op_queue_ = std::move(rop_queue);
  parameters.original_len_ = original_len;
  parameters.target_len_ = target_len;
  parameters.get_original_ = [&original](int pos) { return original[pos]; };
  parameters.get_target_ = [&target](int pos) { return target[pos]; };

  return MakeParameterized(std::move(parameters));
}

std::unique_ptr<Delta> Delta::Make(std::istream &original, int original_len,
                                   std::istream &target, int target_len) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::StreamReplaceOpQueue(original, original_len, target, target_len));
  std::unique_ptr<q::OpQueue> rop_queue(
      new q::RStreamReplaceOpQueue(original, original_len, target, target_len));

  MakeParameters parameters;
  parameters.forward_op_queue_ = std::move(op_queue);
  parameters.reverse_op_queue_ = std::move(rop_queue);
  parameters.original_len_ = original_len;
  parameters.target_len_ = target_len;
  parameters.get_original_ = [&original](int pos) {
    original.seekg(pos);
    return original.get();
  };
  parameters.get_target_ = [&target](int pos) {
    target.seekg(pos);
    return target.get();
  };

  return MakeParameterized(std::move(parameters));
}

std::unique_ptr<Delta> Delta::Make(std::string original_file,
                                   std::string target_file) {
  // Memory-map the files and diff them.

  struct stat original_stat;
  int original_fd;
  original_fd = open(original_file.c_str(), O_RDONLY, 0);
  if (original_fd == -1)
    return nullptr;
  if (fstat(original_fd, &original_stat) == -1) {
    close(original_fd);
    return nullptr;
  }

  struct stat target_stat;
  int target_fd;
  target_fd = open(target_file.c_str(), O_RDONLY, 0);
  if (target_fd == -1) {
    close(original_fd);
    return nullptr;
  }
  if (fstat(target_fd, &target_stat) == -1) {
    close(original_fd);
    close(target_fd);
    return nullptr;
  }

  int original_size = original_stat.st_size;
  int target_size = target_stat.st_size;

  const char *original_mmap = (const char *)mmap(NULL, original_size, PROT_READ,
                                                 MAP_PRIVATE, original_fd, 0);
  const char *target_mmap = (const char *)mmap(NULL, target_size, PROT_READ,
                                               MAP_PRIVATE, target_fd, 0);

  auto diff = badiff::Delta::Make(original_mmap, original_stat.st_size,
                                  target_mmap, target_stat.st_size);
  close(original_fd);
  close(target_fd);
  return std::move(diff);
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

void Delta::Apply(std::string original_file, std::string target_file) {
  std::ifstream original_stream(original_file);
  std::ofstream target_stream(target_file);
  Apply(original_stream, target_stream);
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

void Delta::Serialize(std::string delta_file) const {
  std::ofstream delta_stream(delta_file);
  Serialize(delta_stream);
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

bool Delta::Deserialize(std::string delta_file) {
  std::ifstream delta_stream(delta_file);
  return Deserialize(delta_stream);
}

} // namespace badiff
