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

//std::random_device dev;
//std::mt19937 rng(dev());
//std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 6);

// NOTE: DO NOT TRY TO STORE AN EMPTY STRING OR YOU'LL GET ERRORS
class AhoCorasick
{
public:
	AhoCorasick(const std::vector<std::string>& vec)
		: sz(vec.size())
	{
		for (int i = 0; i < vec.size(); ++i)
			push(vec[i], i);
		ahoInitialize();
	}

	std::vector<std::vector<int>> feed(std::string_view sv)
	{
		auto res = std::vector<std::vector<int>>(sz);
		auto curNode = root.get();
		for (int i = 0; i < sv.size(); ++i) {
			auto idx = sv[i] - 'a';
			curNode = curNode->transition[idx];
			if (curNode->tick)
				res[curNode->tickIdx].push_back(i);
			for (auto node = curNode->dictLink; node; node = node->dictLink)
				res[node->tickIdx].push_back(i);
		}
		return res;
	}

private:
	struct Node {
		static constexpr int alphabetSize = 26;

		int tickIdx = -1;

		// If tick is true, the node corresponds to a word in the dictionary
		bool tick = false;
		std::vector<std::unique_ptr<Node>> children = std::vector<std::unique_ptr<Node>>(alphabetSize);
		// Where we do transition upon reading a character: if children[i] exists then transition[i] is the same;
		//	otherwise the value is (pre-) calculated using fail links
		std::vector<Node*> transition = std::vector<Node*>(alphabetSize, nullptr);
		// Points to the first suffix of our node that exists in the tree
		Node* suffLink = nullptr;
		// First node with a tick that is reachable over suffix links
		Node* dictLink = nullptr;
	};

	void push(std::string_view sv, int idx)
	{
		auto curNode = root.get();
		for (auto c : sv) {
			auto idx = c - 'a';
			if (!(curNode->children[idx]))
				curNode->children[idx] = std::make_unique<Node>();
			curNode = curNode->children[idx].get();
		}
		curNode->tick = true;
		curNode->tickIdx = idx;
	}

	Node* findNode(std::string_view sv)
	{
		auto curNode = root.get();
		for (auto c : sv) {
			auto idx = c - 'a';
			if (!(curNode->children[idx]))
				return nullptr;
			curNode = curNode->children[idx].get();
		}
		return curNode;
	}

	// Initializes the transition array and the suffLink for the root and the suffLinks for its children
	// Returns a queue containing root's children
	std::queue<Node*> initRoot()
	{
		std::queue<Node*> res;
		for (int i = 0; i < root->children.size(); ++i) {
			if (!(root->children[i])) {
				root->transition[i] = root.get();
				continue;
			}
			auto child = root->children[i].get();
			res.push(child);
			root->transition[i] = child;
			child->suffLink = root.get();
		}
		return res;
	}

	// Computes all the suffix links, transitions and the dictionary links for all nodes in the trie
	void ahoInitialize()
	{
		auto q = initRoot();
		while (!q.empty()) {
			// cur already has suffLink correctly initilized
			// Furthermore, we know that cur != root.get() so cur->suffLink is non-null
			auto cur = q.front();
			q.pop();
			if (cur->suffLink->tick)
				cur->dictLink = cur->suffLink;
			else
				cur->dictLink = cur->suffLink->dictLink;
			for (int i = 0; i < cur->children.size(); ++i) {
				if (!(cur->children[i])) {
					cur->transition[i] = cur->suffLink->transition[i];
					continue;
				}
				auto child = cur->children[i].get();
				q.push(child);
				cur->transition[i] = child;
				child->suffLink = cur->suffLink->transition[i];
			}
		}
	}

	std::unique_ptr<Node> root = std::make_unique<Node>();
	const int sz;
};

int getShortest(const std::vector<int>& ends, int strLen, int k)
{
	if (ends.size() < k)
		return -1;
	auto curLen = ends[k - 1] - ends[0];
	for (int i = k; i < ends.size(); ++i)
		curLen = std::min(curLen, ends[i] - ends[i - (k - 1)]);
	return curLen + strLen;
}

std::vector<int> solve(std::string_view sv, std::vector<int> ks, std::vector<std::string> ms)
{
	auto aho = AhoCorasick(ms);
	auto allEnds = aho.feed(sv);
	std::vector<int> res;
	res.reserve(ms.size());
	for (int i = 0; i < ks.size(); ++i)
		res.push_back(getShortest(allEnds[i], ms[i].size(), ks[i]));
	return res;
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	//std::cin >> t;
	for (int i = 0; i < t; ++i) {
		std::string s;
		std::cin >> s;
		int n;
		std::cin >> n;
		std::vector<int> ks;
		ks.reserve(n);
		std::vector<std::string> ms;
		ms.reserve(n);
		for (int j = 0; j < n; ++j) {
			int k;
			std::string m;
			std::cin >> k >> m;
			ks.push_back(k);
			ms.push_back(m);
		}
		for (auto i : solve(s, ks, ms))
			std::cout << i << '\n';
	}
}