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

std::random_device dev;
std::mt19937 rng(dev());
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

template <typename T>
struct MinWrapper
{
	const T& operator()(const T& t1, const T& t2) const { return std::min(t1, t2); }
};

// Allows for O(1) range "minimum" queries over a sequence a_1, ..., a_n, for a binary idempotent operation op 
// Where "minimum" min(i, j) of range a_i,...,a_j is defined to be
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
		:  RMQ(operation)
	{
		construct(std::move(vec));
	}

	// (Re-) constructs the sparse table corresponding to the sequence in vec
	void construct(std::vector<T> vec)
	{
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
		auto loggo = logFloor(hi - lo + 1);
		return op(mins[loggo][lo], mins[loggo][hi - (int(1) << loggo) + 1]);
	}

private:
	std::vector<std::vector<T>> mins;
	BinaryIdempotentOp op;
};

// Supports O(1) LCA queries on a tree
class LCA
{
public:
	LCA(const std::vector<std::vector<int>>& adj, int root = 0)
		: firstVisit(adj.size(), -1), rmq(tourInfoMin)
	{
		tour(adj, root, -1, 0);
		rmq.construct(eulerTour);
	}

	int lca(int u, int v) const
	{
		if ((!inGraph(u)) || (!inGraph(v)))
			return -1;
		auto left = firstVisit[u], right = firstVisit[v];
		if (left > right)
			std::swap(left, right);
		return rmq.getMin(left, right).vertex;
	}

	int depth(int u) const
	{
		if (!inGraph(u))
			return -1;
		return eulerTour[firstVisit[u]].depth;
	}

private:
	// Since on for any pair of vertices in the graph, the minimum-depth node a path between them is unique,
	// we can use RMQ with the TourInfo struct and the "min" function below
	struct TourInfo
	{
		int vertex;
		int depth;
	};
	static constexpr auto tourInfoMin = 
		[](const TourInfo& t1, const TourInfo& t2) {if (t1.depth < t2.depth) return t1; return t2; };

	// Performs an Euler tour of the tree starting at cur with a given depth and parent
	void tour(const std::vector<std::vector<int>>& adj, const int cur, const int parent, const int depth)
	{
		if (firstVisit[cur] < 0)
			firstVisit[cur] = eulerTour.size();

		auto curInfo = TourInfo{ .vertex = cur, .depth = depth };
		eulerTour.push_back(curInfo);
		
		for (auto i : adj[cur]) {
			if (i == parent)
				continue;
			tour(adj, i, cur, depth + 1);
			eulerTour.push_back(curInfo);
		}
	}

	bool inGraph(int u) const { return (u >= 0) && (u < firstVisit.size()); }

	std::vector<TourInfo> eulerTour;
	std::vector<int> firstVisit;
	RMQ<TourInfo, decltype(tourInfoMin)> rmq;
};

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout << std::setprecision(12);

	int t = 1;
	//std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int n;
		std::cin >> n;
		auto edges = readVecSz<P<int, int>>(n - 1, [](auto p) {--p.first; --p.second; return p; });
		auto adj = VVI(n);
		for (auto [i, j] : edges) {
			adj[i].push_back(j);
			adj[j].push_back(i);
		}
		auto vec = readVec<P<int, int>>([](auto p) {--p.first; --p.second; return p; });

		auto lca = LCA(adj);
		auto balance = std::vector<int>(n, 0);
		for (auto [i, j] : vec) {
			auto ancestor = lca.lca(i, j);
			++balance[i]; ++balance[j];
			balance[ancestor] -= 2;
		}
		y_combinator([&](auto dfs, int cur, int par) -> void {
			for (auto i : adj[cur]) {
				if (i == par)
					continue;
				dfs(i, cur);
				balance[cur] += balance[i];
			}
		})(0, -1);

		for (auto [i, j] : edges) {
			auto higher = lca.lca(i, j);
			if (higher == i)
				i = j;
			std::cout << balance[i] << ' ';
		}
		std::cout << '\n';
	}
}