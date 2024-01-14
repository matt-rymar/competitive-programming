// Returns a sorted copy of the vector and the number of inversions (using the
// the default comparison).
std::pair<std::vector<int>, int64_t> countInversions(std::vector<int> vec) {
  // Merge-sort-based inversion counting alg (based on iterative merge sort).
  int64_t res = 0;
  auto prev = vec;
  for (int step = 1; step < vec.size(); step *= 2) {
    for (int beg = 0; (beg + step) < vec.size(); beg += 2 * step) {
      const int leftBound = beg + step;
      const int rightBound =
          std::min(static_cast<int>(vec.size()), beg + 2 * step);
      int i = beg;
      int m = beg;
      int n = leftBound;
      while ((m < leftBound) && (n < rightBound)) {
        if (prev[m] <= prev[n])
          vec[i++] = prev[m++];
        else {
          res += leftBound - m;
          vec[i++] = prev[n++];
        }
      }
      while (m < leftBound)
        vec[i++] = prev[m++];
      while (n < rightBound)
        vec[i++] = prev[n++];
    }
    using std::swap;
    // swap(prev, vec); // For some bizarre reason, writing swap(prev, vec) here
    // breaks the code (as in, the algorithm gives wrong results for large
    // inputs).
    prev = vec;
  }
  return {prev, res};
}

std::vector<std::pair<int, int>> tracedCopy(const std::vector<int>& arr) {
  auto res = std::vector<std::pair<int, int>>();
  res.reserve(arr.size());
  for (int i = 0; i < arr.size(); ++i)
    res.emplace_back(arr[i], i);
  return res;
}

// Returns a vector with all the inversion counts to the left (i.e. res[i] = no.
// of j < i s.t. arr[j] > arr[i]). Hence, we have \sum_i res[i] =
// countInversions(arr).second.
std::vector<int> getInversionCounts(const std::vector<int>& arr) {
  auto res = std::vector<int>(arr.size());
  auto vec = tracedCopy(arr);
  auto prev = vec;
  for (int step = 1; step < vec.size(); step *= 2) {
    for (int beg = 0; (beg + step) < vec.size(); beg += 2 * step) {
      const int leftBound = beg + step;
      const int rightBound =
          std::min(static_cast<int>(vec.size()), beg + 2 * step);
      int i = beg;
      int m = beg;
      int n = leftBound;
      while ((m < leftBound) && (n < rightBound)) {
        if (prev[m].first <= prev[n].first)
          vec[i++] = prev[m++];
        else {
          res[prev[n].second] += leftBound - m;
          vec[i++] = prev[n++];
        }
      }
      while (m < leftBound)
        vec[i++] = prev[m++];
      while (n < rightBound)
        vec[i++] = prev[n++];
    }
    using std::swap;
    // swap(prev, vec); // For some bizarre reason, writing swap(prev, vec) here
    // breaks the code (as in, the algorithm gives wrong results for large
    // inputs).
    prev = vec;
  }
  return res;
}

/*
Alternative version of inversion counting which uses Fenwick trees.
Basic idea: for each element in the sorted order ask: how many of the elements
before me in the sorted order were inversions?
int64_t countInversions(const std::vector<int>& vec)
{
        auto coolvec = sortedTracedCopy(vec, std::less<int>{});
        int64_t res = 0;
        auto tree = Fenwick<int>(coolvec.size() + 1);
        for (int i = 0; i < coolvec.size(); ++i) {
                res += i - tree.sum(coolvec[i].first);
                tree.add(coolvec[i].first, 1);
        }
        return res;
}
*/