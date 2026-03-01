/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <badiff/q/cdc_op_queue.hpp>

#include <algorithm>
#include <unordered_map>

namespace badiff {
namespace q {

namespace {

static uint32_t GEAR[256];
static bool gear_initialized = false;

// Populate the gear hash lookup table with pseudo-random values derived from
// a xorshift-multiply sequence so chunk boundaries are content-defined but
// independent of file position.
static void InitGear() {
  if (gear_initialized)
    return;
  uint32_t seed = 0x27D4EB2Fu;
  for (int i = 0; i < 256; i++) {
    seed ^= seed >> 15;
    seed *= 2246822519u;
    seed ^= seed >> 13;
    GEAR[i] = seed;
  }
  gear_initialized = true;
}

} // namespace

CdcOpQueue::CdcOpQueue(const char *original, int original_len,
                       const char *target, int target_len, int avg_chunk,
                       std::function<void(int, int, int, int)> *reporter)
    : min_chunk_(avg_chunk / 4), max_chunk_(avg_chunk * 4),
      reporter_(reporter), original_len_(original_len),
      target_len_(target_len), original_pos_(0), target_pos_(0) {
  InitGear();
  // mask_: set the low log2(avg_chunk) bits so split probability is ~1/avg_chunk
  uint32_t bits = 1;
  while ((1u << bits) < (uint32_t)avg_chunk)
    bits++;
  mask_ = (1u << bits) - 1u;

  CdcSplit(original, original_len, Op::DELETE, deletes_);
  CdcSplit(target, target_len, Op::INSERT, inserts_);
  Plan();
}

CdcOpQueue::~CdcOpQueue() {}

void CdcOpQueue::CdcSplit(const char *data, int len, Op::Type type,
                           std::vector<Op> &out) {
  uint32_t hash = 0;
  int start = 0;

  auto emit = [&](int end) {
    int extent = end - start;
    if (extent <= 0)
      return;
    std::unique_ptr<char[]> value(new char[extent]);
    std::copy(data + start, data + end, value.get());
    out.emplace_back(type, extent, std::move(value));
    start = end;
    hash = 0;
  };

  for (int i = 0; i < len; i++) {
    hash = (hash << 1) + GEAR[(unsigned char)data[i]];
    int extent = i - start + 1;
    if (extent >= max_chunk_ || (extent >= min_chunk_ && (hash & mask_) == 0))
      emit(i + 1);
  }
  emit(len);
}

uint32_t CdcOpQueue::ChunkHash(const char *data, int len) const {
  // FNV-1a over the chunk bytes for fast exact-match detection.
  uint32_t h = 2166136261u;
  for (int i = 0; i < len; i++)
    h = (h ^ (unsigned char)data[i]) * 16777619u;
  return h;
}

void CdcOpQueue::Plan() {
  // Build hash index: content hash -> sorted list of delete chunk indices.
  std::unordered_map<uint32_t, std::vector<int>> hash_to_di;
  for (int i = 0; i < (int)deletes_.size(); i++) {
    uint32_t h =
        ChunkHash(deletes_[i].GetValue().get(), deletes_[i].GetLength());
    hash_to_di[h].push_back(i);
  }

  // Greedy LCS: walk insert chunks in order; for each one find the earliest
  // matching delete chunk (by hash then exact bytes) that preserves relative
  // order (enforced by min_di).
  std::vector<bool> di_used(deletes_.size(), false);
  int min_di = 0;
  std::vector<std::pair<int, int>> lcs; // (delete_idx, insert_idx)

  for (int ii = 0; ii < (int)inserts_.size(); ii++) {
    uint32_t h =
        ChunkHash(inserts_[ii].GetValue().get(), inserts_[ii].GetLength());
    auto it = hash_to_di.find(h);
    if (it == hash_to_di.end())
      continue;
    for (int di : it->second) {
      if (di < min_di || di_used[di])
        continue;
      const Op &d = deletes_[di];
      const Op &ins = inserts_[ii];
      if (d.GetLength() == ins.GetLength() &&
          std::equal(d.GetValue().get(), d.GetValue().get() + d.GetLength(),
                     ins.GetValue().get())) {
        lcs.push_back({di, ii});
        di_used[di] = true;
        min_di = di + 1;
        break;
      }
    }
  }

  // Generate the op sequence from the LCS alignment.
  // Between matched pairs: interleave unmatched DELETEs and INSERTs so the
  // downstream GraphOpQueue can diff them byte-by-byte.
  // At each matched pair: emit NEXT (identical content, no diffing needed).
  int di = 0, ii = 0;
  for (auto &match : lcs) {
    int lcs_di = match.first, lcs_ii = match.second;
    while (di < lcs_di || ii < lcs_ii) {
      if (di < lcs_di)
        output_.push_back(std::move(deletes_[di++]));
      if (ii < lcs_ii)
        output_.push_back(std::move(inserts_[ii++]));
    }
    output_.emplace_back(Op::NEXT, deletes_[lcs_di].GetLength());
    di++;
    ii++;
  }
  // Remaining unmatched chunks.
  while (di < (int)deletes_.size() || ii < (int)inserts_.size()) {
    if (di < (int)deletes_.size())
      output_.push_back(std::move(deletes_[di++]));
    if (ii < (int)inserts_.size())
      output_.push_back(std::move(inserts_[ii++]));
  }
}

bool CdcOpQueue::Pull() {
  if (output_.empty())
    return false;
  const Op &op = output_.front();
  Op::Type type = op.GetType();
  int len = op.GetLength();
  Prepare(std::move(output_.front()));
  output_.pop_front();
  if (type == Op::DELETE || type == Op::NEXT)
    original_pos_ += len;
  if (type == Op::INSERT || type == Op::NEXT)
    target_pos_ += len;
  if (reporter_)
    (*reporter_)(original_pos_, target_pos_, original_len_, target_len_);
  return true;
}

} // namespace q
} // namespace badiff
