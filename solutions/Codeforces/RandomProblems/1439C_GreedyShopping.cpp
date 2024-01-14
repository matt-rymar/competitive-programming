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

class SegmentTree
{
public:
	SegmentTree(const int lo, const int hi, const std::vector<int64_t>& vec)
		: root(std::make_unique<Node>(lo, hi, vec))
	{ }

	int noItems(const int lo, const int64_t val) { return root->query(lo, POS_INF<int>, val).first; }
	void rangeMaxSet(const int hi, const int64_t val) { root->rangeSetMax(0, hi, val); }

private:
	struct Node {
	public:
		Node(const int lo, const int hi, const std::vector<int64_t>& vec)
			: leftmost(lo), rightmost(hi)
		{
			if (leftmost == rightmost) {
				min = max = sum = vec[lo];
				return;
			}
			const auto mid = (lo + hi) / 2;
			left = std::make_unique<Node>(lo, mid, vec);
			right = std::make_unique<Node>(mid + 1, hi, vec);
			recompute();
		}

		// [noItems, moneyLeft]
		std::pair<int, i64> query(const int lo, const int hi, const int64_t val)
		{
			if (isDisjoint(lo, hi) || (val < min))
				return { 0, val };
			if (coversUs(lo, hi) && (val >= sum))
				return { rangeLen(), val - sum };
			push();
			auto [noLeft, newVal] = getLeft()->query(lo, hi, val);
			auto [noRight, moneyLeft] = getRight()->query(lo, hi, newVal);
			return { noLeft + noRight, moneyLeft };
		}

		void rangeSetMax(const int lo, const int hi, const int64_t val)
		{
			if (isDisjoint(lo, hi) || (val <= min))
				return;
			if (coversUs(lo, hi) && (val >= max)) {
				max = min = val;
				sum = val * rangeLen();
				lazySetMax = val;
				return;
			}
			push();
			getLeft()->rangeSetMax(lo, hi, val);
			getRight()->rangeSetMax(lo, hi, val);
			recompute();
		}

	private:
		void recompute()
		{
			if (leftmost == rightmost)
				return;
			max = getLeft()->max;
			min = getRight()->min;
			sum = getLeft()->sum + getRight()->sum;
		}

		void push()
		{
			if ((leftmost == rightmost) || (lazySetMax < 0))
				return;
			getLeft()->max = getLeft()->min = getLeft()->lazySetMax
				= getRight()->max = getRight()->min = getRight()->lazySetMax
				= lazySetMax;
			getLeft()->sum = lazySetMax * getLeft()->rangeLen();
			getRight()->sum = lazySetMax * getRight()->rangeLen();
			lazySetMax = -1;
		}

		Node* getLeft()
		{
			//if ((!left) && (leftmost < rightmost))
			//	left = std::make_unique<Node>(leftmost, (leftmost + rightmost) / 2);
			return left.get();
		}

		Node* getRight()
		{
			//if ((!right) && (leftmost < rightmost))
			//	right = std::make_unique<Node>(((leftmost + rightmost) / 2) + 1, rightmost);
			return right.get();
		}

		bool isDisjoint(int lo, int hi) const { return (rightmost < lo) || (hi < leftmost); }
		bool coversUs(int lo, int hi) const { return (lo <= leftmost) && (rightmost <= hi); }
		int rangeLen() const { return rightmost - leftmost + 1; }

		const int leftmost, rightmost;
		int64_t min = 0;
		int64_t max = 0;
		int64_t sum = 0;
		int64_t lazySetMax = -1;
		std::unique_ptr<Node> left, right;
	};

	std::unique_ptr<Node> root;
};

// Old version with an ad-hoc segment tree

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

		auto tree = SegmentTree(0, n - 1, vec);
		for (int i = 0; i < q; ++i) {
			int t, x, y;
			std::cin >> t >> x >> y;
			--x;
			if (t == 1)
				tree.rangeMaxSet(x, y);
			else
				std::cout << tree.noItems(x, y) << '\n';
		}
	}
}