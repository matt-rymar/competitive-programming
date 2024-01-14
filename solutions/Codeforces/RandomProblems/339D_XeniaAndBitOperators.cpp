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

// Segment Tree with lazy propagation that allows range queries and range updates in O(lgn) time
// For an example use, see Codeforces 1478E--Nezzar And Binary String
template<typename Data,
	typename UpdateData,
	typename InitFn>
	class SegmentTree
{
public:
	// Constructs the segment tree in O(nlgn) time
	SegmentTree(const InitFn& init, const int sz)
		: root(std::make_unique<Node>(init, 0, sz - 1))
	{ }

	// Accumulates the data in the range in O(lgn) time
	Data query(const int lo, const int hi)
	{
		return root->query(lo, hi);
	}

	// Lazily updates the range in O(lgn) time
	void update(const int lo, const int hi, const UpdateData& upd)
	{
		root->update(lo, hi, upd);
	}

	// Updates the entry at index idx
	// Right now it doesn't do anything smarter than update(idx, idx, upd)
	void pointUpdate(const int idx, const UpdateData& upd)
	{
		update(idx, idx, upd);
	}

	// Returns a vector containg all of the data stored in the tree
	// Note: since it accesses all the nodes in the tree, it runs in O(nlgn) time
	std::vector<Data> getContents()
	{
		std::vector<Data> res;
		root->getContents(res);
		return res;
	}

private:
	class Node {
	public:
		Node(const InitFn& init, const int lo, const int hi)
			: leftmost(lo), rightmost(hi)
		{
			if (leftmost == rightmost) {
				data = init(leftmost);
				return;
			}
			auto mid = lo + (hi - lo) / 2;
			left = std::make_unique<Node>(init, lo, mid);
			right = std::make_unique<Node>(init, mid + 1, hi);
			data = left->data + right->data;
		}

		Data query(const int lo, const int hi)
		{
			if (isDisjoint(lo, hi))
				return Data();
			if (coversUs(lo, hi))
				return data;
			push();
			return left->query(lo, hi) + right->query(lo, hi);
		}

		void update(const int lo, const int hi, const UpdateData& upd)
		{
			if (isDisjoint(lo, hi))
				return;
			if (coversUs(lo, hi)) {
				lazySet = true;
				lazyUpdate = upd;
				data += upd;
				return;
			}
			push();
			left->update(lo, hi, upd);
			right->update(lo, hi, upd);
			data = left->data + right->data;
		}

		void push()
		{
			if (!lazySet)
				return;
			lazySet = false;
			left->data += lazyUpdate;
			right->data += lazyUpdate;
			left->lazySet = right->lazySet = true;
			left->lazyUpdate = right->lazyUpdate = lazyUpdate;
		}

		void getContents(std::vector<Data>& res)
		{
			if (leftmost == rightmost) {
				res.push_back(data);
				return;
			}
			push();
			left->getContents(res);
			right->getContents(res);
		}
	private:
		const int leftmost, rightmost;
		std::unique_ptr<Node> left, right;
		Data data;
		bool lazySet = false;
		UpdateData lazyUpdate;

		bool coversUs(const int lo, const int hi) { return (lo <= leftmost) && (rightmost <= hi); }
		bool isDisjoint(const int lo, const int hi) { return (hi < leftmost) || (rightmost < lo); }
		int rangeLen() { return rightmost - leftmost + 1; }
	};

	std::unique_ptr<Node> root;
};

// Structure that stores all data necessary to update a segment (range)
struct UpdateData
{
	uint32_t val;
};

// Stores data about a segment
struct Data
{
	uint32_t val = 0;
	bool doOr = true;

	// Note: this operator may *not* be commutative, but it *has to* be associative
	friend Data operator+(const Data& left, const Data& right)
	{
		return Data{
			.val = (left.doOr) ? (left.val | right.val) : (left.val ^ right.val),
			.doOr = !left.doOr
		};
	}

	// Updates data about segment
	friend void operator+=(Data& data, const UpdateData& upd)
	{
		data.val = upd.val;
	}
};

// Constructs a segment tree using the Data and UpdateData structures above
// (Useful for making type deduction work)
template<typename InitFn>
SegmentTree<Data, UpdateData, InitFn> DefSegmentTree(const InitFn& init, int sz)
{
	return SegmentTree<Data, UpdateData, InitFn>(init, sz);
}

// Segment Tree is actually overkill here, I think--so much power is left unused

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout << std::setprecision(12);

	int t = 1;
	//std::cin >> t;
	for (int _testCase = 0; _testCase < t; ++_testCase) {
		int n, m;
		std::cin >> n >> m;
		auto vec = readVecSz<uint32_t>(1 << n);
		auto tree = DefSegmentTree([&](auto i) {return Data{ .val = vec[i], .doOr = true }; }, vec.size());
		for (int j = 0; j < m; ++j) {
			int idx;
			uint32_t val;
			std::cin >> idx >> val;
			--idx;
			tree.pointUpdate(idx, UpdateData{ val });
			std::cout << tree.query(0, POS_INF<int>).val << '\n';
		}
	}
}