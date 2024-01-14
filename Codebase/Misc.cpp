// ------------------------------------------------------------
// Binary search stuff.
// ------------------------------------------------------------

// TODO: make the range bounds templated type?
// Finds the first index idx in [lo, hi] for which pred(idx) is true (or hi + 1
// if none found).
//
// Runs in O(lgn) time and makes O(lgn) calls to pred, where n = hi - lo + 1.
//
// Note: requires that pred be monotonically non-decreasing, i.e. there must
// exist a value x such that
//  pred(idx) == false for all idx in [lo, x], and
//  pred(idx) == true for all idx in (x, hi].
// (Note that in particular, this x is allowed to be <lo or >hi, that is, pred
// may be a constant function)
//
// WARNING: binary search is notorious for off-by-one errors and so far I've
// only tested the code below on a few tasks, so I do not have 100% confidence
// in its correctness.
// For an example, see:
//  --Codeforces 1486C2--Guessing the Greatest
//	--Codeforces 1487D--Pythagorean Triples
template <typename Pred>
int binarySearch(int lo, int hi, const Pred& pred) {
  while (lo < hi) {
    const auto mid = lo + (hi - lo) / 2;
    if (pred(mid))
      hi = mid;
    else
      lo = mid + 1;
  }
  if (pred(lo))
    return lo;
  return lo + 1;
}

// "Ternary search"-equivalent for a function f that is only defined on ints in
// the interval [lo, hi]. Finds an arbitrary local minimum of f (where we assume
// f(lo-1) == f(hi+1) == infinity). In particular, for a unimodal f, finds the
// global minimum.
//
// Runs in O(lg(hi-lo)) time, making just as many calls to f.
//
// For an example, see
//  --Codeforces 1480C--Searching Local Minimum
template <typename IntFunction>
int ternarySearch(const int lo, const int hi, const IntFunction& f) {
  const auto pred = [&](int i) {
    if (i == hi)
      return true;
    if (f(i) <= f(i + 1))  // Change to >= to find a local maximum.
      return true;
    return false;
  };
  // Note that technically speaking, pred does not fulfill the requirements of
  // binarySearch, but it will correctly guide the search nonetheless. This is
  // because we basically want to find any x such that !pred(x) && pred(x+1).
  return binarySearch(lo, hi, pred);
}

// ------------------------------------------------------------
// Codeforces interactive problem template stuff.
// ------------------------------------------------------------

struct Query {
  int lo, hi;

  // operator< just for the purpose of using std::map (yes Titus Winters, I know
  // you don't approve but this is Comp. Prog.)
  friend bool operator<(const Query& q1, const Query& q2) {
    return std::tie(q1.lo, q1.hi) < std::tie(q2.lo, q2.hi);
  }

  friend std::ostream& operator<<(std::ostream& os, const Query& q) {
    os << q.lo + 1 << ' ' << q.hi + 1;
    return os;
  }
};

struct Response {
  int val;

  friend std::istream& operator>>(std::istream& is, Response& r) {
    is >> r.val;
    --(r.val);
    return is;
  }
};

// Made for Codeforces interactive problems.
template <typename Query, typename Response>
Response interactMemoized(const Query& q) {
  static auto memo = std::map<Query, Response>();
  if (memo.count(q) > 0)
    return memo[q];

  std::cout << "? " << q << std::endl;
  Response r;
  std::cin >> r;
  memo[q] = r;
  return r;
}

// ------------------------------------------------------------
// Longest non-decreasing subsequence stuff.
// ------------------------------------------------------------

// Returns indices corresponding to the longest non-decreasing subsequence s.t.
// all elements are in the range [lo, hi].
//
// To go from non-decreasing to increasing, simply change the std::upper_bound
// to std::lower_bound.
std::vector<int> longestNonDecreasingSubsequence(
    const std::vector<int>& vec, int lo = std::numeric_limits<int>::min(),
    int hi = std::numeric_limits<int>::max()) {
  std::vector<int> frontElems;
  auto preds = std::vector<int>(vec.size(), -1);
  for (int i = 0; i < vec.size(); ++i) {
    if ((vec[i] < lo) || (vec[i] > hi))
      continue;
    if (frontElems.size() == 0) {
      frontElems.push_back(i);
      continue;
    }
    auto pos = std::upper_bound(frontElems.begin(), frontElems.end(), vec[i],
                                [&vec](auto x, auto y) { return x < vec[y]; });
    if (pos == frontElems.begin()) {
      frontElems.front() = i;
    } else {
      preds[i] = *(pos - 1);
      if (pos == frontElems.end())
        frontElems.push_back(i);
      else
        *pos = i;
    }
  }
  if (frontElems.size() == 0)
    return {};
  std::vector<int> res;
  for (int cur = frontElems.back(); cur >= 0; cur = preds[cur])
    res.push_back(cur);
  return std::vector<int>(res.crbegin(), res.crend());
}

// ------------------------------------------------------------
// Prev/next smaller stuff
// ------------------------------------------------------------

// For every i, res[i] = j, where j > i is the closest index j such that vec[i]
// >= vec[j] (or vec.size() if no such j exists).
//
// Runs in O(n) time.
template <typename T>
std::vector<int> nextSmaller(const std::vector<T>& vec) {
  auto res = std::vector<int>(vec.size(), vec.size());
  for (int i = (int)vec.size() - 2; i >= 0; --i) {
    auto j = i + 1;
    for (; (j < vec.size()) && (vec[i] < vec[j]);
         j = res[j])  // Change to vec[i] > vec[j] to get next (non-strict)
                      // larger, vec[i] <= vec[j] to get next strictly smaller,
                      // etc.
      ;
    res[i] = j;
  }
  return res;
}

// For every i, res[i] = j, where j < i is the closest index j such that vec[i]
// >= vec[j] (or -1 if no such j exists).
//
// Runs in O(n) time.
template <typename T>
std::vector<int> prevSmaller(const std::vector<T>& vec) {
  auto res = std::vector<int>(vec.size(), -1);
  for (int i = 1; i < vec.size(); ++i) {
    auto j = i - 1;
    for (; (j >= 0) && (vec[i] < vec[j]);
         j = res[j])  // Change to vec[i] > vec[j] to get prev (non-strict)
                      // larger, vec[i] <= vec[j] to get prev strictly smaller,
                      // etc.
      ;
    res[i] = j;
  }
  return res;
}

// ------------------------------------------------------------
// Find cycles in a permutation stuff.
// ------------------------------------------------------------

std::vector<int> getCycle(const std::vector<int>& permutation,
                          std::vector<bool>& visited, const int begIdx) {
  auto res = std::vector<int>{begIdx};
  visited[begIdx] = true;
  for (int i = permutation[begIdx]; i != begIdx; i = permutation[i]) {
    res.push_back(i);
    visited[i] = true;
  }
  return res;
}

// Given a permutation of [0..n-1], return all the cycles in it.
//
// For an example, see
// https://www.hackerrank.com/challenges/minimum-swaps-2/problem
std::vector<std::vector<int>> permutationCycles(
    const std::vector<int>& permutation) {
  std::vector<std::vector<int>> res;
  auto visited = std::vector<bool>(permutation.size(), false);
  for (int i = 0; i < permutation.size(); ++i) {
    if (visited[i])
      continue;
    res.push_back(getCycle(permutation, visited, i));
  }
  return res;
}

// ------------------------------------------------------------
// Shunting-Yard stuff (currently quite non-advanced still).
// ------------------------------------------------------------

int doOp(int x, int y, char op) {
  switch (op) {
    case '+':
      return x + y;
    case '-':
      return x - y;
    case '*':
      return x * y;
    case '/':
      return x / y;
  }

  return -1;  // Hopefully this doesn't happen.
}

// Gets rid of whitespace and unary pluses and minuses.
std::string normalize(std::string_view sv) {
  std::string res;
  for (int i = 0; i < sv.size(); ++i) {
    if (std::isspace(sv[i]))
      continue;

    const bool isUnaryOp =
        ((sv[i] == '+') || (sv[i] == '-')) &&
        (res.empty() || (!std::isdigit(res.back()) && (res.back() != ')')));
    if (isUnaryOp)
      res.push_back('0');

    res.push_back(sv[i]);
  }
  return res;
}

// Simple version of the shunting-yard alg, since we assume that the input is
// valid, only +-*/ are allowed, and that all operators are left-associative,
// that all intermediate results fit in an int.
//
// WARNING: handling of unary ops may be a bit weird if combined with other
// operators in edge cases like 1/-5 (what is it even supposed to mean?): in the
// current version this would get transformed to 1/0-5 which would obviously
// throw an error.
//
// For examples, see:
//  https://leetcode.com/problems/basic-calculator/
//  https://leetcode.com/problems/basic-calculator-ii/
int shuntingYard(std::string_view sv) {
  // The -1 priority on '(' ensures it is never accidentally popped off the
  // stack
  static const auto priorities =
      std::map<char, int>{{'(', -1}, {'+', 0}, {'-', 0}, {'*', 1}, {'/', 1}};

  std::vector<char> opStack;
  std::vector<int> numStack;

  auto doStackOp = [&]() {
    const auto [x, y] = std::array{numStack[numStack.size() - 2],
                                   numStack[numStack.size() - 1]};
    numStack.erase(numStack.end() - 2, numStack.end());
    numStack.push_back(doOp(x, y, opStack.back()));
    opStack.pop_back();
  };

  const auto normalized = normalize(sv);
  const auto normalizedSv =
      std::string_view(normalized);  // For convenient use with std::from_chars
  for (auto it = normalizedSv.cbegin(); it != normalizedSv.cend();) {
    const auto c = *it;
    if (std::isdigit(c)) {
      const auto numEnd = std::find_if_not(
          it, normalizedSv.cend(), [](auto c) { return std::isdigit(c); });
      int num;
      std::from_chars(it, numEnd, num);
      numStack.push_back(num);
      it = numEnd;
    } else if (c == '(') {
      opStack.push_back(c);
      ++it;
    } else if (c == ')') {
      while (opStack.back() != '(')
        doStackOp();
      opStack.pop_back();
      ++it;
    } else {
      while ((!opStack.empty()) &&
             (priorities.at(c) <= priorities.at(opStack.back())))
        doStackOp();
      opStack.push_back(c);
      ++it;
    }
  }

  while (!opStack.empty())
    doStackOp();

  return numStack.front();
}

// ------------------------------------------------------------
// Neat representation for a piecewise-linear convex/concave function stuff.
// ------------------------------------------------------------

// Represents a convex/concave function in a neat manner.
class PiecewiseLinear {
 public:
  // Change points stores the list of [where, howMuch] for every point where the
  // slope of the function changes.
  PiecewiseLinear(std::map<int, int> changePoints, int rightmostM,
                  int rightmostB)
      : changePoints(changePoints), rightM(rightmostM), rightB(rightmostB) {}

  // Adds the function "other" to us.
  //
  // Runs in O(other.changePoints.size()*log(changePoints.size() +
  // other.changePoints.size())) time.
  PiecewiseLinear& operator+=(const PiecewiseLinear& other) {
    rightM += other.rightM;
    rightB += other.rightB;
    for (auto [p, delta] : other.changePoints)
      changePoints[p] += delta;
    return *this;
  }

  // Returns a minimum of the function, if it exists.
  //
  // Runs in O(changePoints.size()) time
  std::optional<int> min() {
    if (auto opt = handleTrivialMin(); opt)
      return *opt;

    // Function is a non-trivial convex function; find the min by finding the
    // place where slope changes sign (Or more precisely, goes from positive to
    // non-positive) (Note that it may be the case that it never happens; in
    // that case, no minimum exists).
    int lastSlope = rightM;
    int lastB = rightB;
    auto ptIt = changePoints.crbegin();
    for (; ptIt != changePoints.crend(); ++ptIt) {
      int curSlope = lastSlope - ptIt->second;
      int curB = (lastSlope - curSlope) * ptIt->first + lastB;
      lastSlope = curSlope;
      lastB = curB;
      if (curSlope <= 0)
        break;
    }
    // Slope never turned negative; no minimum
    if (ptIt == changePoints.crend())
      return {};
    return lastSlope * (ptIt->first) + lastB;
  }

  std::optional<int> max() {
    if (auto opt = handleTrivialMax(); opt)
      return *opt;

    // Function is a non-trivial concave function; find the max by finding the
    // place where slope changes sign (Or more precisely, goes from negative to
    // non-negative) (Note that it may be the case that it never happens; in
    // that case, no maximum exists).
    int lastSlope = rightM;
    int lastB = rightB;
    auto ptIt = changePoints.crbegin();
    for (; ptIt != changePoints.crend(); ++ptIt) {
      int curSlope = lastSlope - ptIt->second;
      int curB = (lastSlope - curSlope) * ptIt->first + lastB;
      lastSlope = curSlope;
      lastB = curB;
      if (curSlope >= 0)
        break;
    }
    // Slope never turned negative; no minimum.
    if (ptIt == changePoints.crend())
      return {};
    return lastSlope * (ptIt->first) + lastB;
  }

 private:
  std::map<int, int> changePoints;
  // Intervals have the form y = mx + b; below we store the values for the
  // rightmost one.
  int rightM;
  int rightB;

  std::optional<std::optional<int>> handleTrivialMin() {
    // If the function is just a line, no minimum unless slope is zero.
    if (changePoints.size() == 0) {
      if (rightM == 0)
        return std::optional<int>{rightB};
      return std::optional<int>{};
    }
    // If the function is any non-constant concave function, no minimum.
    if ((changePoints.cbegin()->second) < 0)
      return std::optional<int>{};
    // If the function is convex but even the rightmost slope is negative, no
    // minimum.
    if (rightM < 0)
      return std::optional<int>{};
    // If the slope is zero at the rightmost part, it will be a minimum.
    if (rightM == 0)
      return std::optional<int>{rightB};
    return {};
  }

  std::optional<std::optional<int>> handleTrivialMax() {
    // If the function is just a line, no maximum unless slope is zero.
    if (changePoints.size() == 0) {
      if (rightM == 0)
        return std::optional<int>{rightB};
      return std::optional<int>{};
    }
    // If the function is any non-constant convex function, no maximum.
    if ((changePoints.cbegin()->second) > 0)
      return std::optional<int>{};
    // If the function is concave but even the rightmost slope is positive, no
    // maximum.
    if (rightM > 0)
      return std::optional<int>{};
    // If the slope is zero at the rightmost part, it will be a maximum.
    if (rightM == 0)
      return std::optional<int>{rightB};
    return {};
  }
};