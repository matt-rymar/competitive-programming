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

	// WARNING: Runs pretty slow, may TLE despite good asymptotic complexity
	// Searches for the first index hi such that !pred(query(lo, hi), target)
	// If there is no such index, return the length of the stored array (that is, one past the last valid index)
	// Runs in O(lgn) time
	// Note: for this function to work Data must have a default constructor that reprents the
	//	accumulation of zero elements (basically be a neutral element of Data::operator+)
	// Note: the function pred should be a binary predicate on Data (intuitively, a "<" operator) that must define 
	//	a strict weak ordering that is monotonically increasing in interval length
	//	(so we must have pred(query(lo, hi), query(lo, hi + k)) for all lo <= hi and all k > 0)
	// TODO: I think the requirement on the default constructor could be removed, but would make the code more complex
	//	(we'd basically need to first descend to the leaf corresponding to lo, 
	//		and then call the current version of lowerBound with lo+1 and the data at lo as currAcc,
	//		and also be careful about the case where the empty interval is enough)
	template<typename BinaryPredicate>
	int lowerBound(const int lo, const Data& target, const BinaryPredicate& pred)
	{
		auto [idx, acc] = root->lowerBound(lo, target, pred, Data());
		return idx;
	}

	// WARNING: Runs pretty slow, may TLE despite good asymptotic complexity
	// Same as lowerBound except returns first index hi such that pred(target, query(lo, hi))
	template<typename BinaryPredicate>
	int upperBound(const int lo, const Data& target, const BinaryPredicate& pred)
	{
		auto [idx, acc] = root->upperBound(lo, target, pred, Data());
		return idx;
	}

	// Returns a vector containg all of the data stored in the tree (i.e. all the Data's in the leaf nodes, in order)
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
				// Could also move this line to the member initialization above
				// That would let allow Data not to have a default constructor, but force us to call init more times than is necessary
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

		// Return value = [hi, acc], where
		//	hi = first index such that !pred(query(lo, hi), target), or rightmost + 1 if no such index exists
		//	acc = query(lo, hi) (if hi == rightmost + 1, then returns query(lo, rightmost))
		// TODO: Update the comments above
		// TODO: Make sure this works correctly for non-commutative Data::operator+
		// TODO: Implement this with a Node* pointer so as to avoid constantly passing the references to target and pred
		template<typename BinaryPredicate>
		std::pair<int, Data> lowerBound(const int lo, const Data& target, const BinaryPredicate& pred, const Data& curAcc)
		{
			// We only consider sequences which begin somewhere in our interval
			if ((lo < leftmost) || (rightmost < lo))
				return { rightmost + 1, curAcc };

			// We're already good enough (this case may pop up if the empty interval is good)
			if (!pred(curAcc, target))
				return { lo - 1, curAcc };

			const auto totalAcc = curAcc + data;

			// We are a leaf, return our index if we're good, our index + 1 if we aren't
			if (leftmost == rightmost) 
				return { rightmost + pred(totalAcc, target), totalAcc};

			// Even our whole interval wouldn't be enough, so we can break early 
			//	(here we need the monotonicity of pred, which is crucial for the logarithmic running time)
			if ((lo == leftmost) && pred(totalAcc, target)) 
				return { rightmost + 1, totalAcc };

			// Standard case: first check if the index is in our left half, if not, check the right one
			push();
			auto [leftIdx, leftAcc] = left->lowerBound(lo, target, pred, curAcc);
			if (leftIdx <= left->rightmost)
				return { leftIdx, leftAcc };
			
			return right->lowerBound(std::max(lo, right->leftmost), target, pred, leftAcc);
		}

		template<typename BinaryPredicate>
		std::pair<int, Data> upperBound(const int lo, const Data& target, const BinaryPredicate& pred, const Data& curAcc)
		{
			// We only consider sequences which begin somewhere in our interval
			if ((lo < leftmost) || (rightmost < lo))
				return { rightmost + 1, curAcc };

			// We're already good enough (this case may pop up if the empty interval is good)
			if (pred(target, curAcc))
				return { lo - 1, curAcc };

			const auto totalAcc = curAcc + data;

			// We are a leaf, return our index if we're good, our index+1 if we aren't
			if (leftmost == rightmost)
				return { rightmost + !pred(target, totalAcc), totalAcc };

			// Even our whole interval wouldn't be enough, so we can break early 
			//	(here we need the monotonicity of pred, which is crucial for the logarithmic running time)
			if ((lo == leftmost) && !pred(target, totalAcc))
				return { rightmost + 1, totalAcc };

			// Standard case: first check if the index is in our left half, if not, check the right one
			push();
			auto [leftIdx, leftAcc] = left->upperBound(lo, target, pred, curAcc);
			if (leftIdx <= left->rightmost)
				return { leftIdx, leftAcc };

			return right->upperBound(std::max(lo, right->leftmost), target, pred, leftAcc);
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
	i64 val;
};

// Stores data about a segment
struct Data
{
	i64 min = POS_INF<i64>;
	i64 max = NEG_INF<i64>;
	i64 sum = 0;
	i64 rangeLen = 0;

	// Note: this operator does *not* have to be commutative, but it *has to* be associative
	friend Data operator+(const Data& left, const Data& right)
	{
		return Data{
			.min = std::min(left.min, right.min),
			.max = std::max(left.max, right.max),
			.sum = left.sum + right.sum,
			.rangeLen = left.rangeLen + right.rangeLen,
		};
	}

	// Updates data about segment
	friend void operator+=(Data& data, const UpdateData& upd)
	{
		// Assumes that data.max <= upd.val
		data.min = data.max = upd.val;
		data.sum = upd.val * data.rangeLen;
	}
};

// Constructs a segment tree using the Data and UpdateData structures above
// (Useful for making type deduction work)
template<typename InitFn>
SegmentTree<Data, UpdateData, InitFn> DefSegmentTree(const InitFn& init, int sz)
{
	return SegmentTree<Data, UpdateData, InitFn>(init, sz);
}

// Version with the new Segment Tree library, but before query_if and update_if
// Because there are no functions like those above, the implementation below is pretty slow and barely passes
// This may be either because lowerBound and upperBound are particularly slow for some reason
// Or because we need to call them a few times per query, unlike query_if or update_if which needs just one call
// It's probably the latter, but it may be both

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cout << std::setprecision(12);

	int t = 1;
	//std::cin >> t;
	for (int _testCase = 0; _testCase < t; ++_testCase) {
		int n, q;
		std::cin >> n >> q;
		auto vec = readVecSz<i64>(n);

		auto tree = DefSegmentTree([&](auto i) {return Data{ vec[i], vec[i], vec[i], 1 }; }, vec.size());
		auto minComp = [](const Data& lhs, const Data& rhs) {return lhs.min > rhs.min; };
		auto sumComp = [](const Data& lhs, const Data& rhs) {return lhs.sum < rhs.sum; };
		auto dummy = Data();
		auto rangeSetMax = [&](const int hi, const i64 val) {
			dummy.min = val;
			const auto lo = tree.lowerBound(0, dummy, minComp);
			tree.update(lo, hi, UpdateData{ val });
		};
		auto findBegin = [&](const int lo, const i64 val) {
			dummy.min = val;
			return tree.lowerBound(lo, dummy, minComp);
		};
		auto endSum = [&](const int lo, const i64 val) {
			auto beg = findBegin(lo, val);
			if (beg >= vec.size())
				return std::make_pair((int)vec.size(), Data());
			dummy.sum = val;
			auto end = tree.upperBound(beg, dummy, sumComp);
			return std::make_pair(end, tree.query(beg, end - 1));
		};
		auto shopQuery = [&](int lo, i64 val) {
			i64 res = 0;
			while ((lo < vec.size()) && (val > 0)) {
				auto [newLo, d] = endSum(lo, val);
				lo = newLo;
				val -= d.sum;
				res += d.rangeLen;
			}
			return res;
		};

		for (int i = 0; i < q; ++i) {
			int type, x;
			i64 y;
			std::cin >> type >> x >> y;
			--x;
			if (type == 1)
				rangeSetMax(x, y);
			else
				std::cout << shopQuery(x, y) << '\n';
		}
	}
}