#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <deque>
#include <functional>
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
	std::size_t operator () (const std::pair<T1, T2>& p) const {
		auto h1 = custom_hash{}(std::hash<T1>{}(p.first));
		auto h2 = custom_hash{}(std::hash<T2>{}(p.second));

		hash_combine(h1, h2);
		return h1;
	}
};

// std::unordered_map<std::pair<int, int>, int, pair_hash> map;

template<typename T = int>
struct Point {
	T x, y;

	friend bool operator==(const Point& lhs, const Point& rhs) { return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y); }
	friend bool operator!=(const Point& lhs, const Point& rhs) { return !(lhs == rhs); }
	friend std::ostream& operator<<(std::ostream& os, const Point& p)
	{
		os << p.x << ' ' << p.y;
		return os;
	}
	// Warning: no error handling
	friend std::istream& operator>>(std::istream& is, Point& p)
	{
		is >> p.x >> p.y;
		return is;
	}
};

template<typename T>
struct PointHash
{
	std::size_t operator()(const Point<T>& p) const { return pair_hash{}(std::make_pair(p.x, p.y)); }
};

template<typename T>
std::vector<T> sortedCopy(const std::vector<T> vec)
{
	auto cpy = vec;
	std::sort(cpy.begin(), cpy.end());
	return cpy;
}

template<typename T, typename Comp>
std::vector<T> sortedCopy(const std::vector<T> vec, Comp comp)
{
	auto cpy = vec;
	std::sort(cpy.begin(), cpy.end(), comp);
	return cpy;
}

template<typename T>
std::vector<std::pair<int, T>> sortedTracedCopy(const std::vector<T> vec)
{
	std::vector<std::pair<int, T>> cpy;
	cpy.reserve(vec.size());
	for (int i = 0; i < vec.size(); ++i)
		cpy.emplace_back(i, vec[i]);
	std::sort(cpy.begin(), cpy.end(),
		[](const auto& p1, const auto& p2) {return p1.second < p2.second; });
	return cpy;
}

template<typename T, typename Comp>
std::vector<std::pair<int, T>> sortedTracedCopy(const std::vector<T> vec, Comp comp)
{
	std::vector<std::pair<int, T>> cpy;
	cpy.reserve(vec.size());
	for (int i = 0; i < vec.size(); ++i)
		cpy.emplace_back(i, vec[i]);
	std::sort(cpy.begin(), cpy.end(),
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

template<typename T>
using V = std::vector<T>;
using VI = V<int>;
template<typename T>
using VV = std::vector<V<T>>;
template<typename T>
constexpr T NEG_INF = std::numeric_limits<T>::min();
template<typename T>
constexpr T POS_INF = std::numeric_limits<T>::max();
using i64 = int64_t;

class UnionFind
{
public:
	int getSet(int i)
	{
		if (exists(i))
			return findCompress(i);
		parent[i] = i;
		rank[i] = 0;
		size[i] = 1;
		return i;
	}

	int getSize(int i)
	{
		if (!exists(i))
			return 0;
		return size[getSet(i)];
	}

	void unionize(int i, int j)
	{
		if (i == j)
			return;
		auto root1 = getSet(i);
		auto root2 = getSet(j);
		if (root1 == root2)
			return;
		const auto sz = getSize(root1) + getSize(root2);
		if (rank[root1] > rank[root2])
			parent[root2] = root1;
		else if (rank[root1] < rank[root2])
			parent[root1] = root2;
		else {
			parent[root2] = root1;
			++rank[root1];
		}
		size[getSet(i)] = sz;
	}

	bool exists(int i)
	{
		return parent.count(i) > 0;
	}
private:
	std::unordered_map<int, int> parent;
	std::unordered_map<int, int> rank;
	std::unordered_map<int, int> size;

	int findCompress(int i)
	{
		// Iterative version
		std::vector<int> stack{ i };
		for (; parent[i] != i; i = parent[i])
			stack.push_back(parent[i]);
		auto root = stack.back();
		for (auto i : stack)
			parent[i] = root;
		return root;

		/* //RECURSIVE VERSION
		if (parent[i] == i)
			return i;
		parent[i] = findCompress(parent[i]);
		return parent[i];
		*/
	}
};

std::optional<std::pair<int, int>> getCycle_recursive(const std::vector<std::vector<int>>& adj, int cur, std::vector<int>& parent)
{
	for (auto i : adj[cur]) {
		if (i == parent[cur])
			continue;
		if (parent[i] >= 0)
			return { {i, cur} };
		parent[i] = cur;
		if (auto cyc = getCycle_recursive(adj, i, parent); cyc)
			return cyc;
	}
	return {};
}

std::optional<std::vector<int>> getCycle(const std::vector<std::vector<int>>& adj)
{
	auto parent = std::vector<int>(adj.size(), -1);
	std::optional<std::pair<int, int>> endpoints;
	for (int i = 0; i < adj.size(); ++i) {
		if (parent[i] >= 0)
			continue;
		parent[i] = i;
		endpoints = getCycle_recursive(adj, i, parent);
		if (endpoints)
			break;
	}
	if (!endpoints)
		return {};
	auto [from, to] = *endpoints;
	std::vector<int> res{ to };
	for (auto cur = parent[to]; res.back() != from; cur = parent[cur])
		res.push_back(cur);
	return res;
}

void bfs(const std::vector<std::vector<int>>& adj, int s, const std::unordered_set<int>& cycle, std::vector<bool>& visited, UnionFind& uf)
{
	visited[s] = true;
	std::queue<int> q;
	q.push(s);
	while (!q.empty()) {
		auto cur = q.front();
		q.pop();
		for (auto i : adj[cur]) {
			if (visited[i] || (cycle.count(i) > 0))
				continue;
			uf.unionize(i, cur);
			q.push(i);
			visited[i] = true;
		}
	}
}

UnionFind getComponents(const std::vector<std::vector<int>>& adj, const std::unordered_set<int>& cycle)
{
	auto visited = std::vector<bool>(adj.size());
	auto uf = UnionFind();
	for (auto i : cycle)
		bfs(adj, i, cycle, visited, uf);
	return uf;
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int64_t n;
		std::cin >> n;
		auto adj = readGraphUn(n, n);
		auto cycle = *getCycle(adj);
		auto cycleSet = std::unordered_set<int>{ cycle.cbegin(), cycle.cend() };
		auto uf = getComponents(adj, cycleSet);
		int64_t res = n * (n - 1);
		for (auto i : cycle) {
			auto set = uf.getSet(i);
			const int64_t sz = uf.getSize(set);
			res -= sz * (sz - 1) / 2;
		}
		std::cout << res << '\n';
	}
}