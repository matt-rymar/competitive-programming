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
	Point(int x, int y)
		: x(x), y(y)
	{ }

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

char other(char c)
{
	return (c == '0') ? '1' : '0';
}

std::vector<std::vector<Point<int>>> solve(std::vector<std::string>& mat)
{
	std::vector<std::vector<Point<int>>> res;
	const auto target = '0';
	for (int i = 0; i < (mat.size() - 1); ++i) {
		for (int j = 0; j < (mat[i].size() - 1); ++j) {
			if (mat[i][j] == target)
				continue;
			Point p1{ i + 1, j + 1 };
			mat[i][j] = target;
			Point p2{ i + 2, j + 1 };
			mat[i + 1][j] = other(mat[i + 1][j]);
			auto vec = std::vector<Point<int>>{ p1, p2 };
			if (mat[i][j + 1] == target) {
				vec.push_back(Point{ i + 2, j + 2 });
				mat[i + 1][j + 1] = other(mat[i + 1][j + 1]);
			}
			else {
				vec.push_back(Point{ i + 1, j + 2 });
				mat[i][j + 1] = other(mat[i][j + 1]);
			}
			res.push_back(vec);
		}
		if (mat[i].back() == target)
			continue;
		Point p1{ i + 1, (int)mat[i].size() };
		mat[i].back() = other(mat[i].back());
		Point p2{ i + 2, (int)mat[i].size() };
		mat[i + 1].back() = other(mat[i+1].back());
		Point p3{ i + 2, (int)mat[i].size() - 1};
		mat[i + 1][mat[i].size() - 2] = other(mat[i + 1][mat[i].size() - 2]);
		res.push_back(std::vector<Point<int>>{p1, p2, p3});
	}
	bool needUndo = false;
	for (int j = 0; j < (mat.back().size() - 2); ++j) {
		if (needUndo) {
			Point p1{ (int)mat.size() - 1, j + 1 };
			Point p2{ (int)mat.size(), j + 2 };
			mat.back()[j + 1] = other(mat.back()[j + 1]);
			if (mat.back()[j] == target) {
				Point p3{ (int)mat.size() - 1, j + 2 };
				res.push_back(std::vector<Point<int>>{p1, p2, p3});
			}
			else {
				Point p3{ (int)mat.size(), j + 1 };
				res.push_back(std::vector<Point<int>>{p1, p2, p3});
				needUndo = false;
			}
		}
		else {
			if (mat.back()[j] != target) {
				Point p1{ (int)mat.size() - 1, j + 2 };
				Point p2{ (int)mat.size(), j + 1 };
				Point p3{ (int)mat.size(), j + 2 };
				mat.back()[j + 1] = other(mat.back()[j + 1]);
				res.push_back(std::vector<Point<int>>{p1, p2, p3});
				needUndo = true;
			}
		}
	}
	std::vector<std::vector<char>> lol{ {target, target}, { mat.back()[mat.back().size() - 2], mat.back().back() }};
	if (needUndo)
		lol[0][0] = target == '0' ? '1' : '0';
	int noTargets = (lol[0][0] == target) + 1 + (lol[1][0] == target) + (lol[1][1] == target);
	if (noTargets == 4)
		return res;
	if (noTargets == 3) {
		std::vector<Point<int>> ps;
		int tc = 0;
		for (int i = 0; i < lol.size(); ++i) {
			for (int j = 0; j < lol[i].size(); ++j) {
				if (lol[i][j] != target) {
					ps.emplace_back(i + mat.size() - 1, j + mat.front().size() - 1);
					lol[i][j] = target;
				}
				else if (tc++ < 2) {
					ps.emplace_back(i + mat.size() - 1, j + mat.front().size() - 1);
					lol[i][j] = target == '0' ? '1' : '0';
				}
			}
		}
		res.push_back(ps);
		noTargets = 2;
	}
	if (noTargets == 2) {
		std::vector<Point<int>> ps;
		int nonTC = 0;
		for (int i = 0; i < lol.size(); ++i) {
			for (int j = 0; j < lol[i].size(); ++j) {
				if (lol[i][j] == target) {
					lol[i][j] = target == '0' ? '1' : '0';
					ps.emplace_back(i + mat.size() - 1, j + mat.front().size() - 1);
				}
				else if (nonTC++ < 1) {
					lol[i][j] = target;
					ps.emplace_back(i + mat.size() - 1, j + mat.front().size() - 1);
				}
			}
		}
		res.push_back(ps);
		noTargets = 1;
	}
	if (noTargets == 1) {
		std::vector<Point<int>> ps;
		for (int i = 0; i < lol.size(); ++i) {
			for (int j = 0; j < lol[i].size(); ++j) {
				if (lol[i][j] != target)
					ps.emplace_back(i + mat.size() - 1, j + mat.front().size() - 1);
			}
		}
		res.push_back(ps);
		return res;
	}
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int n, m;
		std::cin >> n >> m;
		std::vector<std::string> mat;
		mat.reserve(n);
		for (int j = 0; j < n; ++j) {
			std::string s;
			std::cin >> s;
			mat.push_back(s);
		}
		auto vec = solve(mat);
		std::cout << vec.size() << '\n';
		for (auto& v : vec)
			std::cout << v[0] << ' ' << v[1] << ' ' << v[2] << '\n';
	}
}