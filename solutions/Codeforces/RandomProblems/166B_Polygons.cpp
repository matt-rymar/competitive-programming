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
T crossProduct(Point<T> a, Point<T> b)
{
	return a.x * b.y - a.y * b.x;
}

enum class TurnDir
{
	left, colinear, right
};
template <typename T>
TurnDir turnDirection(const Point<T>& a, Point<T> b, Point<T> c)
{
	b -= a;
	c -= a;
	const auto prod = crossProduct(b, c);
	if (prod > 0)
		return TurnDir::left;
	if (prod == 0)
		return TurnDir::colinear;
	return TurnDir::right;
}

template <typename T>
T magSquared(const Point<T>& a)
{
	return a.x * a.x + a.y * a.y;
}

// If points are colinear, it puts the farther points first
// By definition, origin is less than all other points (except itself)
template <typename T>
bool polarAngleLess(const Point<T>& origin, Point<T> a, Point<T> b)
{
	if (a == origin)
		return b != origin;
	if (b == origin)
		return false;

	a -= origin;
	b -= origin;

	if (a.y* b.y < 0)
		return a.y < 0;
	if (a.y * b.y == 0) {
		if (a.y == 0) {
			if (b.y == 0) {
				if (a.x* b.x < 0)
					return a.x > 0;
				return magSquared(a) > magSquared(b);
			}
			return (a.x >= 0) || (b.y < 0);
		}
		return (b.x < 0) && (a.y > 0);
	}
	const auto prod = crossProduct(a, b);
	if (prod == 0)
		return magSquared(a) > magSquared(b);
	return prod > 0;
}

template <typename T>
void polarSort(std::vector<Point<T>>& vec, const Point<T>& orig)
{
	std::sort(vec.begin(), vec.end(),
		[&orig](const auto& p1, const auto& p2) {return polarAngleLess(orig, p1, p2); });
}

// ------------------------------------------------------------
// Convex hull stuff
// ------------------------------------------------------------

// Note that it assumes that the points vector does not contain duplicate points
template <typename T>
void polarUnique(std::vector<Point<T>>& points, bool allOnBoundary)
{
	auto beg = points.begin() + 1;
	if (allOnBoundary) {
		++beg;
		while ((beg != points.end()) && (turnDirection(points.front(), *(beg - 1), *beg) == TurnDir::colinear))
			++beg;
		// By default, polar sort puts the farthest points first (so that std::unique works well with it)
		// However, for convex hull we need the first set of points to be in order of ascending distance from the origin point
		std::reverse(points.begin() + 1, beg);
		if (beg == points.end())
			return;
	}
	auto end = points.end();
	if (allOnBoundary) {
		// We know that there are at least two different polar angles (else we'd have had beg == points.end() and would have returned)
		// => No need for bound checks
		--end;
		while (turnDirection(points.front(), *(end - 1), *end) == TurnDir::colinear)
			--end;
	}
	points.erase(std::unique(beg, end,
		[&points](const auto& p1, const auto& p2) {return turnDirection(points.front(), p1, p2) == TurnDir::colinear; }), end);
}

// WARNING: Not tested on a problem where I needed to call convexHull with allOnBoundary == false
// Implements Graham's scan, possibly extended to also return all points on the boundary of the convex hull
// Note: the convex hull returned does not contain any duplicate points
// If allOnBoundary is set to true, the convex hull includes all the points on the boundary of the convex hull
// (that is including points on the line segment between a pair of other points on the hull)
// Note that even if allOnBoundary is set, duplicates will be removed
template <typename T>
std::vector<Point<T>> convexHull(std::vector<Point<T>> points, bool allOnBoundary)
{
	if (points.size() <= 2)
		return points;
	const auto lowestPoint = *std::min_element(points.cbegin(), points.cend(),
		[](const auto& p1, const auto& p2) {return std::tie(p1.y, p1.x) < std::tie(p2.y, p2.x); });
	polarSort(points, lowestPoint);
	points.erase(std::unique(points.begin(), points.end()), points.end());
	polarUnique(points, allOnBoundary);
	if (points.size() <= 3)
		return points;
	std::vector<Point<int64_t>> stack{ points[0], points[1] };
	for (int i = 2; i < points.size(); ++i) {
		while (!((turnDirection(stack[stack.size() - 2], stack[stack.size() - 1], points[i]) == TurnDir::left)
			|| (allOnBoundary && (turnDirection(stack[stack.size() - 2], stack[stack.size() - 1], points[i]) == TurnDir::colinear))))
			stack.pop_back();
		stack.push_back(points[i]);
	}

	return stack;
}

bool solve(std::vector<Point<int64_t>> A, const std::vector<Point<int64_t>>& B)
{
	A.insert(A.cend(), B.cbegin(), B.cend());
	auto hull = convexHull(A, true);
	auto bSet = std::unordered_set<Point<int64_t>>(B.cbegin(), B.cend());
	for (const auto& p : hull) {
		if (bSet.count(p) > 0)
			return false;
	}
	return true;
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	//std::cin >> t;
	for (int i = 0; i < t; ++i) {
		auto A = readVec<Point<int64_t>>();
		auto B = readVec<Point<int64_t>>();
		if (solve(A, B))
			std::cout << "YES\n";
		else
			std::cout << "NO\n";
	}
}