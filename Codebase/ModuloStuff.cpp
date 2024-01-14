// Returns a list of primes in the range [2, n].
std::vector<int> primeSieve(const int n) {
  if (n < 2)
    return {};
  auto isPrime = std::vector<bool>(n + 1, true);
  isPrime[0] = isPrime[1] = false;
  for (int i = 4; i <= n; i += 2)
    isPrime[i] = false;
  auto res = std::vector<int>{2};
  for (int64_t i = 3; i <= n;
       i += 2) {  // Technically i*i <= n would be enough to find all primes,
                  // but since we need to also add primes > sqrt(n) into the
                  // vector, might as well do it in this loop.
    if (!isPrime[i])
      continue;
    res.push_back(i);
    for (int64_t j = i * i; j <= n; j += i)
      isPrime[j] = false;
  }
  return res;
}

// Returns a list of prime factors of n (those which are contained in "primes"),
// alongside with the power the factor is there with.
//
// Note: if not all prime factors are in primes, the remainder from dividing out
// all the primes is also attached at the end of the vector.
std::vector<std::pair<int64_t, int>> primeFactorization(
    const std::vector<int>& primes, int64_t n) {
  std::vector<std::pair<int64_t, int>> res;
  for (auto p : primes) {
    if (n <= 1)
      break;
    int cnt = 0;
    while (n % p == 0) {
      ++cnt;
      n /= p;
    }
    if (cnt > 0)
      res.emplace_back(p, cnt);
  }
  if (n > 1)
    res.emplace_back(n, 1);
  return res;
}

// Returns (d, x, y), where d = gcd(a, b), d = ax + by.
//
// Note: T should be a signed type to make sure the function works in a valid
// way: x and/or y may be negative.
template <typename T>
std::tuple<T, T, T> extendedEuclid(T a, T b) {
  T prevX = 1, x = 0, prevY = 0, y = 1;
  while (b != 0) {
    auto q = a / b;

    auto oldX = x;
    x = prevX - q * x;
    prevX = oldX;

    auto oldY = y;
    y = prevY - q * y;
    prevY = oldY;

    auto oldA = a;
    a = b;
    b = oldA % b;
  }

  return {a, prevX, prevY};
}

// Returns the inverse of a mod n (i.e. a^-1 (mod n)), precisely speaking the
// one in [0, n).
//
// Note: it assumes that such integer exists, i.e. gcd(a, n) = 1.
//
// Note: T should be a signed integer type to make sure the function works
// correctly.
template <typename T>
T modInverse(T a, T n) {
  auto [d, x, y] = extendedEuclid(a, n);
  // Make sure x \in [0, n)
  while (x < 0)
    x += n;
  return x % n;
}

// Returns a list of inverses of k modulo p, for k in [0, upTo) (or in [0, p) if
// upTo < 0) more efficiently than one-by-one.
//
// Note, 0 can't have an inverse so the entry for k = 0 is a meaningless value.
//
// Note, if p isn't prime, the result may be meaningless.
//
// See https://codeforces.com/blog/entry/5457?#comment-106714 for an explanation
// of this black magic. (Basically a derivation starting from the "inverse" of
// the desired value: p % i = p - (p / i) * i.)
template <typename T>
std::vector<T> modInverseRange(const T p, T upTo = -1) {
  if (upTo < 0)
    upTo = p;
  else
    upTo = std::min(p, upTo);

  std::vector<T> res{0, 1};
  res.reserve(upTo);
  for (int i = 2; i < upTo; ++i)
    res.push_back(((p - (((p / i) * res[p % i]) % p)) % p));
  return res;
}

// Computes (n choose k) % mod, given a vector of modular inverses (for example,
// the one computed by modInverseRange above).
template <typename T>
T nChooseKMod(const T n, const T k, const T mod,
              const std::vector<T>& modInverses) {
  T res = 1;
  for (T i = n; i > std::max(k, n - k); --i)
    res = (res * i) % mod;
  for (T i = 1; i <= std::min(k, n - k); ++i)
    res = (res * modInverses[i]) % mod;
  return res;
}

// Computes (n choose k) in the naive way which may easily overflow (even if the
// result wouldn't).
template <typename T>
T nChooseKNonMod(const T n, const T k) {
  T res = 1;
  for (T i = n; i > std::max(k, n - k); --i)
    res *= i;
  for (T i = 1; i <= std::min(k, n - k); ++i)
    res /= i;
  return res;
}

// Returns (a^b) mod n.
//
// Note: a, b assumed >= 0 and n assumed > 0
template <typename T>
T modExp(T a, T b, T n) {
  if (n == 1)
    return 0;
  T res = 1;
  a %= n;
  while (b > 0) {
    if ((b % 2) == 1)
      res = (res * a) % n;
    b /= 2;
    a = (a * a) % n;
  }
  return res;
}

// TODO: Move the free functions to static functions of ModNum? (see modExp for
// why it might be desirable).
// TODO: following opperators: unary +, - (unary and binary), -=, /, /=, ++,
// ++(int), --, --(int).
// TODO: Plenty other things, even basic stuff like comparison operators (I only
// add them as I need them for a task).
// TODO: Support for negative exponents in modExp?
// TODO: Cached modular inverse (something like a static std::map member
// variable), also something like modular inverse range with another cache,
// but this one a std::vector, for better speed (so mod inverses
// will be cached twice if used with modInverseRange, but whatever).
// TODO: Make all the relevant stuff constexpr.
//
// For examples, see
//	-- Codeforces 1462E2--Close Tuples (uses binomial coefficients)
//  -- Leetcode:
//  https://leetcode.com/problems/count-different-palindromic-subsequences
template <int MODULUS>
class ModNum {
 public:
  ModNum(const int64_t n) : value(getRep(n)) {}
  ModNum(const int n) : ModNum(static_cast<int64_t>(n)) {}
  ModNum(const uint32_t n) : ModNum(static_cast<int64_t>(n)) {}
  ModNum(const uint64_t n) : ModNum(static_cast<int64_t>(n % MOD)) {}

  // Returns the current value of the number.
  // Note: val() is always guaranteed to be in [0, MODULUS).
  int64_t val() const { return value; }

  friend ModNum& operator+=(ModNum& lhs, const ModNum& rhs) {
    lhs.value += rhs.value;
    if (lhs.value >= MOD)
      lhs.value -= MOD;
    return lhs;
  }
  friend ModNum operator+(ModNum lhs, const ModNum& rhs) {
    lhs += rhs;
    return lhs;
  }
  friend ModNum& operator-=(ModNum& lhs, const ModNum& rhs) {
    lhs.value -= rhs.value;
    if (lhs.value < 0)
      lhs.value += MOD;
    return lhs;
  }
  friend ModNum operator-(ModNum lhs, const ModNum& rhs) {
    lhs -= rhs;
    return lhs;
  }
  friend ModNum& operator*=(ModNum& lhs, const ModNum& rhs) {
    lhs.value = (lhs.value * rhs.value) % MOD;
    return lhs;
  }
  friend ModNum operator*(ModNum lhs, const ModNum& rhs) {
    lhs *= rhs;
    return lhs;
  }

  // Raises the current value to the given power (modulo given exponent).
  ModNum& setExp(const int64_t exponent) {
    *this = this->modExp(value, exponent);
    return *this;
  }
  // Returns the current number raised to the given power.
  ModNum exp(const int64_t exponent) const {
    auto cpy = *this;
    cpy.setExp(exponent);
    return cpy;
  }

  // STATIC METHODS.

  // Returns the representant of n in the range [0, MODULUS).
  static int64_t getRep(const int64_t n) {
    if (n < 0) {
      const auto res = MOD - ((-n) % MOD);
      if (res == MOD)
        return 0;
      return res;
    } else {
      return (n % MOD);
    }
  }

  // Returns (a^b) % MODULUS.
  static ModNum modExp(const int64_t a, const int64_t b) {
    return ModNum(::modExp(a, b, MOD));
  }

  // Returns a cache of modular inverses at least up to the given number (i.e.,
  // the returned vector has size >= upTo+1). If upTo < 0, computes the inverse
  // range for all numbers, that is, modInverseRange(-1) ==
  // modInverseRange(MODULUS-1).
  //
  // Note that while the entry at zero exists, it's  obviously meaningless.
  // Note that if MODULUS is not prime, the inverses may not exist and so other
  // entries in the table will also be meaningless.
  static const std::vector<ModNum>& modInverseRange(int upTo) {
    static std::vector<ModNum> cache{0, 1};

    if (upTo < 0)
      upTo = MOD - 1;
    // Should technically be (upTo + 1) but frankly the additional addition
    // instruction prolly costs more than the potential benefit.
    cache.reserve(upTo);
    while (cache.size() <= upTo) {
      const auto i = static_cast<int64_t>(cache.size());
      // Since inv can't be 0, the expression below won't yield inv == MOD, so
      // no need to check for it.
      const auto inv = MOD - (((MOD / i) * cache[MOD % i].value) % MOD);
      cache.emplace_back(inv);
    }
    return cache;
  }

  // Same as modInverseRange, except for factorials.
  static const std::vector<ModNum>& factRange(int upTo) {
    static std::vector<ModNum> cache{1};

    if (upTo < 0)
      upTo = MOD - 1;
    cache.reserve(upTo);
    while (cache.size() <= upTo)
      cache.push_back(cache.back() * cache.size());
    // Could cast cache.size() to int64_t to avoid double modulo in ModNum
    // constructor, but w/e, the above looks nicer.
    return cache;
  }

  // Same as modInverseRange, except for inverses of factorials (i.e., for
  // (n!)^(-1)).
  static const std::vector<ModNum>& factInverseRange(int upTo) {
    static std::vector<ModNum> cache{1};

    if (upTo < 0)
      upTo = MOD - 1;
    cache.reserve(upTo);
    const auto& invs = modInverseRange(upTo);
    while (cache.size() <= upTo)
      cache.push_back(cache.back() * invs[cache.size()]);
    return cache;
  }

  // Returns (n choose k) % MODULUS (assuming 0 <= k <= n).
  //
  // Due to caching, first execution takes O(n) time, all subsequent ones with
  // n' <= n are O(1).
  static ModNum nChooseK(const int n, const int k) {
    const auto& facts = factRange(n);
    const auto& invs = factInverseRange(n);
    return facts[n] * invs[n - k] * invs[k];
  }

 private:
  // Made MODULUS an int to make sure it is not too large.
  static constexpr int64_t MOD = MODULUS;

  int64_t value;
};

using CFModNum = ModNum<1'000'000'007>;