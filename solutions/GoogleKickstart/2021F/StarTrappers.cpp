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

template <typename T, typename Func>
std::vector<T> readVecSz(const int sz, const Func& f)
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

template<typename T, typename Func>
std::vector<T> readVec(const Func& f)
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
using PII = P<int, int>;
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
using ui32 = uint32_t;

// Function for the Google-style output format
template <typename Printable>
void goog(int tc, Printable p)
{
	std::cout << "Case #" << tc + 1 << ": " << p << '\n';
}

i64 solve(V<P<i64, i64>>& eventPoints, i64 c)
{
	return 0;
}

template<typename T>
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

// ------------------------------------------------------------
// Turn direction stuff
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// Line segment computation stuff
// ------------------------------------------------------------

// Checks whether p is on the line segment from a to b (inclusive both ends)
// For an example use, see 
//  - Google Kickstart 2021F: Star Trappers https://codingcompetitions.withgoogle.com/kickstart/round/0000000000435bae/0000000000888d45#problem
template<typename T>
bool pointOnSegment(const Point<T>& p, const Point<T>& a, const Point<T> b)
{
	if (turnDirection(p, a, b) != TurnDir::colinear)
		return false;

	auto [xLeft, xRight] = std::minmax(a.x, b.x);
	auto [yDown, yUp] = std::minmax(a.y, b.y);
	return (xLeft <= p.x) && (p.x <= xRight)
		&& (yDown <= p.y) && (p.y <= yUp);
}

// ------------------------------------------------------------
// Polygon algorithms stuff
// ------------------------------------------------------------

// Checks whether p lies on one of the line segments of the polygon
// The polygon can be arbitrary (convex or concave), with vertices in a counter-clockwise or clockwise order
// Runs in O(n) time 
bool pointOnBoundary(const Point<double>& p, const std::vector<Point<double>>& points)
{
	for (int i = 1; i < points.size(); ++i) {
		if (pointOnSegment(p, points[i - 1], points[i]))
			return true;
	}

	return pointOnSegment(p, points.back(), points.front());
}

// Returns true if p is strictly inside the polygon (i.e., not on its boundary)
// The polygon can be convex or concave, without duplicate points and with vertices in a counter-clockwise or clockwise order
// Runs in O(n) time
// Based on: https://stackoverflow.com/a/15599478 and https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
// For an example use, see 
//  - Google Kickstart 2021F: Star Trappers https://codingcompetitions.withgoogle.com/kickstart/round/0000000000435bae/0000000000888d45#problem
bool pointInPolygon(const Point<double>& p, const std::vector<Point<double>>& points) {
	int i, j, nvert = points.size();
	bool c = false;

	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((points[i].y > p.y) != (points[j].y > p.y)) &&
			(p.x < (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
			)
			c = !c;
	}

	return c && !pointOnBoundary(p, points);
}

// Computes the perimeter of the given polygon
// The polygon can be arbitrary, with vertices in a counter-clockwise or clockwise order
// Runs in O(n) time 
double perimeter(const std::vector<Point<double>>& points)
{
	double res = std::sqrt(magSquared(points[0] - points.back()));

	for (int i = 1; i < points.size(); ++i)
		res += std::sqrt(magSquared(points[i] - points[i - 1]));

	return res;
}

// ------------------------------------------------------------
// Polar angle stuff
// ------------------------------------------------------------

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
		return a.y > 0;
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

// ------------------------------------------------------------
// Polar sort stuff
// ------------------------------------------------------------

// If polar angle is the same, breaks ties by putting farthest points first (except for the origin, which is smaller than any other point)
// For an example use, see 
//  - Google Kickstart 2021F: Star Trappers https://codingcompetitions.withgoogle.com/kickstart/round/0000000000435bae/0000000000888d45#problem
template <typename T>
void polarSort(std::vector<Point<T>>& vec, Point<T> orig = Point<T>{ 0, 0 })
{
	std::sort(vec.begin(), vec.end(),
		[&orig](const auto& p1, const auto& p2) {return polarAngleLess(orig, p1, p2); });
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout << std::setprecision(12);

	int t = 1;
	std::cin >> t;
	for (int _testCase = 0; _testCase < t; ++_testCase) {
		auto vec = readVec<Point<double>>();
		Point<double> blue;
		std::cin >> blue;

		polarSort(vec, blue);

		double res = POS_INF<double>;
		for (int i = 0; i < vec.size(); ++i) {
			for (int j = i + 1; j < vec.size(); ++j) {
				for (int k = j + 1; k < vec.size(); ++k) {
					auto polygon = std::vector<Point<double>>{ vec[i], vec[j], vec[k] };

					if (pointInPolygon(blue, polygon)) {
						res = std::min(res, perimeter(polygon));
						continue;
					}

					for (int m = k + 1; m < vec.size(); ++m) {
						polygon.push_back(vec[m]);
						if (pointInPolygon(blue, polygon))
							res = std::min(res, perimeter(polygon));
						polygon.pop_back();
					}
				}
			}
		}

		if (res == POS_INF<double>)
			goog(_testCase, "IMPOSSIBLE");
		else
			goog(_testCase, res);

		//goog(_testCase, best);
	}
}