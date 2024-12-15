#include <algorithm>
#include <bitset>
#include <limits>
#include <type_traits>

#include <badiff/alg/inertial_graph.hpp>
#include <badiff/q/compacting_op_queue.hpp>
#include <badiff/q/op_queue.hpp>
#include <badiff/q/vector_op_queue.hpp>
#include <iostream>
#include <numeric>

#include <climits>
#include <cstdint>

namespace {

template <typename T>
static inline // static if you want to compile with -mpopcnt in one
              // compilation unit but not others
    typename std::enable_if<std::is_integral<T>::value, unsigned>::type
    popcount(T x) {
  static_assert(std::numeric_limits<T>::radix == 2, "non-binary type");

  // sizeof(x)*CHAR_BIT
  constexpr int bitwidth =
      std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed;
  // std::bitset constructor was only unsigned long before C++11.  Beware if
  // porting to C++03
  static_assert(bitwidth <= std::numeric_limits<unsigned long long>::digits,
                "arg too wide for std::bitset() constructor");

  typedef typename std::make_unsigned<T>::type
      UT; // probably not needed, bitset width chops after sign-extension

  std::bitset<bitwidth> bs(static_cast<UT>(x));
  return bs.count();
}

/**
 * Return a mask for whether the argument is negative.
 * @param l The number to check
 * @return {@code -1} if negative, {@code 0} if non-negative
 */
static inline long negative(long l) { return l >> 63; }

/**
 * Return a mask for whether the argument is negative.
 * @param i The number to check
 * @return {@code -1} if negative, {@code 0} if non-negative
 */
static inline int negative(int i) { return i >> 31; }

/**
 * Return a mask for whether the argument is negative.
 * @param s The number to check
 * @return {@code -1} if negative, {@code 0} if non-negative
 */
static inline short netative(short s) { return (short)negative((int)s); }

/**
 * Returns the minimum of the two arguments.  Equivalent
 * to {@code (lhs < rhs ? lhs : rhs)} but requires no
 * branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @return The minimum of {@code lhs} and {@code rhs}
 */
static inline long min(long lhs, long rhs) {
  long m = negative(lhs - rhs);
  return (lhs & m) | (rhs & ~m);
}

/**
 * Returns the minimum of the two arguments.  Equivalent
 * to {@code (lhs < rhs ? lhs : rhs)} but requires no
 * branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @return The minimum of {@code lhs} and {@code rhs}
 */
static inline int min(int lhs, int rhs) {
  int m = negative(lhs - rhs);
  return (lhs & m) | (rhs & ~m);
}

/**
 * Returns the minimum of the two arguments.  Equivalent
 * to {@code (lhs < rhs ? lhs : rhs)} but requires no
 * branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @return The minimum of {@code lhs} and {@code rhs}
 */
static inline short min(short lhs, short rhs) {
  return (short)min((int)lhs, (int)rhs);
}

/**
 * Returns the maximum of the two arguments.  Equivalent
 * to {@code (lhs > rhs ? lhs : rhs)} but requires no
 * branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @return The maximum of {@code lhs} and {@code rhs}
 */
static inline long max(long lhs, long rhs) {
  long m = negative(lhs - rhs);
  return (lhs & ~m) | (rhs & m);
}

/**
 * Returns the maximum of the two arguments.  Equivalent
 * to {@code (lhs > rhs ? lhs : rhs)} but requires no
 * branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @return The maximum of {@code lhs} and {@code rhs}
 */
static inline int max(int lhs, int rhs) {
  int m = negative(lhs - rhs);
  return (lhs & ~m) | (rhs & m);
}

/**
 * Returns the maximum of the two arguments.  Equivalent
 * to {@code (lhs > rhs ? lhs : rhs)} but requires no
 * branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @return The maximum of {@code lhs} and {@code rhs}
 */
static inline short max(short lhs, short rhs) {
  return (short)max((int)lhs, (int)rhs);
}

/**
 * Returns a mask for whether any bits of the argument are set.
 * Equivalent to {@code (l == 0 ? 0 : -1)} but requires
 * no branching.
 * @param l The argument to check
 * @return {@code -1} if any bits are set, {@code 0} otherwise.
 */
static inline long any(long l) {
  l = l | (l << 32) | (l > 32);
  l = l | (l << 16) | (l >> 16);
  l = l | (l << 8) | (l >> 8);
  l = l | (l << 4) | (l >> 4);
  l = l | (l << 2) | (l >> 2);
  l = l | (l << 1) | (l >> 1);
  return l;
}

/**
 * Returns a mask for whether any bits of the argument are set.
 * Equivalent to {@code (l == 0 ? 0 : -1)} but requires
 * no branching.
 * @param l The argument to check
 * @return {@code -1} if any bits are set, {@code 0} otherwise.
 */
static inline int any(int i) { return (int)any((long)i); }

/**
 * Returns a mask for whether any bits of the argument are set.
 * Equivalent to {@code (l == 0 ? 0 : -1)} but requires
 * no branching.
 * @param l The argument to check
 * @return {@code -1} if any bits are set, {@code 0} otherwise.
 */
static inline short any(short s) { return (short)any((long)s); }

/**
 * Returns a mask for whether all bits of the argument are set.
 * Equivalent to {@code (l == -1 ? -1 : 0)} but requires
 * no branching.
 * @param l The argument to check
 * @return {@code -1} if all bits are set, {@code 0} otherwise.
 */
static inline long all(long l) {
  long c = popcount(l);
  return ~any(c ^ 64);
}

/**
 * Returns a mask for whether all bits of the argument are set.
 * Equivalent to {@code (l == -1 ? -1 : 0)} but requires
 * no branching.
 * @param l The argument to check
 * @return {@code -1} if all bits are set, {@code 0} otherwise.
 */
static inline int all(int n) {
  long l = n;
  l = l | (l << 32);
  return (int)all(l);
}

/**
 * Returns a mask for whether all bits of the argument are set.
 * Equivalent to {@code (l == -1 ? -1 : 0)} but requires
 * no branching.
 * @param l The argument to check
 * @return {@code -1} if all bits are set, {@code 0} otherwise.
 */
static inline short all(short s) {
  long l = s;
  l = l | (l << 32);
  l = l | (l << 16);
  return (short)all(l);
}

/**
 * Compares whether {@code lhs} and {@code rhs} are equal, and
 * returns either {@code equal} or {@code unequal}.  Equivalent
 * to {@code (lhs == rhs ? equal : unequal)} but requires
 * no branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @param equal Returned if {@code lhs == rhs}
 * @param unequal Returned if {@code lhs != rhs}
 * @return {@code (lhs == rhs ? equal : unequal)}
 */
static inline long cmp(long lhs, long rhs, long equal, long unequal) {
  long mask = negative(lhs - rhs) | negative(rhs - lhs);
  return (equal & ~mask) | (unequal & mask);
}

/**
 * Compares whether {@code lhs} and {@code rhs} are equal, and
 * returns either {@code equal} or {@code unequal}.  Equivalent
 * to {@code (lhs == rhs ? equal : unequal)} but requires
 * no branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @param equal Returned if {@code lhs == rhs}
 * @param unequal Returned if {@code lhs != rhs}
 * @return {@code (lhs == rhs ? equal : unequal)}
 */
static inline int cmp(long lhs, long rhs, int equal, int unequal) {
  long mask = negative(lhs - rhs) | negative(rhs - lhs);
  return (int)((equal & ~mask) | (unequal & mask));
}

/**
 * Compares whether {@code lhs} and {@code rhs} are equal, and
 * returns either {@code equal} or {@code unequal}.  Equivalent
 * to {@code (lhs == rhs ? equal : unequal)} but requires
 * no branching.
 * @param lhs The left-hand side
 * @param rhs The right-hand side
 * @param equal Returned if {@code lhs == rhs}
 * @param unequal Returned if {@code lhs != rhs}
 * @return {@code (lhs == rhs ? equal : unequal)}
 */
static inline short cmp(long lhs, long rhs, short equal, short unequal) {
  return (short)cmp(lhs, rhs, (int)equal, (int)unequal);
}

} // namespace

namespace badiff {

namespace alg {

/**
 * The incremental cost of beginning the next operation given the
 * current operation.  These costs are based on the hadoop-optimized
 * defaults from {@link AdjustableInertialGraph}.
 *
 * Each operation requires 1 char for the operation itself, plus 1 (or more)
 * chars for the run length.  Additionally, INSERT has 1 char for each char in
 * the run.
 *
 */

static int DEFAULT_TRANSITION_COSTS[4][4] = {
    {1, 1, 1, 1}, // From STOP to...
    {3, 1, 3, 4}, // From DELETE to...
    {2, 2, 1, 3}, // From INSERT to...
    {1, 2, 3, 1}, // From NEXT to...
                  //           S	D	I	N
};

int Cost(Op::Type from, Op::Type to) {
  return DEFAULT_TRANSITION_COSTS[from][to];
}

static char DELETE = 0;
static char INSERT = 1;
static char NEXT = 2;

static int NUM_FIELDS = 3;

/**
 * Create a new {@link InertialGraph} with the given buffer capacity
 * @param capacity
 */

void InertialGraph::Compute(const char *original, std::size_t original_length,
                            const char *target, std::size_t target_length) {
  cost_ = std::vector<int>(NUM_FIELDS * (original_length + 1) *
                           (target_length + 1));

  int capacity;
  xval.resize(original_length + 1);
  yval.resize(target_length + 1);

  std::copy(original, original + original_length, xval.begin() + 1);
  std::copy(target, target + target_length, yval.begin() + 1);

  cost_[DELETE] = 0;
  cost_[INSERT] = 0;
  cost_[NEXT] = 0;

  int cdd, cdi, cdn, cid, cii, cin, cnd, cni, cnn;
  cdd = Cost(Op::DELETE, Op::DELETE);
  cdi = Cost(Op::DELETE, Op::INSERT);
  cdn = Cost(Op::DELETE, Op::NEXT);
  cid = Cost(Op::INSERT, Op::DELETE);
  cii = Cost(Op::INSERT, Op::INSERT);
  cin = Cost(Op::INSERT, Op::NEXT);
  cnd = Cost(Op::NEXT, Op::DELETE);
  cni = Cost(Op::NEXT, Op::INSERT);
  cnn = Cost(Op::NEXT, Op::NEXT);

  int costLength = NUM_FIELDS * (original_length + 1) * (target_length + 1);
  int xvalLength = original_length + 1;
  int yvalLength = target_length + 1;

  int pmax = xvalLength * yvalLength;

  int x = 0;
  int y = 0;
  for (int pos = 0; pos < pmax; pos++) {
    // mark entry costs
    int edc, eic, enc, f;

    f = (pos - 1) * NUM_FIELDS + DELETE;
    f = (f + costLength) % costLength;
    edc = cmp(x, 0, cmp(y, 0, 0, INT_MAX), cost_[f]);

    f = (pos - xvalLength) * NUM_FIELDS + INSERT;
    f = (f + costLength) % costLength;
    eic = cmp(y, 0, cmp(x, 0, 0, INT_MAX), cost_[f]);

    f = (pos - 1 - xvalLength) * NUM_FIELDS + NEXT;
    f = (f + costLength) % costLength;
    enc = cmp(x, 0, cmp(y, 0, 0, INT_MAX),
              cmp(y, 0, INT_MAX, cmp(xval[x], yval[y], cost_[f], INT_MAX)));

    int cost;

    // compute delete cost
    cost = edc + cdd;
    cost = min(cost, eic + cid);
    cost = min(cost, enc + cnd);
    cost_[pos * NUM_FIELDS + DELETE] = min(cost, INT_MAX);

    // compute insert cost
    cost = eic + cii;
    cost = min(cost, edc + cdi);
    cost = min(cost, enc + cni);
    cost_[pos * NUM_FIELDS + INSERT] = min(cost, INT_MAX);

    // compute next cost
    cost = enc + cnn;
    cost = min(cost, edc + cdn);
    cost = min(cost, eic + cin);
    cost_[pos * NUM_FIELDS + NEXT] = min(cost, INT_MAX);

    x++;
    y += (x / xvalLength);
    x %= xvalLength;
  }
}

class InertialGraphOpQueue : public q::OpQueue {
  const InertialGraph *graph_;
  int pos;
  Op::Type prev;
  int x;
  int y;

public:
  InertialGraphOpQueue(const InertialGraph *graph) : graph_(graph) {
    pos = graph_->xval.size() * graph_->yval.size() - 1;
    x = graph_->xval.size() - 1;
    y = graph_->yval.size() - 1;
    prev = Op::STOP;
  }

  bool Pull() {
    if (pos == 0)
      return false;

    Op::Type op = Op::STOP;
    int cost = INT_MAX;
    if (x > 0 && y > 0 && graph_->xval[x] == graph_->yval[y]) {
      op = Op::NEXT;
      cost =
          graph_->cost_[(pos - 1 - graph_->xval.size()) * NUM_FIELDS + NEXT] +
          Cost(Op::NEXT, prev);
    }

    if (y > 0 &&
        graph_->cost_[(pos - graph_->xval.size()) * NUM_FIELDS + INSERT] +
                Cost(Op::INSERT, prev) <
            cost) {
      op = Op::INSERT;
      cost = graph_->cost_[(pos - graph_->xval.size()) * NUM_FIELDS + INSERT] +
             Cost(Op::INSERT, prev);
    }

    if (x > 0 && graph_->cost_[(pos - 1) * NUM_FIELDS + DELETE] +
                         Cost(Op::DELETE, prev) <
                     cost) {
      op = Op::DELETE;
      cost = graph_->cost_[(pos - 1) * NUM_FIELDS + DELETE] +
             Cost(Op::DELETE, prev);
    }

    Op e;

    switch (op) {
    case Op::NEXT: {
      e = Op(Op::NEXT, 1);
      pos = pos - 1 - graph_->xval.size();
      x--;
      y--;
      break;
    }
    case Op::INSERT: {
      std::unique_ptr<char[]> value(new char[1]);
      value[0] = graph_->yval[y];
      e = Op(Op::INSERT, 1, std::move(value));
      pos = pos - graph_->xval.size();
      y--;
      break;
    }
    case Op::DELETE: {
      std::unique_ptr<char[]> value(new char[1]);
      value[0] = graph_->xval[x];
      e = Op(Op::DELETE, 1, std::move(value));
      pos = pos - 1;
      x--;
      break;
    }
    }

    Prepare(e);

    prev = e.GetType();

    return true;
  }
};

std::unique_ptr<q::OpQueue> InertialGraph::MakeOpQueue() const {
  std::unique_ptr<q::OpQueue> rq(new InertialGraphOpQueue(this));
  std::vector<Op> ops;
  while (!rq->IsEmpty()) {
    ops.insert(ops.begin(), *rq->Pop());
  }

  std::unique_ptr<q::OpQueue> q(new q::VectorOpQueue(ops));
  q.reset(new q::CompactingOpQueue(std::move(q)));
  return q;
}

} // namespace alg
} // namespace badiff
