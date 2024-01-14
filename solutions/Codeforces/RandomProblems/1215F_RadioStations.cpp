#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
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
std::vector<T> readVectorSz(int sz)
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
std::vector<T> readVectorSz(int sz, const std::function<T(T)>& f)
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
std::vector<T> readVector()
{
	int sz;
	std::cin >> sz;
	return readVectorSz<T>(sz);
}

template<typename T>
std::vector<T> readVector(const std::function<T(T)>& f)
{
	int sz;
	std::cin >> sz;
	return readVectorSz<T>(sz, f);
}

template<typename T>
std::vector<std::vector<T>> readMatrixSz(int rows, int cols)
{
	std::vector<std::vector<T>> res;
	res.reserve(rows);
	for (int i = 0; i < rows; ++i)
		res.push_back(readVectorSz<T>(cols));
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

std::vector<std::vector<int>> getReverseAdj(const std::vector<std::vector<int>>& adj)
{
	auto res = std::vector<std::vector<int>>(adj.size());
	for (int i = 0; i < adj.size(); ++i) {
		for (auto j : adj[i])
			res[j].push_back(i);
	}
	return res;
}

void sccsForwardDfs(const std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::stack<int>& ord, int u)
{
	visited[u] = true;
	for (auto i : adj[u]) {
		if (visited[i])
			continue;
		sccsForwardDfs(adj, visited, ord, i);
	}
	ord.push(u);
}

void sccsBackwardDfs(const std::vector<std::vector<int>>& adj, std::vector<int>& who, int rep, int u)
{
	who[u] = rep;
	for (auto i : adj[u]) {
		if (who[i] >= 0)
			continue;
		sccsBackwardDfs(adj, who, rep, i);
	}
}

/*
// Doesn't do a true dfs, but rather kind of like a bfs but with a stack
//	That's enough for SCCs, though--we only need that all successors of a node are fully processed before it is
// DOESN'T WORK; DON'T USE
void sccsForwardDfsNonRec(const std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::stack<int>& ord, int u)
{
	visited[u] = true;
	std::stack<int> nodes;
	nodes.push(u);
	while (!nodes.empty()) {
		auto cur = nodes.top();
		for (auto i : adj[cur]) {
			if (visited[i])
				continue;
			nodes.push(i);
			visited[i] = true;
		}
		if (cur == nodes.top()) {
			ord.push(cur);
			nodes.pop();
		}
	}
}

void sccsBackwardBfs(const std::vector<std::vector<int>>& revAdj, std::vector<int>& who, int rep)
{
	who[rep] = rep;
	std::queue<int> q;
	q.push(rep);
	while (!q.empty()) {
		auto cur = q.front();
		q.pop();
		for (auto i : revAdj[cur]) {
			if (who[i] >= 0)
				continue;
			who[i] = rep;
			q.push(i);
		}
	}
}*/

std::pair<std::vector<int>, std::vector<int>> sccs(const std::vector<std::vector<int>>& adj)
{
	std::stack<int> order;
	auto visited = std::vector<bool>(adj.size(), false);
	for (int i = 0; i < adj.size(); ++i) {
		if (visited[i])
			continue;
		sccsForwardDfs(adj, visited, order, i);
		//sccsForwardDfsNonRec(adj, visited, order, i);
	}

	auto revAdj = getReverseAdj(adj);
	auto who = std::vector<int>(adj.size(), -1);
	std::vector<int> topOrder;
	while (!order.empty()) {
		auto cur = order.top();
		order.pop();
		if (who[cur] >= 0)
			continue;
		sccsBackwardDfs(revAdj, who, cur, cur);
		//sccsBackwardBfs(revAdj, who, cur);
		topOrder.push_back(cur);
	}
	return { who, topOrder };
}

inline int getNegVar(int x, int noVariables)
{
	if (x > noVariables)
		return x - noVariables;
	return x + noVariables;
}

// Returns the implication graph corresponding to the clause set
std::vector<std::vector<int>> getImplicationGraph(const std::vector<std::pair<int, int>>& clauses, int noVariables)
{
	// We need a vertex for each variable and its negation (plus one since our variables are in the range [1..n])
	// For a variable i, its negation is the vertex i+noVariables
	// Adj may contain duplicates but it shouldn't matter in the context of an implication graph
	//	(while I got MLEs when using std::unordered_set)
	auto adj = std::vector<std::vector<int>>(2 * noVariables + 1);
	for (auto [x, y] : clauses) {
		if (x < 0)
			x = -x + noVariables;
		if (y < 0)
			y = -y + noVariables;
		adj[getNegVar(x, noVariables)].push_back(y);
		adj[getNegVar(y, noVariables)].push_back(x);
	}
	return adj;
}

std::unordered_map<int, int> repToTopOrd(const std::vector<int>& topOrd)
{
	std::unordered_map<int, int> res;
	for (int i = 0; i < topOrd.size(); ++i)
		res[topOrd[i]] = i;
	return res;
}

// Solves the 2-Sat instance given in the vector
// Each entry of the input vector is a pair of variables, indexed in [1..n]
// If the variable >0 than the variable appears non-negated in the clause and otherwise it's negated
// Returns a satisfying assignment, if one exists--that is, it returns a vector v of size n+1 where
//	v[i] == true <=> i-th variable should be set to true
std::optional<std::vector<bool>> twoSat(const std::vector<std::pair<int, int>>& clauses, int noVariables)
{
	auto adj = getImplicationGraph(clauses, noVariables);
	auto [who, topSort] = sccs(adj);
	auto repToOrd = repToTopOrd(topSort);
	auto res = std::vector<bool>{ false };
	res.reserve(noVariables + 1);
	for (int i = 1; i <= noVariables; ++i) {
		auto neg = i + noVariables;
		if (who[i] == who[neg])
			return {};
		if (repToOrd[who[i]] < repToOrd[who[neg]])
			res.push_back(false);
		else
			res.push_back(true);
	}
	return res;
}

inline int freqGreaterIdx(int val, int p)
{
	return (p + 1) + (val - 2);
}

std::vector<std::pair<int, int>> getClauses(const std::vector<std::pair<int, int>>& complaints,
	const std::vector<std::pair<int, int>>& constraints,
	const std::vector<std::pair<int, int>>& interfers,
	const int M)
{
	auto res = std::vector<std::pair<int, int>>();
	const int p = constraints.size();

	for (auto [x, y] : complaints)
		res.emplace_back(x, y);
	for (auto [x, y] : interfers)
		res.emplace_back(-x, -y);
	for (int i = 0; i < p; ++i) {
		auto [l, r] = constraints[i];
		if (l > 1)
			res.emplace_back(-(i + 1), freqGreaterIdx(l, p));
		if (r < M)
			res.emplace_back(-(i + 1), -freqGreaterIdx(r + 1, p));
	}
	for (int i = 2; i < M; ++i)
		res.emplace_back(-freqGreaterIdx(i + 1, p), freqGreaterIdx(i, p));
	return res;
}

int getSigPow(const std::vector<bool>& sol, int firstFreq)
{
	int res = 1;
	for (int i = firstFreq; (i < sol.size()) && sol[i]; ++i)
		++res;
	return res;
}

std::optional<std::pair<int, std::vector<int>>> solve(std::vector<std::pair<int, int>>& complaints, std::vector<std::pair<int, int>>& constraints,
	std::vector<std::pair<int, int>>& interfers,
	const int M, const int p)
{
	auto clauses = getClauses(complaints, constraints, interfers, M);
	auto sol = twoSat(clauses, p + M - 1);
	if (!sol)
		return {};
	auto k = getSigPow(*sol, freqGreaterIdx(2, p));
	std::vector<int> res;
	for (int i = 1; i <= p; ++i) {
		if ((*sol)[i])
			res.push_back(i);
	}
	return { { k, res } };
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	//std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int n, p, M, m;
		std::cin >> n >> p >> M >> m;
		auto complaints = readVectorSz<std::pair<int, int>>(n);
		auto constraints = readVectorSz<std::pair<int, int>>(p);
		auto interfers = readVectorSz<std::pair<int, int>>(m);
		auto res = solve(complaints, constraints, interfers, M, p);
		if (!res)
			std::cout << -1 << '\n';
		else {
			std::cout << res->second.size() << ' ' << res->first << '\n';
			std::cout << res->second << '\n';
		}
	}
}