// Could be improved, see:
// https://stackoverflow.com/questions/2589096/find-most-significant-bit-left-most-that-is-set-in-a-bit-array.
// Or:
// https://math.stackexchange.com/questions/3028227/least-and-most-significant-bit-calculation-using-bitwise-operations.
// However, we only use the msb function once upon construction of the tree so
// idc. Also, use stuff from <bit> when it becomes available.
template <typename T>
constexpr inline T msb(T x) {
  // x & (x - 1) == x but with lsb unset.
  while ((x & (x - 1)) > 0)
    x &= (x - 1);
  return x;
}

template <typename T>
constexpr inline T lsb(T x) {
  // To get how this works, think about 2's complement.
  return (x & (-x));
}

// Note: the tree is 0-indexed and prefix sums are inclusive.
//
// All operations (except for the construction) run in O(lgn).
//
// For an example, see Codeforces Task 992E--Nastya and King-Shamans.
template <typename T>
class Fenwick {
 public:
  Fenwick(int noElements)
      : size(noElements + 1),
        lastPower(msb(size)),
        tree(2 * lastPower + 1, 0) {}  // Possible overflow if size too large.

  Fenwick(const std::vector<T>& vec) : Fenwick(vec.size()) {
    for (int i = 0; i < vec.size(); ++i)
      add(i, vec[i]);
  }

  // Returns the prefix sum of [0, idx].
  // Note that idx < 0 is a valid input, for which 0 will be returned.
  T sum(int idx) const { return actuallySum(idx + 1); }

  // Adds k to the element at index idx
  void add(int idx, T k) { actuallyAdd(idx + 1, k); }

  // Returns the first idx for which sum(idx) >= target (or size of the original
  // vector if no such idx exists) WARNING: requires that the prefix sum array
  // be monotonically non-decreasing (i.e. all entries >= 0).
  int lowerBound(T target) { return actualLowerBound(target) - 1; }

  // Returns the element at the given index.
  // Note: this runs in O(lgn) time which is asymptotically optimal, but there
  // is a neater way to do it, which yields a better constant factor, see
  // https://www.topcoder.com/community/competitive-programming/tutorials/binary-indexed-trees/.
  T getElement(int idx) const { return sum(idx) - sum(idx - 1); }

  // Sets the element at the given index.
  void setElement(int idx, T val) {
    const auto delta = val - getElement(idx);
    add(idx, delta);
  }

 private:
  // The number of elements in the tree + 1 (the +1 is since we do 1-indexing so
  // tree[0] is not a part of the tree). Note that that is different from the
  // underlying structure size, i.e., size != tree.size().
  const int size;
  // Largest power of two occuring in the size of the tree.
  const int lastPower;
  // Note that internally, the tree is 1-indexed. (Also, the constructor
  // allocates too large a vector for powers of two, but it doesn't matter cause
  // it still allocates O(n) space.)
  std::vector<T> tree;

  T actuallySum(int idx) const {
    T sum = 0;
    for (; idx > 0; idx &= (idx - 1))
      sum += tree[idx];
    return sum;
  }

  void actuallyAdd(int idx, T k) {
    for (; idx < tree.size(); idx += lsb(idx))
      tree[idx] += k;
  }

  int actualLowerBound(const T target) {
    T sum = 0;
    int idx = 0;
    for (auto power = lastPower; power > 0; power >>= 1) {
      if ((sum + tree[idx | power]) <
          target) {  // Change to <= target to get upper bound... I think.
        sum += tree[idx | power];
        idx |= power;
      }
    }
    // Since our internal vector size may be larger than the original vector,
    // we need to make sure the returned index is not too large.
    return std::min(idx + 1, size);
  }
};