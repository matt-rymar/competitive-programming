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

std::vector<std::vector<int>> readGraph(int vertices, int edges, bool directed)
{
	auto g = std::vector<std::vector<int>>(vertices, std::vector<int>());
	for (int i = 0; i < edges; ++i) {
		int u, v;
		std::cin >> u >> v;
		--u; --v;
		g[u].push_back(v);
		if (!directed)
			g[v].push_back(u);
	}
	return g;
}

template <typename Weight = int>
std::vector<std::vector<std::pair<int, Weight>>> readGraphWeighted(int vertices, int edges, bool directed)
{
	auto g = std::vector<std::vector<std::pair<int, Weight>>>(vertices);
	for (int i = 0; i < edges; ++i) {
		int u, v;
		Weight w;
		std::cin >> u >> v >> w;
		--u; --v;
		g[u].emplace_back(v, w);
		if (!directed)
			g[v].emplace_back(u, w);
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

// Could be done better, see: https://stackoverflow.com/questions/2589096/find-most-significant-bit-left-most-that-is-set-in-a-bit-array
// Or: https://math.stackexchange.com/questions/3028227/least-and-most-significant-bit-calculation-using-bitwise-operations
// However, we only use the msb function once upon construction of the tree so idc
// Update: and now we don't use the function at all
template<typename T>
constexpr inline T msb(T x)
{
	// x & (x - 1) == x but with lsb unset
	while ((x & (x - 1)) > 0)
		x &= (x - 1);
	return x;
}

template<typename T>
constexpr inline T lsb(T x)
{
	// To get how this works, think about 2's complement
	return (x & (-x));
}

// Note: the tree is 0-indexed and prefix sums are inclusive
// All operations (except for the construction) run in O(lgn)
template<typename T>
class Fenwick
{
public:
	Fenwick(int noElements) : size(noElements + 1), lastPower(msb(size)), tree(2 * lastPower + 1, 0) {} // Possible overflow if size too large

	Fenwick(const std::vector<T>& vec) : Fenwick(vec.size())
	{
		for (int i = 0; i < vec.size(); ++i)
			add(i, vec[i]);
	}

	// Returns the prefix sum of [0, idx]
	// Note that idx < 0 is a valid input, for which 0 will be returned
	T sum(int idx) const { return actuallySum(idx + 1); }

	// Adds k to the element at index idx
	void add(int idx, T k) { actuallyAdd(idx + 1, k); }

	// Returns the first idx for which sum(idx) >= target (or size of the original vector if no such idx exists)
	// WARNING: requires that the prefix sum array be monotonically non-decreasing (i.e. all entries >= 0)
	int lowerBound(T target) { return actualLowerBound(target) - 1; }

	// Returns the element at the given index
	T getElement(int idx) const { return sum(idx) - sum(idx - 1); }

	// Sets the element at the given index
	void setElement(int idx, T val) { const auto delta = val - getElement(idx); add(idx, delta); }

private:
	// The number of elements in the tree + 1 (the +1 is since we do 1-indexing so tree[0] is not a part of the tree)
	const int size;
	// Largest power of two occuring in the size of the tree
	const int lastPower;
	// Note that internally, the tree is 1-indexed
	// (Also, the constructor allocates too large a vector for powers of two, but it doesn't matter cause it still allocates O(n) space)
	std::vector<T> tree;

	T actuallySum(int idx) const
	{
		T sum = 0;
		for (; idx > 0; idx &= (idx - 1))
			sum += tree[idx];
		return sum;
	}

	void actuallyAdd(int idx, T k)
	{
		for (; idx < tree.size(); idx += lsb(idx))
			tree[idx] += k;
	}

	int actualLowerBound(const T target)
	{
		T sum = 0;
		int idx = 0;
		for (auto power = lastPower; power > 0; power >>= 1) {
			if ((sum + tree[idx | power]) < target) {
				sum += tree[idx | power];
				idx |= power;
			}
		}
		// Since our internal vector size may be larger than the original vector, 
		// we need to make sure the returned index is not too large
		return std::min(idx+1, size);
	}
};



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
		auto vec = readVecSz<i64>(n);

		auto tree = Fenwick(vec);
		auto findKing = [&]() -> int {
			i64 curSum = 0;
			for (;;) {
				auto idx = tree.lowerBound(2 * curSum);

				if (idx >= vec.size())
					return -2;

				curSum = tree.sum(idx);
				if (2 * vec[idx] == curSum)
					return idx;
			}
		};

		for (int i = 0; i < q; ++i) {
			int p, x;
			std::cin >> p >> x;
			--p;
			tree.add(p, x - vec[p]);
			vec[p] = x;
			std::cout << findKing() + 1 << '\n';
		}
	}
}