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

std::pair<std::vector<std::unordered_map<int, std::pair<int, int>>>, std::vector<std::unordered_map<int, std::pair<int, int>>>>
getStuff(const VV<char>& mat)
{
	auto left = std::vector<std::unordered_map<int, std::pair<int, int>>>(10);
	auto top = left;
	for (int i = 0; i < mat.size(); ++i) {
		for (int j = 0; j < mat[i].size(); ++j) {
			auto digit = mat[i][j] - '0';
			if (left[digit].count(i) == 0)
				left[digit][i] = std::make_pair(j, j);
			left[digit][i].first = std::min(left[digit][i].first, j);
			left[digit][i].second = std::max(left[digit][i].second, j);
			if (top[digit].count(j) == 0)
				top[digit][j] = std::make_pair(i, i);
			top[digit][j].first = std::min(top[digit][j].first, i);
			top[digit][j].second = std::max(top[digit][j].second, i);
		}
	}
	return { left, top };
}

std::pair<std::unordered_map<int, std::pair<int, int>>, std::unordered_map<int, std::pair<int, int>>> extremes(const std::vector<std::unordered_map<int, std::pair<int, int>>>& left,
	const std::vector<std::unordered_map<int, std::pair<int, int>>>& top, int n)
{
	std::unordered_map<int, std::pair<int, int>> leftmost, topmost;
	for (int i = 0; i <= 9; ++i) {
		for (int j = 0; j < n; ++j) {
			if (left[i].count(j) == 0)
				continue;
			if (leftmost.count(i) == 0)
				leftmost[i] = left[i].at(j);
			leftmost[i].first = std::min(leftmost[i].first, left[i].at(j).first);
			leftmost[i].second = std::max(leftmost[i].second, left[i].at(j).second);
		}
	}
	for (int i = 0; i <= 9; ++i) {
		for (int j = 0; j < n; ++j) {
			if (top[i].count(j) == 0)
				continue;
			if (topmost.count(i) == 0)
				topmost[i] = top[i].at(j);
			topmost[i].first = std::min(topmost[i].first, top[i].at(j).first);
			topmost[i].second = std::max(topmost[i].second, top[i].at(j).second);
		}
	}
	return { leftmost, topmost };
}

int64_t solveDigit(const std::vector<std::unordered_map<int, std::pair<int, int>>>& left,
	const std::vector<std::unordered_map<int, std::pair<int, int>>>& top,
	const std::unordered_map<int, std::pair<int, int>>& leftmost,
	const std::unordered_map<int, std::pair<int, int>>& topmost,
	int n, int d)
{
	int64_t res = 0;
	for (int i = 0; i < n; ++i) {
		if (left[d].count(i) == 0)
			continue;
		int64_t maxBase = left[d].at(i).second - left[d].at(i).first;
		res = std::max(res, maxBase * std::max(i, n - i - 1));
		int64_t maxHeight = std::max(i - topmost.at(d).first, topmost.at(d).second - i);
		maxBase = std::max(left[d].at(i).second, n - left[d].at(i).first - 1);
		res = std::max(res, maxBase * maxHeight);
	}
	for (int i = 0; i < n; ++i) {
		if (top[d].count(i) == 0)
			continue;
		int64_t maxBase = top[d].at(i).second - top[d].at(i).first;
		res = std::max(res, maxBase * std::max(i, n - i - 1));
		int64_t maxHeight = std::max(i - leftmost.at(d).first, leftmost.at(d).second - i);
		maxBase = std::max(top[d].at(i).second, n - top[d].at(i).first - 1);
		res = std::max(res, maxBase * maxHeight);
	}
	return res;
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int n;
		std::cin >> n;
		auto mat = readMatrixSz<char>(n, n);
		auto [left, top] = getStuff(mat);
		auto [leftmost, topmost] = extremes(left, top, n);
		for (int j = 0; j <= 9; ++j)
			std::cout << solveDigit(left, top, leftmost, topmost, n, j) << ' ';
		std::cout << '\n';
	}
}