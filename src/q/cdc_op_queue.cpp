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

CdcOpQueue::Sig CdcOpQueue::ChunkSig(const char *data, int len) {
  // Compute the kSigSize smallest FNV-1a 4-gram hashes (bottom-K MinHash).
  // Maintained as a max-heap so large values are cheaply discarded.
  // The final sorted vector supports O(n) set-intersection in SigSimilarity.
  std::vector<uint32_t> heap;
  heap.reserve(kSigSize + 1);
  for (int i = 0; i + 3 < len; i++) {
    uint32_t h = 2166136261u;
    h = (h ^ (uint8_t)data[i]) * 16777619u;
    h = (h ^ (uint8_t)data[i + 1]) * 16777619u;
    h = (h ^ (uint8_t)data[i + 2]) * 16777619u;
    h = (h ^ (uint8_t)data[i + 3]) * 16777619u;
    if ((int)heap.size() < kSigSize) {
      heap.push_back(h);
      if ((int)heap.size() == kSigSize)
        std::make_heap(heap.begin(), heap.end());
    } else if (h < heap.front()) {
      std::pop_heap(heap.begin(), heap.end());
      heap.back() = h;
      std::push_heap(heap.begin(), heap.end());
    }
  }
  std::sort(heap.begin(), heap.end());
  return heap;
}

float CdcOpQueue::SigSimilarity(const Sig &a, const Sig &b) {
  if (a.empty() || b.empty())
    return 0.0f;
  int common = 0, i = 0, j = 0;
  while (i < (int)a.size() && j < (int)b.size()) {
    if (a[i] == b[j]) {
      ++common;
      ++i;
      ++j;
    } else if (a[i] < b[j]) {
      ++i;
    } else {
      ++j;
    }
  }
  // Normalize by the larger signature size (Jaccard estimate for bottom-K MinHash).
  return (float)common / std::max((int)a.size(), (int)b.size());
}

void CdcOpQueue::EmitSegment(int di0, int di1, int ii0, int ii1,
                              const std::vector<Sig> &del_sigs,
                              const std::vector<Sig> &ins_sigs) {
  const int nd = di1 - di0;
  const int ni = ii1 - ii0;
  if (nd == 0) {
    for (int ii = ii0; ii < ii1; ii++)
      output_.push_back(std::move(inserts_[ii]));
    return;
  }
  if (ni == 0) {
    for (int di = di0; di < di1; di++)
      output_.push_back(std::move(deletes_[di]));
    return;
  }

  // Order-preserving approximate matching via DP (weighted LCS).
  // dp[a*W+b] = best total similarity using deletes_[di0..di0+a) paired with
  // inserts_[ii0..ii0+b). choice records the decision at each cell.
  static const float kMinSimilarity = 0.1f;
  const int W = ni + 1;
  std::vector<float> dp((nd + 1) * W, 0.0f);
  std::vector<uint8_t> choice((nd + 1) * W, 0); // 0=skip-d 1=skip-i 2=match

  for (int a = 1; a <= nd; a++) {
    for (int b = 1; b <= ni; b++) {
      float best = dp[(a - 1) * W + b];
      uint8_t ch = 0;
      if (dp[a * W + (b - 1)] > best) {
        best = dp[a * W + (b - 1)];
        ch = 1;
      }
      float sim = SigSimilarity(del_sigs[di0 + a - 1], ins_sigs[ii0 + b - 1]);
      if (sim >= kMinSimilarity && dp[(a - 1) * W + (b - 1)] + sim > best) {
        best = dp[(a - 1) * W + (b - 1)] + sim;
        ch = 2;
      }
      dp[a * W + b] = best;
      choice[a * W + b] = ch;
    }
  }

  // Backtrack to collect matched pairs (in reverse order, then reversed).
  std::vector<std::pair<int, int>> matches;
  for (int a = nd, b = ni; a > 0 && b > 0;) {
    switch (choice[a * W + b]) {
    case 2:
      matches.push_back(std::make_pair(di0 + a - 1, ii0 + b - 1));
      --a;
      --b;
      break;
    case 0:
      --a;
      break;
    default:
      --b;
      break;
    }
  }
  std::reverse(matches.begin(), matches.end());

  // Emit: place each matched pair as adjacent DELETE+INSERT so GraphOpQueue
  // diffs them against each other; interleave remaining unmatched chunks.
  int di = di0, ii = ii0;
  for (int m = 0; m < (int)matches.size(); m++) {
    int mdi = matches[m].first;
    int mii = matches[m].second;
    while (di < mdi || ii < mii) {
      if (di < mdi)
        output_.push_back(std::move(deletes_[di++]));
      if (ii < mii)
        output_.push_back(std::move(inserts_[ii++]));
    }
    output_.push_back(std::move(deletes_[di++]));
    output_.push_back(std::move(inserts_[ii++]));
  }
  while (di < di1 || ii < ii1) {
    if (di < di1)
      output_.push_back(std::move(deletes_[di++]));
    if (ii < ii1)
      output_.push_back(std::move(inserts_[ii++]));
  }
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
        lcs.push_back(std::make_pair(di, ii));
        di_used[di] = true;
        min_di = di + 1;
        break;
      }
    }
  }

  // Compute bottom-K MinHash signatures for all chunks (used by EmitSegment
  // to pair approximately similar unmatched chunks for graph diffing).
  std::vector<Sig> del_sigs(deletes_.size());
  std::vector<Sig> ins_sigs(inserts_.size());
  for (int i = 0; i < (int)deletes_.size(); i++)
    del_sigs[i] = ChunkSig(deletes_[i].GetValue().get(), deletes_[i].GetLength());
  for (int i = 0; i < (int)inserts_.size(); i++)
    ins_sigs[i] = ChunkSig(inserts_[i].GetValue().get(), inserts_[i].GetLength());

  // Generate the op sequence from the LCS alignment.
  // Between exact-match anchors: use approximate matching to pair similar
  // unmatched chunks adjacent (DELETE then INSERT) for graph diffing.
  // At each exact-match anchor: emit NEXT (no diffing needed).
  int di = 0, ii = 0;
  for (int m = 0; m < (int)lcs.size(); m++) {
    int lcs_di = lcs[m].first, lcs_ii = lcs[m].second;
    EmitSegment(di, lcs_di, ii, lcs_ii, del_sigs, ins_sigs);
    output_.emplace_back(Op::NEXT, deletes_[lcs_di].GetLength());
    di = lcs_di + 1;
    ii = lcs_ii + 1;
  }
  EmitSegment(di, (int)deletes_.size(), ii, (int)inserts_.size(), del_sigs, ins_sigs);
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
