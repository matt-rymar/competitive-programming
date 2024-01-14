// Computes the suffix array as well as the LCP array of an arbitrary string in
// O(nlgn) time.
//
// Afterwards, can answer LCP queries about two arbitrary suffixes in O(1) time
// (with O(nlgn) precomputation time).
//
// Note that LCP[i] stores the LCP of i-th and (i+1)th suffix, *not* i and
// (i-1)th as is also sometimes done.
//
// For an explanation of the algorithm, see
// https://cp-algorithms.com/string/suffix-array.html.
//
// WARNING: the LCP functionality currently does not work in the cyclic mode.
// WARNING: The mode endLarge has NOT been properly tested.
//
// For an example, see Codeforces Task 1073G--Yet Another LCP Problem.
class SuffixArray {
 public:
  // Handles how the end of the string is treated. That is, given two suffixes
  // "aa" and "a", which one should be smaller?
  enum class Mode {
    // Standard lexicographic ordering, i.e. if one string is shorter than the
    // other and matches all the way, it's smaller. That is, with that we will
    // have "a" < "aa".
    endSmall,

    // The opposite of endSmall, that is, "aa" < "a".
    //
    // BEWARE: not tested.
    endLarge,

    // The input string is treated as cyclic, so if our input string is "abcb",
    // then the "suffix" starting at position 1 is "bcba", while the "suffix"
    // starting at position 3 is "babc".
    cyclic,
  };

  SuffixArray(std::string_view sv, Mode mode) {
    suffArr = std::move(suffixLogSquared(sv, mode));
    constructLcp(sv);
  }

  // Returns the longest common prefix between suffixes beginning at i and j.
  int longestCommonPrefix(int i, int j) const {
    static auto rmq = RMQ(lcpArr);

    if (i == j)
      return rank.size() - i;

    const auto [lo, hi] = std::minmax(rank[i], rank[j]);
    return rmq.getMin(lo, hi - 1);
  }

  int stringSize() const { return rank.size(); }

  std::vector<int> rank;
  std::vector<int> suffArr;
  std::vector<int> lcpArr;

 private:
  // Note: obviously, we can never have isEndSmall == isEndLarge == true, but
  // they're not always opposites if the mode is cyclic, then we'll have
  // isEndSmall == isEndLarge == false.
  std::vector<int> getInitialSorted(std::string_view sv, const bool isEndSmall,
                                    const bool isEndLarge) const {
    // Note that (at least) one of isEndSmall and isEndLarge is always false.
    auto sorted = std::vector<int>(sv.size() + isEndSmall + isEndLarge);
    std::iota(sorted.begin() + isEndSmall, sorted.end(), 0);
    std::sort(sorted.begin() + isEndSmall, sorted.end() - isEndLarge,
              [&](auto i, auto j) { return sv[i] < sv[j]; });
    if (isEndSmall)
      sorted[0] = sv.size();
    return sorted;
  }

  // Sorts the 1-character substrings of sv and initializes the equivalence
  // classes between them.
  // If the mode is not cyclic (isEndSmall || isEndLarge == true), it also
  // appropriately adjusts the return values so that the other procedures
  // correctly handle that mode.
  std::tuple<std::vector<int>, std::vector<int>, int> suffixInit(
      std::string_view sv, const bool isEndSmall, const bool isEndLarge) const {
    auto sorted = getInitialSorted(sv, isEndSmall, isEndLarge);

    auto equivClass = std::vector<int>(sv.size() + isEndSmall + isEndLarge, -1);
    int curClass = 0;
    equivClass[sorted[0]] = curClass;
    if (isEndSmall)
      equivClass[sorted[1]] = ++curClass;
    for (int i = 1 + isEndSmall; (i + isEndLarge) < sorted.size(); ++i) {
      if (sv[sorted[i - 1]] != sv[sorted[i]])
        ++curClass;
      equivClass[sorted[i]] = curClass;
    }
    if (isEndLarge)
      equivClass[sorted.back()] = ++curClass;

    return {sorted, equivClass, curClass + 1};
  }

  // Simple Suffix Array algorithm that uses std::sort() instead of counting
  // sort. Hence, it runs in O(nlog^2n) instead of O(nlogn).
  //
  // The method basically always computes the ordering of cyclic shifts instead
  // of of suffixes. So if the mode is not set to cyclic, it effectively reduces
  // the problem to cyclic shifts by adding a fictitious character at the end of
  // the string.
  std::vector<int> suffixLogSquared(std::string_view sv, Mode mode) const {
    if (sv.size() == 0)
      return {};

    const bool isEndSmall = (mode == Mode::endSmall);
    const bool isEndLarge = (mode == Mode::endLarge);
    const int size = sv.size() + isEndSmall + isEndLarge;

    auto [sorted, equivClass, classCount] =
        suffixInit(sv, isEndSmall, isEndLarge);

    // Since our string is cirular, we may have negative or large indices, which
    // the function below handles.
    auto idx = [size](int i) -> int {
      if (i >= size)
        return i - size;
      if (i < 0)
        return i + size;
      return i;
    };

    for (int halfLen = 1; halfLen < size; halfLen *= 2) {
      std::sort(sorted.begin(), sorted.end(), [&](auto i, auto j) {
        return std::tie(equivClass[idx(i - halfLen)], equivClass[i]) <
               std::tie(equivClass[idx(j - halfLen)], equivClass[j]);
      });

      auto newEquivClass = std::vector<int>(size, -1);
      int curClass = 0;
      sorted[0] = idx(sorted[0] - halfLen);
      newEquivClass[sorted[0]] = curClass;
      for (int i = 1; i < sorted.size(); ++i) {
        sorted[i] = idx(sorted[i] - halfLen);
        if (std::tie(equivClass[sorted[i - 1]],
                     equivClass[idx(sorted[i - 1] + halfLen)]) !=
            std::tie(equivClass[sorted[i]],
                     equivClass[idx(sorted[i] + halfLen)]))
          ++curClass;
        newEquivClass[sorted[i]] = curClass;
      }
      equivClass = std::move(newEquivClass);
    }

    // Get rid of the fictitious character if it had been added.
    if (isEndSmall)
      sorted.erase(sorted.begin());
    if (isEndLarge)
      sorted.pop_back();

    return sorted;
  }

  // Constructs the rank and LCP arrays assumming that suffArr has already been
  // initialized.
  // TODO: I think it would be easy enough to adjust the algorithm to work with
  // cyclic suffixes too (check for k < n instead of i+k < n and j+k < n), but
  // the code would get uglier from the branching.
  void constructLcp(std::string_view sv) {
    rank.resize(sv.size());
    for (int i = 0; i < sv.size(); ++i)
      rank[suffArr[i]] = i;

    lcpArr = std::vector<int>(sv.size(), -1);
    int k = 0;
    for (int i = 0; i < sv.size(); ++i) {
      if (rank[i] == (sv.size() - 1)) {
        k = 0;
        continue;
      }
      for (int j = suffArr[rank[i] + 1];
           ((i + k) < sv.size()) && ((j + k) < sv.size()) &&
           (sv[i + k] == sv[j + k]);
           ++k)
        ;
      lcpArr[rank[i]] = k;
      if (k > 0)
        --k;
    }
  }

  // Beware, while tweaking the class I changed only suffixLogSquared() so the
  // function below may be outdated.
  // Note, the implementation below is not optimized and as such doesn't really
  // have better performance than the O(nlog^2n) that uses std::sort(), for
  // input sizes common in competitive programming.
  /*void suffixLog(std::string_view sv)
  {
          auto [sorted, equivClass, classCount] = suffixInit(sv);

          for (int halfLen = 1; halfLen < sv.size(); halfLen *= 2) {
                  auto newClasses = std::vector<std::vector<int>>(classCount);
                  for (auto i : sorted) {
                          auto realIdx = idx(i - halfLen);
                          newClasses[equivClass[realIdx]].push_back(realIdx);
                  }
                  auto newEquivClass = equivClass;
                  sorted.clear();
                  int newClassCount = 0;
                  for (const auto& vec : newClasses) {
                          if (vec.empty())
                                  continue;
                          sorted.insert(sorted.end(), vec.cbegin(), vec.cend());
                          newEquivClass[vec.front()] = newClassCount;
                          for (int i = 1; i < vec.size(); ++i) {
                                  if (equivClass[idx(vec[i - 1] + halfLen)] !=
  equivClass[idx(vec[i] + halfLen)])
                                          ++newClassCount;
                                  newEquivClass[vec[i]] = newClassCount;
                          }
                          ++newClassCount;
                  }
                  equivClass = newEquivClass;
                  classCount = newClassCount;
          }

          suffArr = std::move(sorted);
  }*/
};