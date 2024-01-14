#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0200r0.html
template<class Fun>
class y_combinator_result {
	Fun fun_;
public:
	template<class T>
	explicit y_combinator_result(T&& fun) : fun_(std::forward<T>(fun)) {}

	template<class ...Args>
	decltype(auto) operator()(Args &&...args) {
		return fun_(std::ref(*this), std::forward<Args>(args)...);
	}
};

template<class Fun>
decltype(auto) y_combinator(Fun&& fun) {
	return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

template <typename T>
std::vector<T> readVecSz(int sz)
{
	std::vector<T> res;
	res.reserve(sz);
	for (int i = 0; i < sz; ++i) {
		T tmp;
		std::cin >> tmp;
		res.push_back(std::move(tmp));
	}
	return res;
}

template <typename T>
std::vector<T> readVecSz(int sz, const std::function<T(T)>& f)
{
	std::vector<T> res;
	res.reserve(sz);
	for (int i = 0; i < sz; ++i) {
		T tmp;
		std::cin >> tmp;
		res.push_back(std::move(f(tmp)));
	}
	return res;
}

template <typename T>
std::vector<T> readVec()
{
	int sz;
	std::cin >> sz;
	return readVecSz<T>(sz);
}

template<typename T>
std::vector<T> readVec(const std::function<T(T)>& f)
{
	int sz;
	std::cin >> sz;
	return readVecSz<T>(sz, f);
}

template<typename T>
std::vector<std::vector<T>> readMatrixSz(int rows, int cols)
{
	std::vector<std::vector<T>> res;
	res.reserve(rows);
	for (int i = 0; i < rows; ++i)
		res.push_back(readVecSz<T>(cols));
	return res;
}

template<typename T>
std::vector<std::vector<T>> readMatrix()
{
	int m, n;
	std::cin >> m >> n;
	return readMatrixSz<T>(m, n);
}

std::vector<std::vector<int>> readGraphDir(int vertices, int edges)
{
	auto g = std::vector<std::vector<int>>(vertices, std::vector<int>());
	for (int i = 0; i < edges; ++i) {
		int u, v;
		std::cin >> u >> v;
		--u; --v;
		g[u].push_back(v);
	}
	return g;
}

std::vector<std::vector<int>> readGraphUn(int vertices, int edges)
{
	auto g = std::vector<std::vector<int>>(vertices, std::vector<int>());
	for (int i = 0; i < edges; ++i) {
		int u, v;
		std::cin >> u >> v;
		--u; --v;
		g[u].push_back(v);
		g[v].push_back(u);
	}
	return g;
}

template<typename T, typename U>
std::istream& operator>>(std::istream& is, std::pair<T, U>& p)
{
	is >> p.first >> p.second;
	return is;
}

struct custom_hash {
	static uint64_t splitmix64(uint64_t x) {
		// http://xorshift.di.unimi.it/splitmix64.c
		x += 0x9e3779b97f4a7c15;
		x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
		x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
		return x ^ (x >> 31);
	}

	size_t operator()(uint64_t x) const {
		static const uint64_t FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();
		return splitmix64(x + FIXED_RANDOM);
	}
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
	// https://stackoverflow.com/questions/8513911/how-to-create-a-good-hash-combine-with-64-bit-output-inspired-by-boosthash-co
	// (Sort of)
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct pair_hash {
	// https://stackoverflow.com/questions/32685540/why-cant-i-compile-an-unordered-map-with-a-pair-as-key
	// (Sort of)

	template <class T1, class T2>
	static std::size_t hash(const std::pair<T1, T2>& p)
	{
		auto h1 = custom_hash{}(std::hash<T1>{}(p.first));
		auto h2 = custom_hash{}(std::hash<T2>{}(p.second));

		hash_combine(h1, h2);
		return h1;
	}
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2>& p) const { return hash(p); }
};

// std::unordered_map<std::pair<int, int>, int, pair_hash> map;

namespace std
{
	template<typename T, typename U>
	struct hash<pair<T, U>>
	{
		std::size_t operator()(const pair<T, U>& p) const { return pair_hash::hash(p); }
	};
}

template<typename T, typename Comp = std::less<T>>
std::vector<T> sortedCopy(std::vector<T> vec, Comp comp = std::less<T>{})
{
	std::sort(vec.begin(), vec.end(), comp);
	return vec;
}

// Note: if two elements are equivalent, sortedTracedCopy preserves the original order of the items
//	(In other words, the sort performed is stable)
template<typename T, typename Comp = std::less<T>>
std::vector<std::pair<int, T>> sortedTracedCopy(const std::vector<T>& vec, Comp comp = std::less<T>{})
{
	std::vector<std::pair<int, T>> cpy;
	cpy.reserve(vec.size());
	for (int i = 0; i < vec.size(); ++i)
		cpy.emplace_back(i, vec[i]);
	std::stable_sort(cpy.begin(), cpy.end(),
		[&](const auto& p1, const auto& p2) {return comp(p1.second, p2.second); });
	return cpy;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto i : vec)
		os << i << ' ';
	return os;
}

//std::random_device dev;
//std::mt19937 rng(dev());
//std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 6);

template<typename T, typename U>
using P = std::pair<T, U>;
template<typename T>
using V = std::vector<T>;
using VI = V<int>;
template<typename T>
using VV = V<V<T>>;
using VVI = VV<int>;
template<typename T>
constexpr T NEG_INF = std::numeric_limits<T>::min();
template<typename T>
constexpr T POS_INF = std::numeric_limits<T>::max();
using i64 = int64_t;

template <typename T>
int logFloor(T n)
{
	if (n <= 0)
		return -1;
	int res = -1;
	while (n) {
		++res;
		n >>= 1;
	}
	return res;
}

// Allows us to provide a convenient default for RMQ operation while allowing the use of a general idempotent operation
template <typename T>
struct MinWrapper
{
	const T& operator()(const T& t1, const T& t2) const { return std::min(t1, t2); }
};

// Allows for O(1) range "minimum" queries over a sequence a_1, ..., a_n, for a binary idempotent operation op 
// Where "minimum" min(i, j) of range a_i,...,a_j, i <= j is defined to be
//		a_i, if i == j
//		op(a_i, min(a_{i+1}, ..., a_j)), if i < j)
template<typename T = int, typename BinaryIdempotentOp = MinWrapper<T>>
class RMQ {
public:
	// Creates an empty RMQ structure
	RMQ(BinaryIdempotentOp operation = MinWrapper<T>{})
		: op(std::move(operation))
	{ }

	// Automatically constructs the RMQ structure based on the sequence and the operation given
	RMQ(std::vector<T> vec, BinaryIdempotentOp operation = MinWrapper<T>{})
		: RMQ(operation)
	{
		construct(std::move(vec));
	}

	// (Re-) constructs the sparse table corresponding to the sequence in vec
	void construct(std::vector<T> vec)
	{
		fillLogs(vec.size() + 1);
		mins = std::vector<std::vector<T>>(logFloor(vec.size()) + 1, std::vector<T>(vec.size())),
			mins[0] = std::move(vec);
		for (int i = 1; i < mins.size(); ++i) {
			for (int j = 0; j < mins[i].size(); ++j) {
				const int upper = std::min(int(mins[i].size()) - 1, j + ((int(1)) << (i - 1)));
				mins[i][j] = op(mins[i - 1][j], mins[i - 1][upper]);
			}
		}
	}

	// Returns the "minimum" value (according to the operation) in the range [lo, hi]
	T getMin(const int lo, const int hi) const
	{
		const auto loggo = logFloor(hi - lo + 1);
		return op(mins[loggo][lo], mins[loggo][hi - (int(1) << loggo) + 1]);
	}

private:
	inline static std::vector<int> logs = std::vector<int>{ -1, 0 };

	// Our main sparse table of size O(nlogn) where first dimension are the lengths
	std::vector<std::vector<T>> mins;
	BinaryIdempotentOp op;

	// Fills the static logs vector so that it has size >= n+1 
	// (i.e. contains log-floors for (at least) all integers up to n inclusive)
	void fillLogs(int n)
	{
		for (int i = logs.size(); i <= n; ++i)
			logs.push_back(logs[i / 2] + 1);
	}
};

// Computes the SuffixArray as well as the LCP array in O(nlgn)
// Afterwards can answer LCP queries about two arbitrary suffixes in O(1) time
//	(with O(nlgn) precomputation time)
// WARNING: The mode endLarge has NOT been tested
class SuffixArray {
public:
	// Handles how the end of the string is treated
	// That is, given two suffixes "aa" and "a", which one should be smaller?
	enum class Mode {
		// Standard lexicographic ordering, i.e. if one string is shorter than the other and matches all the way, it's smaller
		// That is, with that we will have "a" < "aa"
		endSmall, 

		// The inverse of endSmall, that is, "aa" < "a"
		// BEWARE: not tested
		endLarge,

		// The input string is treated as cyclic, so if our input string is "abcb",
		//  then the substring of length 3 starting at position 1 is "bcb",
		//  while the substring of length 3 starting at position 3 is "bab"
		cyclic,
	};

	SuffixArray(std::string_view sv, Mode mode)
	{
		suffArr = std::move(suffixLogSquared(sv, mode));
		constructLcp(sv);
	}

	// Returns the longest common prefix between suffixes beginning at i and j
	int longestCommonPrefix(int i, int j) const
	{
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
	std::vector<int> getInitialSorted(std::string_view sv, const bool isEndSmall, const bool isEndLarge) const
	{
		// Note that (at least) one of isEndSmall and isEndLarge is always false
		auto sorted = std::vector<int>(sv.size() + isEndSmall + isEndLarge);
		std::iota(sorted.begin() + isEndSmall, sorted.end(), 0);
		std::sort(sorted.begin() + isEndSmall, sorted.end() - isEndLarge, [&](auto i, auto j) {return sv[i] < sv[j]; });
		if (isEndSmall)
			sorted[0] = sv.size();
		return sorted;
	}

	std::tuple<std::vector<int>, std::vector<int>, int> suffixInit(std::string_view sv, 
		const bool isEndSmall, const bool isEndLarge) const
	{
		auto sorted = getInitialSorted(sv, isEndSmall, isEndLarge);

		auto equivClass = std::vector<int>(sv.size() + isEndSmall, -1);
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

		return { sorted, equivClass, curClass + 1 };
	}

	// Simple Suffix Array algorithm that uses std::sort() instead of counting sort,
	//  therefore it runs in O(nlog^2n) instead of O(nlogn)
	// The method basically always computes the ordering of cyclic shifts instead of of suffixes.
	// So if the mode is not set to cyclic, it effectively reduces the problem to cyclic shifts
	//	by adding a fictitious character at the end of the string
	std::vector<int> suffixLogSquared(std::string_view sv, Mode mode) const
	{
		if (sv.size() == 0)
			return {};

		const bool isEndSmall = (mode == Mode::endSmall);
		const bool isEndLarge = (mode == Mode::endLarge);
		const int size = sv.size() + isEndSmall + isEndLarge;

		auto [sorted, equivClass, classCount] = suffixInit(sv, isEndSmall, isEndLarge);

		auto idx = [size](int i) -> int {
			if (i >= size)
				return i - size;
			if (i < 0)
				return i + size;
			return i;
		};

		for (int halfLen = 1; halfLen < size; halfLen *= 2) {
			std::sort(sorted.begin(), sorted.end(), [&](auto i, auto j)
				{return std::tie(equivClass[idx(i - halfLen)], equivClass[i]) < std::tie(equivClass[idx(j - halfLen)], equivClass[j]); });

			auto newEquivClass = std::vector<int>(size, -1);
			int curClass = 0;
			sorted[0] = idx(sorted[0] - halfLen);
			newEquivClass[sorted[0]] = curClass;
			for (int i = 1; i < sorted.size(); ++i) {
				sorted[i] = idx(sorted[i] - halfLen);
				if (std::tie(equivClass[sorted[i - 1]], equivClass[idx(sorted[i - 1] + halfLen)])
					!= std::tie(equivClass[sorted[i]], equivClass[idx(sorted[i] + halfLen)]))
					++curClass;
				newEquivClass[sorted[i]] = curClass;
			}
			equivClass = std::move(newEquivClass);
		}

		if (isEndSmall)
			sorted.erase(sorted.begin());
		if (isEndLarge)
			sorted.pop_back();

		return sorted;
	}

	// Beware, while tweaking the class I changed only suffixLogSquared() so the function below may be outdated
	// Note, the implementation below is not optimized and as such
	//  doesn't really have better performance than the O(nlog^2n) that uses std::sort() for input sizes common
	//  in competitive programming
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
					if (equivClass[idx(vec[i - 1] + halfLen)] != equivClass[idx(vec[i] + halfLen)])
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

	void constructLcp(std::string_view sv)
	{
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
				((i + k) < sv.size()) && ((j + k) < sv.size()) && (sv[i + k] == sv[j + k]); ++k)
				;
			lcpArr[rank[i]] = k;
			if (k > 0)
				--k;
		}
	}
};

std::vector<std::pair<int, std::pair<bool, int>>> combine(const SuffixArray& sa, const VI& as, const VI& bs)
{
	std::vector<P<int, P<bool, int>>> res;
	for (int i = 0; i < as.size(); ++i)
		res.emplace_back(as[i], std::make_pair(false, i));
	for (int i = 0; i < bs.size(); ++i)
		res.emplace_back(bs[i], std::make_pair(true, i));
	std::sort(res.begin(), res.end(),
		[&](const auto& p1, const auto& p2) {if (p1.first != p2.first) return sa.rank[p1.first] < sa.rank[p2.first]; return p2.second.first; });
	return res;
}

i64 solve(const SuffixArray& sa, const VI& as, const VI& bs)
{
	auto combined = combine(sa, as, bs);

	i64 res = 0;
	for (int i = 1; i < combined.size(); ++i) {
		if (combined[i - 1].first == combined[i].first)
			--res;
	}

	for (auto isA : { false, true }) {
		int v = 0;
		if (isA == combined[0].second.first)
			v = sa.stringSize() - combined[0].first;
		std::map<int, int> counts;
		if (v > 0)
			++counts[v];
		i64 sum = v;
		for (int i = 1; i < combined.size(); ++i) {
			v = std::min(v, sa.longestCommonPrefix(combined[i - 1].first, combined[i].first));
			while ((!counts.empty()) && ((counts.crbegin()->first) > v)) {
				sum -= (counts.crbegin()->first - v) * (counts.crbegin()->second);
				if (v > 0)
					counts[v] += counts.crbegin()->second;
				counts.erase(counts.crbegin()->first);
			}
			if (combined[i].second.first == isA) {
				v = sa.stringSize() - combined[i].first;
				sum += v;
				++counts[v];
			}
			if (combined[i].second.first != isA)
				res += sum;
		}
	}
	return res;
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout << std::setprecision(12);

	int t = 1;
	//std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int n, q;
		std::cin >> n >> q;
		std::string s;
		std::cin >> s;
		s.push_back('$');

		auto sa = SuffixArray(s, SuffixArray::Mode::cyclic);
		for (int i = 0; i < q; ++i) {
			int k, l;
			std::cin >> k >> l;
			auto as = readVecSz<int>(k, [](auto i) {return i - 1; });
			auto bs = readVecSz<int>(l, [](auto i) {return i - 1; });

			std::cout << solve(sa, as, bs) << '\n';
		}
	}
}