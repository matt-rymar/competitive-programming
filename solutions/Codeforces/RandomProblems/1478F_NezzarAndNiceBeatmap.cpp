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
std::vector<T> readVecSz(const int sz)
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
std::vector<T> readVecSz(const int sz, const std::function<T(T)>& f)
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
std::vector<std::vector<T>> readMatSz(const int rows, const int cols)
{
	std::vector<std::vector<T>> res;
	res.reserve(rows);
	for (int i = 0; i < rows; ++i)
		res.push_back(readVecSz<T>(cols));
	return res;
}

template<typename T>
std::vector<std::vector<T>> readMat()
{
	int m, n;
	std::cin >> m >> n;
	return readMatSz<T>(m, n);
}

std::vector<std::vector<int>> readGraph(const int vertices, const int edges, const bool directed)
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
std::vector<std::vector<std::pair<int, Weight>>> readGraphWeighted(const int vertices, const int edges, const bool directed)
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

template<typename T = int64_t>
struct Point {
	T x, y;

	friend bool operator==(const Point& lhs, const Point& rhs) { return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y); }
	friend bool operator!=(const Point& lhs, const Point& rhs) { return !(lhs == rhs); }
	Point& operator-=(const Point& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	friend Point operator-(Point lhs, const Point& rhs) { lhs -= rhs; return lhs; }

	std::size_t hash() const { return pair_hash::hash(std::make_pair(x, y)); }

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

namespace std
{
	template<typename T>
	struct hash<Point<T>>
	{
		std::size_t operator()(const Point<T>& p) const { return p.hash(); }
	};
}

template <typename T>
T crossProduct(const Point<T>& a, const Point<T>& b)
{
	return a.x * b.y - a.y * b.x;
}

template <typename T>
T dotProduct(const Point<T>& a, const Point<T>& b)
{
	return a.x * b.x + a.y * b.y;
}

template <typename T>
T magSquared(const Point<T>& a)
{
	return a.x * a.x + a.y * a.y;
}

template <typename T>
T distSquared(const Point<T>& a, const Point<T>& b)
{
	return magSquared(a - b);
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout << std::setprecision(12);

	int t = 1;
	//std::cin >> t;
	for (int _testCase = 0; _testCase < t; ++_testCase) {
		auto vec = readVec<Point<i64>>();

		std::unordered_set<int> available;
		for (int i = 1; i < vec.size(); ++i)
			available.insert(i);

		std::cout << 1 << ' ';
		int prevIdx = 0;
		for (int i = 1; i < vec.size(); ++i) {
			auto best = std::min_element(available.begin(), available.end(),
				[&](const auto& left, const auto& right)
				{return distSquared(vec[prevIdx], vec[left]) > distSquared(vec[prevIdx], vec[right]); });
			std::cout << *best + 1 << ' ';
			prevIdx = *best;
			available.erase(best);
		}
		std::cout << '\n';
	}
}