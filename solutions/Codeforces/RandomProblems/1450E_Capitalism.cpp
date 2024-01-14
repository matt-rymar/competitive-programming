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
using VV = V<V<T>>;
using VVI = VV<int>;
template<typename T>
constexpr T NEG_INF = std::numeric_limits<T>::min();
template<typename T>
constexpr T POS_INF = std::numeric_limits<T>::max();
using i64 = int64_t;

// Checks whether the (undirected) graph given by adj is bipartite
bool isBipartite(const std::vector<std::vector<int>>& adj)
{
	enum class Color {red, blue, none};
	auto colors = std::vector<Color>(adj.size(), Color::none);
	auto bfs = [&](int i) -> bool {
		std::queue<int> q;
		q.push(i);
		colors[i] = Color::red;
		while (!q.empty()) {
			auto cur = q.front();
			q.pop();
			for (auto i : adj[cur]) {
				if (colors[i] == colors[cur])
					return false;
				if (colors[i] == Color::none) {
					colors[i] = (colors[cur] == Color::red) ? Color::blue : Color::red;
					q.push(i);
				}
			}
		}
		return true;
	};
	for (int i = 0; i < adj.size(); ++i) {
		if (colors[i] != Color::none)
			continue;
		if (!bfs(i))
			return false;
	}
	return true;
}

// Returns the matrix of distances between each pair of nodes in the graph represented by the matrix of weights
// If a negative-weight cycle exists, returns a std::nullopt
std::optional<std::vector<std::vector<int>>> floydWarshall(std::vector<std::vector<int>> weightMatrix)
{
	// Just to be sure weightMatrix[i][i] is sensibly initialized
	for (int i = 0; i < weightMatrix.size(); ++i)
		weightMatrix[i][i] = 0;
	auto prev = weightMatrix;
	// Compute the shortest path lengths
	for (int i = 0; i < weightMatrix.size(); ++i) {
		for (int j = 0; j < weightMatrix.size(); ++j) {
			for (int k = 0; k < weightMatrix.size(); ++k) {
				weightMatrix[j][k] = prev[j][k];
				if ((prev[j][i] != POS_INF<int>) && (prev[i][k] != POS_INF<int>)
					&& ((prev[j][i] + prev[i][k]) < prev[j][k]))
					weightMatrix[j][k] = prev[j][i] + prev[i][k];
			}
		}
		prev = weightMatrix;
	}
	// Check for negative cycles by checking if we found a better path from a vertex to itself
	for (int i = 0; i < weightMatrix.size(); ++i) {
		if (prev[i][i] < 0)
			return std::nullopt;
	}
	return prev;
}

std::optional<std::pair<int, std::vector<int>>> solve(const std::vector<std::vector<int>>& adj, const std::vector<std::vector<int>>& adjMat)
{
	if (!isBipartite(adj))
		return std::nullopt;
	auto opt = floydWarshall(adjMat);
	if (!opt)
		return std::nullopt;
	auto dists = std::move(*opt);
	auto maxDist = NEG_INF<int>;
	int u = -1, v = -1;
	for (int i = 0; i < dists.size(); ++i) {
		for (int j = 0; j < dists.size(); ++j) {
			if ((i == j) || (dists[i][j] == POS_INF<int>))
				continue;
			if (dists[i][j] > maxDist) {
				maxDist = dists[i][j];
				u = i;
				v = j;
			}
		}
	}
	auto res = std::vector<int>(adj.size());
	for (int i = 0; i < adj.size(); ++i)
		res[i] = dists[u][i];
	return { {maxDist, res} };
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout << std::setprecision(12);

	int t = 1;
	//std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int n, m;
		std::cin >> n >> m;
		if (n == 1) {
			std::cout << "YES\n0\n0\n";
			continue;
		}
		auto adj = std::vector<std::vector<int>>(n);
		auto adjMat = std::vector<std::vector<int>>(n, std::vector<int>(n, POS_INF<int>));
		for (int i = 0; i < m; ++i) {
			int u, v, b;
			std::cin >> u >> v >> b;
			--u; --v;
			adj[u].push_back(v);
			adj[v].push_back(u);
			adjMat[u][v] = adjMat[v][u] = 1;
			if (b)
				adjMat[v][u] = -1;
		}
		if (auto opt = solve(adj, adjMat); opt) {
			std::cout << "YES\n";
			std::cout << opt->first << '\n';
			std::cout << opt->second << '\n';
		}
		else {
			std::cout << "NO\n";
		}
	}
}