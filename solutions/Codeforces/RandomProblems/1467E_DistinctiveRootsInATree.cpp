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

struct EulerTourData
{
	EulerTourData(int n)
		: parent(n, -1), firstVisit(n, -1), lastVisit(n, -1)
	{ }

	struct TourVisit
	{
		int vertex;
		int depth;
	};

	std::vector<TourVisit> eulerTour;
	std::vector<int> parent;
	std::vector<int> firstVisit;
	std::vector<int> lastVisit;
};

EulerTourData eulerTour(const std::vector<std::vector<int>>& adj, const int root = 0)
{
	auto res = EulerTourData(adj.size());

	auto tour = y_combinator([&](auto tour, const int cur, const int depth) -> void
		{
			res.firstVisit[cur] = res.eulerTour.size();

			auto curInfo = EulerTourData::TourVisit{ .vertex = cur, .depth = depth };
			res.eulerTour.push_back(curInfo);

			for (auto i : adj[cur]) {
				if (i == res.parent[cur])
					continue;
				res.parent[i] = cur;
				tour(i, depth + 1);
				res.eulerTour.push_back(curInfo);
			}

			res.lastVisit[cur] = res.eulerTour.size() - 1;
		});
	tour(root, 0);

	return res;
}

// Supports O(1) LCA queries on a tree
// (With O(nlogn) precomputation time)
// For example, see Codeforces 191C--Fools and Roads
class LCA
{
public:
	LCA(const std::vector<std::vector<int>>& adj, EulerTourData tourData)
		: rmq(tourVisitMin), tour(std::move(tourData))
	{
		rmq.construct(tour.eulerTour);
	}

	LCA(const std::vector<std::vector<int>>& adj, int root = 0)
		: LCA(adj, eulerTour(adj, root))
	{ }

	int lca(int u, int v) const
	{
		if ((!inGraph(u)) || (!inGraph(v)))
			return -1;
		const auto [left, right] = std::minmax(tour.firstVisit[u], tour.firstVisit[v]);
		return rmq.getMin(left, right).vertex;
	}

	int depth(int u) const
	{
		if (!inGraph(u))
			return -1;
		return tour.eulerTour[tour.firstVisit[u]].depth;
	}

private:
	using TourVisit = EulerTourData::TourVisit;
	static constexpr auto tourVisitMin =
		[](const TourVisit& t1, const TourVisit& t2) {if (t1.depth < t2.depth) return t1; return t2; };

	bool inGraph(int u) const { return (u >= 0) && (u < tour.firstVisit.size()); }

	EulerTourData tour;
	RMQ<TourVisit, decltype(tourVisitMin)> rmq;
};

int solve(const std::vector<std::vector<int>>& adj, const std::vector<int>& color)
{
	auto tour = eulerTour(adj);

	std::unordered_map<int, std::vector<int>> colorVisits;
	for (int i = 0; i < tour.eulerTour.size(); ++i)
		colorVisits[color[tour.eulerTour[i].vertex]].push_back(i);

	auto subtreeContains = [&](auto i, auto c) -> bool {
		auto beg = std::lower_bound(colorVisits[c].cbegin(), colorVisits[c].cend(), tour.firstVisit[i]);
		auto end = std::upper_bound(colorVisits[c].cbegin(), colorVisits[c].cend(), tour.lastVisit[i]);
		return (beg != end);
	};

	auto visitedAbove = [&](auto i) -> bool {
		return (colorVisits[color[i]].front() < tour.firstVisit[i])
			|| (tour.lastVisit[i] < colorVisits[color[i]].back());
	};

	auto delta = std::vector<int>(tour.eulerTour.size() + 1, 0);
	for (int i = 0; i < adj.size(); ++i) {
		if (visitedAbove(i)) {
			++delta[tour.firstVisit[i]];
			--delta[tour.lastVisit[i] + 1];
		}

		for (auto j : adj[i]) {
			if (j == tour.parent[i])
				continue;
			if (!subtreeContains(j, color[i]))
				continue;
			++delta[0];
			--delta[tour.firstVisit[j]];
			++delta[tour.lastVisit[j] + 1];
			--delta.back();
		}
	}

	auto isGood = std::vector<bool>(adj.size(), false);
	int curSum = 0;
	for (int i = 0; i < tour.eulerTour.size(); ++i) {
		curSum += delta[i];
		if (curSum == 0)
			isGood[tour.eulerTour[i].vertex] = true;
	}
	return std::count(isGood.cbegin(), isGood.cend(), true);
}

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
		auto vec = readVecSz<int>(n);
		auto adj = readGraph(n, n - 1, false);

		std::cout << solve(adj, vec) << '\n';
	}
}