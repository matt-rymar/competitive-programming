// ------------------------------------------------------------
// KMP Stuff.
// ------------------------------------------------------------

// TODO: instead of having a KMP for strings and for vectors, create one unified
// version for C++20 ranges.

std::vector<int> prefixFunction(std::string_view str) {
  std::vector<int> res(str.length(), -1);

  for (int i = 1, k = -1; i < str.length(); ++i) {
    for (; (k >= 0) && (str[k + 1] != str[i]);)
      k = res[k];
    if (str[k + 1] == str[i])
      ++k;
    res[i] = k;
  }

  return res;
}

std::size_t knuthMP(std::string_view str, std::string_view sub) {
  if (sub.length() == 0)
    return 0;

  auto pi = prefixFunction(sub);

  for (int i = 0, k = -1; i < str.length(); ++i) {
    for (; (k >= 0) && (sub[k + 1] != str[i]);)
      k = pi[k];
    if (sub[k + 1] == str[i])
      ++k;
    if (k == sub.length() - 1)
      return (i - k);
  }

  return std::string::npos;
}

// KMP, but for arbitrary vectors.
// Returns the pi array where pi[i] := length of the longest proper suffix of
// pattern[0..i] that is also a prefix of pattern.
template <typename T>
std::vector<int> getPiArray(const std::vector<T>& pattern) {
  auto res = std::vector<int>(pattern.size());

  int k = 0;
  for (int i = 1; i < pattern.size(); ++i) {
    while ((k > 0) && (pattern[k] != pattern[i]))
      k = res[k - 1];

    if (pattern[k] == pattern[i])
      ++k;

    res[i] = k;
  }

  return res;
}

// TODO: add support for ranges instead of this startIdx nonsense (can't wait
// for C++20 support on comp. prog. websites). Finds the first occurence of
// pattern in arr[startIdx:] in O(arr.size() - startIdx + pattern.size()) time.
// Return the index where the match starts, or -1 if none found.
//
// For an example, see Leetcode 1764. Form Array by Concatenating Subarrays of
// Another Array,
// https://leetcode.com/problems/form-array-by-concatenating-subarrays-of-another-array/
template <typename T>
int knuthMP(const std::vector<T>& arr, const std::vector<T>& pattern,
            int startIdx = 0) {
  const auto pi = getPiArray(pattern);

  int k = 0;
  for (int i = startIdx; i < arr.size(); ++i) {
    while ((k > 0) && (arr[i] != pattern[k]))
      k = pi[k - 1];

    if (arr[i] == pattern[k])
      ++k;

    if (k == pattern.size())
      return i - k + 1;
  }

  return -1;
}

// ------------------------------------------------------------
// Z-Function Stuff.
// ------------------------------------------------------------

// Returns the z-function array for the input string.
// Assumes that input string non-empty.
//
// Could be simplified so that the inner loop complexity is more obvious, I
// think: add after the first if condition and before the loop, add an if (i +
// res[i] < r) { continue; }. Then it's clear that the while loop will always
// examine new characters (except for the first iteration) and that the
// substring starting at i will extend to at least r, so we can safely skip the
// last if condition, too
std::vector<int> zFunction(std::string_view sv) {
  auto res = std::vector<int>(sv.size());
  res[0] = sv.size();
  int l = 0, r = 0;
  for (int i = 1; i < sv.size(); ++i) {
    if (i <= r)
      res[i] = std::min(res[i - l], r - i + 1);
    while ((i + res[i] < sv.size()) && (sv[i + res[i]] == sv[res[i]]))
      ++res[i];
    if ((i + res[i] - 1) > r) {
      l = i;
      r = i + res[i] - 1;
    }
  }
  return res;
}

// ------------------------------------------------------------
// Longest Palindromic Substring Stuff.
// ------------------------------------------------------------

// Adding breaks between characters allows us to only treat the case of
// odd-length palindromes.
std::string addBreaks(std::string_view sv) {
  std::string res{'|'};
  for (auto c : sv) {
    res.push_back(c);
    res.push_back('|');
  }
  return res;
}

std::string removeBreaks(std::string_view sv) {
  std::string res;
  for (auto c : sv) {
    if (c == '|')
      continue;
    res.push_back(c);
  }
  return res;
}

// Return an array res where res[i] is the "radius" of the palindrome centered
// at i, i.e. sv[i - res[i]..i + res[i]] is the largest palindrome centered at
// i.
std::vector<int> longestCenteredAt(std::string_view sv) {
  int center = 0;
  int rightMost = 0;
  std::vector<int> best(sv.size(), 0);
  int m = -1, n = -1;
  for (int i = 1; i < sv.size(); ++i) {
    if (i >= rightMost) {
      center = i;
      m = i - 1;
      n = i + 1;
    } else {
      if (best[2 * center - i] < (rightMost - i) ||
          ((best[2 * center - i] == (rightMost - i)) &&
           (rightMost == sv.size()))) {
        // The mirror position provides best solution; no need to check any
        // further.
        best[i] = best[2 * center - i];
        continue;
      } else {
        // The mirror position goes to the edge of the current palindrome; we
        // need to check.
        best[i] = rightMost - i;
        center = i;
        m = i - best[i] - 1;
        n = i + best[i] + 1;
      }
    }
    // Find maximal palindrome centered at center.
    while ((m >= 0) && (n < sv.size()) && (sv[m] == sv[n])) {
      --m;
      ++n;
    }
    // (m, n) is a palindrome centered at c.
    rightMost = n - 1;
    best[center] = n - center - 1;
  }

  return best;
}

std::string longestPalindromicSubstring(std::string_view input) {
  auto s = addBreaks(input);
  auto vec = longestCenteredAt(s);
  auto longest = std::max_element(vec.cbegin(), vec.cend());
  auto center = std::distance(vec.cbegin(), longest);
  auto sv = std::string_view(s);
  return removeBreaks(sv.substr(center - *longest, 2 * (*longest) + 1));
}