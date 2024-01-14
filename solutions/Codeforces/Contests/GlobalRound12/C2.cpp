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
using VVI = VV<int>;
template<typename T>
constexpr T NEG_INF = std::numeric_limits<T>::min();
template<typename T>
constexpr T POS_INF = std::numeric_limits<T>::max();
using i64 = int64_t;

bool downBad(const VV<char>& mat, int i, int j)
{
	if (mat[i][j] == '.')
		return false;
	if ((i + 2 < mat.size()) && (mat[i][j] == mat[i + 1][j]) && (mat[i + 1][j] == mat[i + 2][j]))
		return true;
	if ((i + 1 < mat.size()) && (i > 0) && (mat[i][j] == mat[i - 1][j]) && (mat[i][j] == mat[i + 1][j]))
		return true;
	return ((i > 1) && (mat[i][j] == mat[i - 1][j]) && (mat[i][j] == mat[i - 2][j]));
}

bool rightBad(const VV<char>& mat, int i, int j)
{
	if (mat[i][j] == '.')
		return false;
	if ((j + 2 < mat[i].size()) && (mat[i][j] == mat[i][j + 1]) && (mat[i][j] == mat[i][j + 2]))
		return true;
	if ((j + 1 < mat[i].size()) && (j > 0) && (mat[i][j] == mat[i][j - 1]) && (mat[i][j] == mat[i][j + 1]))
		return true;
	return ((j > 1) && (mat[i][j] == mat[i][j - 1]) && (mat[i][j] == mat[i][j - 2]));
}

char other(char c)
{
	if (c == 'X')
		return 'O';
	return 'X';
}

std::pair<int, VV<char>> opsss(VV<char> mat, const int xmod, const int omod)
{
	int ops = 0;
    bool doneStuff = true;
	while (doneStuff) {
        doneStuff = false;
		for (int i = 0; i < mat.size(); ++i) {
			for (int j = 0; j < mat[i].size(); ++j) {
				if (rightBad(mat, i, j)) {
					++ops;
                    doneStuff = true;
					for (int k = j; k <= j + 2; ++k) {
						if (((i + k) % 3 == xmod) && (mat[i][j] == 'X')) {
							mat[i][k] = other(mat[i][k]);
							break;
						}
						if (((i + k) % 3 == omod) && (mat[i][j] == 'O')) {
							mat[i][k] = other(mat[i][k]);
							break;
						}
					}
				}
			}
		}
		for (int i = 0; i < mat.size(); ++i) {
			for (int j = 0; j < mat[i].size(); ++j) {
				if (downBad(mat, i, j)) {
					++ops;
                    doneStuff = true;
					for (int k = i; k <= i + 2; ++k) {
						if (((j + k) % 3 == xmod) && (mat[i][j] == 'X')) {
							mat[k][j] = other(mat[k][j]);
							break;
						}
						if (((j + k) % 3 == omod) && (mat[i][j] == 'O')) {
							mat[k][j] = other(mat[k][j]);
							break;
						}
					}
				}
			}
		}
	}
	
	/*if (xmod == 1) {
		std::cout << "CUR: " << ops << '\n';
		for (const auto& vec : mat) {
			for (auto c : vec)
				std::cout << c;
			std::cout << '\n';
		}
	}*/
	return { ops, mat };
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
		int k = 0;
		for (const auto& vec : mat) {
			for (const auto c : vec) {
				if (c != '.')
					++k;
			}
		}
		bool done = false;
		for (int i = 0; (i < 3) && (!done); ++i) {
			for (int j = 0; j < 3; ++j) {
				if (i == j)
					continue;
				auto [ops, res] = opsss(mat, i, j);
				if (ops <= (k / 3)) {
					//std::cout << "FOUND\n";
					mat = res;
					done = true;
					break;
				}
			}
		}
		for (const auto& vec : mat) {
			for (auto c : vec)
				std::cout << c;
			std::cout << '\n';
		}
	}
}