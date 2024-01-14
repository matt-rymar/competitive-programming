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

bool afterCyclic(const std::vector<int>& after)
{
	enum class State {unvisited, visited, processed};
	auto visited = std::vector<State>(after.size(), State::unvisited);
	auto dfs = y_combinator([&](auto self, auto i) -> bool {
		if (visited[i] == State::processed)
			return true;
		visited[i] = State::visited;
		if (after[i] >= 0) {
			if (visited[after[i]] == State::visited)
				return false;
			if (!self(after[i]))
				return false;
		}
		visited[i] = State::processed;
		return true;
	});
	for (int i = 0; i < after.size(); ++i) {
		if (!dfs(i))
			return true;
	}
	return false;
}

std::vector<int> getReps(const std::vector<int>& before, const std::vector<int>& after) 
{
	auto reps = std::vector<int>(before.size(), -1);
	for (int i = 0; i < before.size(); ++i) {
		if (before[i] >= 0)
			continue;
		reps[i] = i;
		for (int j = after[i]; j >= 0; j = after[j])
			reps[j] = i;
	}
	return reps;
}

std::vector<std::vector<int>> getAdj(const std::vector<std::vector<int>>& fakeAdj, const std::vector<int>& reps)
{
	auto res = std::vector<std::vector<int>>(fakeAdj.size());
	for (int i = 0; i < fakeAdj.size(); ++i) {
		for (auto j : fakeAdj[i]) {
			if (reps[i] == reps[j])
				continue;
			res[reps[i]].push_back(reps[j]);
		}
	}
	return res;
}

bool checkChains(const std::vector<std::vector<int>>& fakeAdj, const std::vector<int>& after, const std::vector<int>& reps)
{
	for (int i = 0; i < fakeAdj.size(); ++i) {
		if (reps[i] != i)
			continue;
		std::unordered_set<int> seen;
		seen.insert(i);
		for (int j = after[i]; j >= 0; j = after[j]) {
			seen.insert(j);
			for (auto k : fakeAdj[j]) {
				if (seen.count(k))
					return false;
			}
		}
	}
	return true;
}

std::pair<int, std::vector<int>> getInDegrees(const std::vector<std::vector<int>>& adj, const std::vector<int>& reps)
{
	auto res = std::vector<int>(adj.size(), 0);
	int count = 0;
	for (int i = 0; i < reps.size(); ++i) {
		if (i != reps[i])
			continue;
		++count;
		for (auto j : adj[i])
			++res[reps[j]];
	}
	return { count, res };
}

std::optional<std::vector<int>> topologicalSort(const std::vector<std::vector<int>>& adj, const std::vector<int>& reps)
{
	auto [count, degrees] = getInDegrees(adj, reps);
	std::queue<int> q;
	for (int i = 0; i < reps.size(); ++i) {
		if ((i != reps[i]) || (degrees[i] > 0))
			continue;
		q.push(i);
	}

	std::vector<int> topSort;
	while (!q.empty()) {
		auto cur = q.front();
		q.pop();
		topSort.push_back(cur);
		for (auto i : adj[reps[cur]]) {
			--degrees[reps[i]];
			if (degrees[reps[i]] == 0)
				q.push(i);
		}
	}

	if (topSort.size() != count)
		return std::nullopt;
	return topSort;
}

std::optional<std::vector<int>> solve(const std::vector<std::vector<int>>& fakeAdj, 
	const std::vector<int>& before, const std::vector<int>& after)
{
	if (afterCyclic(after))
		return std::nullopt;
	auto reps = getReps(before, after);
	if (!checkChains(fakeAdj, after, reps))
		return std::nullopt;
	auto adj = getAdj(fakeAdj, reps);
	auto opt = topologicalSort(adj, reps);
	if (!opt)
		return std::nullopt;
	std::vector<int> res;
	res.reserve(reps.size());
	for (auto i : (*opt)) {
		for (int j = i; j >= 0; j = after[j])
			res.push_back(j);
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
		int n, k;
		std::cin >> n >> k;
		auto vec = readVecSz<int>(n, [](auto i) {return i - 1; });
		auto adj = std::vector<std::vector<int>>(n);
		for (int i = 0; i < vec.size(); ++i) {
			if (vec[i] < 0)
				continue;
			adj[vec[i]].push_back(i);
		}
		auto before = std::vector<int>(n, -1);
		auto after = before;
		bool badInput = false;
		for (int i = 0; i < k; ++i) {
			int x, y;
			std::cin >> x >> y;
			--x; --y;
			if ((after[x] >= 0) || (before[y] >= 0)) {
				badInput = true;
				break;
			}
			after[x] = y;
			before[y] = x;
		}
		if (badInput) {
			std::cout << 0 << '\n';
			continue;
		}
		if (auto opt = solve(adj, before, after); opt) {
			for (auto i : *opt)
				std::cout << i + 1 << ' ';
			std::cout << '\n';
		}
		else
			std::cout << 0 << '\n';
	}
}