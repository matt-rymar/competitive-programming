// TODO: accumulateWhole function? As a shorthand for query(0, sz-1).
// TODO: think about the exact axiomatic requirements on the predicate for the
// search (lowerBound/upperBound) functions.
// TODO: generally clean up lowerBound/upperBound.
// TODO: implicit segment tree (change children from unique_ptr to a proxy with
// overloaded -> and so on) => test on 1469F--Power Sockets.

// Segment Tree with lazy propagation that allows range queries and range
// updates in O(lgn) time.

// Note: Data should have a meaningful default constructor that returns a
// neutral element for the tree operation (that is, for any instance d of Data,
// d + Data() == Data() + d == d).
//
// Note: UpdateData also must have a default constructor. However, the
// default-initialized value does not matter and can be arbitrary.
//
// For an example use, see
//  -- Codeforces 380C--Sereja and Brackets (basic use, only queries and no
//  updates)
//  -- Codeforces 474F--Ant Colony (basic use, slightly more complex
//  accumulation)
//  -- Codeforces 1371F--Raging Thunder (only range queries and range updates,
//  but an elaborate structure to keep track of)
//	-- Codeforces 1439C--Greedy Shopping (query_if() and update_if() used)
//	-- Codeforces 1469F--Power Sockets (range updates, but also the search
// function lowerBound() used)
//	-- Codeforces 1478E--Nezzar And Binary String (uses getContents())
//  -- Codeforces 1491C--Pekora and Trampoline (simple range add/point query
//  problem)
template <typename Data, typename UpdateData>
class SegmentTree {
 public:
  // Constructs the segment tree on [0, sz) in O(nlgn) time, where init(i) is
  // the i-th entry in the range.
  //
  // WARNING: Cannot create an empty tree, i.e. we must have sz > 0.
  template <typename InitFn>
  SegmentTree(const InitFn& init, const int sz) : root(init, 0, sz - 1) {}

  // Accumulates the data in the range [lo, hi] in O(lgn) time.
  Data query(const int lo, const int hi) { return root.query(lo, hi); }

  // Lazily updates the range [lo, hi] in O(lgn) time.
  void update(const int lo, const int hi, const UpdateData& upd) {
    root.update(lo, hi, upd);
  }

  // Returns the data at idx.
  // Right now it doesn't do anything smarter than query(idx, idx).
  Data pointQuery(const int idx) { return query(idx, idx); }

  // Updates the entry at index idx.
  // Right now it doesn't do anything smarter than update(idx, idx, upd).
  void pointUpdate(const int idx, const UpdateData& upd) {
    update(idx, idx, upd);
  }

  // Same as query except only those parts of the range get accumulated, for
  // which the unary predicate pred(Data) == 1. Note that pred can also return
  // -1 in which case the whole segment gets immediately discarded without
  // further going into it. So to summarize: if the query visits a segment that
  // is the queried range covers (in which case normal query would just add it
  // to accumulation):
  //	if pred(Data) == 1, the segment gets added to the accumulation,
  //	if pred(Data) == -1, then the whole segment gets ignored,
  //	if pred(Data) == 0, then the query analyzes the subsegments (starting
  //    with the left half) to see if one of those is valid (or if we are a
  //    leaf, it gets ignored).
  //
  // Note that pred can also be a boolean predicate, in which case the early
  // exit feature simply won't do anything.
  //
  // Note that the running time now depends on the pred and may be
  // little-omega(lgn). For example, if pred(Data) == 0 for all data, then all
  // the leaves in the range [lo, hi] will be visited.
  template <typename UnaryPredicate>
  Data query_if(const int lo, const int hi, const UnaryPredicate& pred) {
    return root.query_if(lo, hi, pred);
  }

  // Updates parts of the range [lo, hi] where pred(data) == 1 (and early exits
  // on pred(data) == -1). For a more detailed description, see query_if, the
  // pred function has the same effect here.
  template <typename UnaryPredicate>
  void update_if(const int lo, const int hi, const UpdateData& upd,
                 const UnaryPredicate& pred) {
    root.update_if(lo, hi, upd, pred);
  }

  // Searches for the first index hi such that !pred(query(lo, hi), target).
  // If there is no such index, returns the length of the stored array (that is,
  // one past the last valid index).
  // Runs in O(lgn) time.
  //
  // Note: the function pred should be a binary predicate on Data (intuitively,
  // a "<" operator) that must define a strict weak ordering that is
  // monotonically increasing in interval length (so we must have
  // pred(query(lo, hi), query(lo, hi + k)) for all lo <= hi and all k > 0).
  //
  // WARNING: May run slow, may TLE despite good asymptotic complexity.
  template <typename BinaryPredicate>
  int lowerBound(const int lo, const Data& target,
                 const BinaryPredicate& pred) {
    auto [idx, acc] = root.lowerBound(lo, target, pred, Data());
    return idx;
  }

  // Same as lowerBound except returns first index hi such that pred(target,
  // query(lo, hi)). See lowerBound for more data on the predicate.
  //
  // WARNING: May run pretty slow, may TLE despite good asymptotic complexity
  template <typename BinaryPredicate>
  int upperBound(const int lo, const Data& target,
                 const BinaryPredicate& pred) {
    auto [idx, acc] = root.upperBound(lo, target, pred, Data());
    return idx;
  }

  // Returns a vector containg all of the data stored in the tree (i.e. all the
  // Data's in the leaf nodes, in order).
  //
  // Note: since it accesses all the nodes in the tree, it runs in O(nlgn) time.
  std::vector<Data> getContents() {
    std::vector<Data> res;
    root.getContents(res);
    return res;
  }

 private:
  class Node {
   public:
    template <typename InitFn>
    Node(const InitFn& init, const int lo, const int hi)
        : leftmost(lo), rightmost(hi) {
      if (leftmost == rightmost) {
        data = init(leftmost);
        return;
      }
      auto mid = lo + (hi - lo) / 2;
      left = std::make_unique<Node>(init, lo, mid);
      right = std::make_unique<Node>(init, mid + 1, hi);
      data = left->data + right->data;
    }

    // TODO: make this simply call query_if with a predicate which always
    // returns true? (It might make the constant factor of query() worse.)
    Data query(const int lo, const int hi) {
      if (isDisjoint(lo, hi))
        return Data();
      if (coversUs(lo, hi))
        return data;
      push();
      const auto leftAcc =
          left->query(lo, hi);  // Make sure to query the left segment first.
      return leftAcc + right->query(lo, hi);
    }

    // TODO: same as for query().
    void update(const int lo, const int hi, const UpdateData& upd) {
      if (isDisjoint(lo, hi))
        return;
      if (coversUs(lo, hi)) {
        accumulateUpdate(upd);
        return;
      }
      push();
      left->update(lo, hi, upd);
      right->update(lo, hi, upd);
      data = left->data + right->data;
    }

    template <typename UnaryPredicate>
    Data query_if(const int lo, const int hi, const UnaryPredicate& pred) {
      if (isDisjoint(lo, hi))
        return Data();
      if (coversUs(lo, hi)) {
        const auto predVal = pred(data);
        if (predVal < 0)
          return Data();
        if (predVal > 0)
          return data;
      }
      if (leftmost ==
          rightmost)  // If we are a leaf and get here, pred(data) == 0.
        return Data();
      push();
      const auto leftAcc = left->query_if(
          lo, hi, pred);  // Make sure to query the left segment first.
      return leftAcc + right->query_if(lo, hi, pred);
    }

    template <typename UnaryPredicate>
    void update_if(const int lo, const int hi, const UpdateData& upd,
                   const UnaryPredicate& pred) {
      if (isDisjoint(lo, hi))
        return;
      if (coversUs(lo, hi)) {
        const auto predVal = pred(data);
        if (predVal < 0)
          return;
        if (predVal > 0) {
          accumulateUpdate(upd);
          return;
        }
      }
      push();
      left->update_if(lo, hi, upd, pred);
      right->update_if(lo, hi, upd, pred);
      data = left->data + right->data;
    }

    // Return value is equal to {hi, acc}, where
    //	hi = first index such that !pred(query(lo, hi), target), or rightmost +
    //    1 if no such index exists;
    //  acc = query(lo, hi) (if hi == rightmost + 1, then returns query(lo,
    //    rightmost)).
    //
    // TODO: Update the comments above.
    // TODO: Make sure this works correctly for non-commutative Data::operator+.
    // TODO: Implement this with a Node* pointer so as to avoid constantly
    // passing the references to target and pred.
    template <typename BinaryPredicate>
    std::pair<int, Data> lowerBound(const int lo, const Data& target,
                                    const BinaryPredicate& pred,
                                    const Data& curAcc) {
      // We only consider sequences which begin somewhere in our interval (see
      // the standard case below to understand why checking for this is
      // convenient).
      if ((lo < leftmost) || (rightmost < lo))
        return {rightmost + 1, curAcc};

      // We're already good enough (this case may pop up if the empty interval
      // is good).
      if (!pred(curAcc, target))
        return {lo - 1, curAcc};

      const auto totalAcc = curAcc + data;

      // We are a leaf, return our index if we're good, our index + 1 if we
      // aren't.
      if (leftmost == rightmost)
        return {rightmost + pred(totalAcc, target), totalAcc};

      // Even our whole interval wouldn't be enough, so we can break early (here
      // we need the monotonicity of pred, which is crucial for the logarithmic
      // running time).
      if ((lo == leftmost) && pred(totalAcc, target))
        return {rightmost + 1, totalAcc};

      // Standard case: first check if the index is in our left half, if not,
      // check the right one.
      push();
      auto [leftIdx, leftAcc] = left->lowerBound(lo, target, pred, curAcc);
      if (leftIdx <= left->rightmost)
        return {leftIdx, leftAcc};

      return right->lowerBound(std::max(lo, right->leftmost), target, pred,
                               leftAcc);
    }

    // TODO: Improve the implementation to remove the duplication between
    // lowerBound and upperBound.
    template <typename BinaryPredicate>
    std::pair<int, Data> upperBound(const int lo, const Data& target,
                                    const BinaryPredicate& pred,
                                    const Data& curAcc) {
      // We only consider sequences which begin somewhere in our interval (see
      // the standard case below to understand why checking for this is
      // convenient).
      if ((lo < leftmost) || (rightmost < lo))
        return {rightmost + 1, curAcc};

      // We're already good enough (this case may pop up if the empty interval
      // is good).
      if (pred(target, curAcc))
        return {lo - 1, curAcc};

      const auto totalAcc = curAcc + data;

      // We are a leaf, return our index if we're good, our index+1 if we
      // aren't.
      if (leftmost == rightmost)
        return {rightmost + !pred(target, totalAcc), totalAcc};

      // Even our whole interval wouldn't be enough, so we can break early (here
      // we need the monotonicity of pred, which is crucial for the logarithmic
      // running time).
      if ((lo == leftmost) && !pred(target, totalAcc))
        return {rightmost + 1, totalAcc};

      // Standard case: first check if the index is in our left half, if not,
      // check the right one.
      push();
      auto [leftIdx, leftAcc] = left->upperBound(lo, target, pred, curAcc);
      if (leftIdx <= left->rightmost)
        return {leftIdx, leftAcc};

      return right->upperBound(std::max(lo, right->leftmost), target, pred,
                               leftAcc);
    }

    void getContents(std::vector<Data>& res) {
      if (leftmost == rightmost) {
        res.push_back(data);
        return;
      }
      push();
      left->getContents(res);
      right->getContents(res);
    }

   private:
    // The ordering of members is probably quite cache-unfriendly (that random
    // bool in the middle lol), but w/e, it's not like a pointer-based Segment
    // Tree will be a speed demon.
    const int leftmost, rightmost;
    std::unique_ptr<Node> left, right;
    Data data;
    bool lazySet = false;
    UpdateData lazyUpdate;

    bool coversUs(const int lo, const int hi) {
      return (lo <= leftmost) && (rightmost <= hi);
    }
    bool isDisjoint(const int lo, const int hi) {
      return (hi < leftmost) || (rightmost < lo);
    }
    int rangeLen() { return rightmost - leftmost + 1; }

    // Pushes our pending update onto our children.
    void push() {
      if (!lazySet)
        return;
      left->accumulateUpdate(lazyUpdate);
      right->accumulateUpdate(lazyUpdate);
      lazySet = false;
    }

    // Updates our current node and the pending update.
    void accumulateUpdate(const UpdateData& upd) {
      data += upd;
      if (lazySet)
        lazyUpdate += upd;
      else
        lazyUpdate = upd;
      lazySet = true;
    }
  };  // End struct Node

  Node root;
};

// Structure that stores all data necessary to update a segment (range).
//
// Note: UpdateData must have a default constructor. However, the
// default-initialized value does not matter and can be arbitrary.
//
// Note: The implementation below is an example for a simple segment tree
// supporting range sum queries. Change accordingly to suit your needs.
struct UpdateData {
  i64 val;

  // Combines two pending updates.
  friend UpdateData& operator+=(UpdateData& upd, const UpdateData& rhs) {
    upd.val += rhs.val;
    return upd;
  }
};

// Stores data about a segment.
//
// Note: Data should have a meaningful default constructor that returns a
// neutral element for the tree operation (that is, for any instance d of Data,
// d + Data() == Data() + d == d).
//
// Note: The implementation below is an example for a simple segment tree
// supporting range sum queries. Change accordingly to suit your needs.
struct Data {
  i64 sum = 0;
  i64 rangeLen = 0;

  // Main operator for the segment tree that is accumulated on.
  //
  // Note: this operator does *not* have to be commutative, but it *has to* be
  // associative.
  friend Data operator+(const Data& left, const Data& right) {
    return Data{
        .sum = left.sum + right.sum,
        .rangeLen = left.rangeLen + right.rangeLen,
    };
  }

  // Updates data about segment.
  friend Data& operator+=(Data& data, const UpdateData& upd) {
    data.sum += upd.val * data.rangeLen;
    return data;
  }
};

// Default Segment Tree (used with the data structures defined above).
using DefSegmentTree = SegmentTree<Data, UpdateData>;