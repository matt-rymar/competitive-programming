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

// Note, by default doesn't add a sentinel at the end so the suffix tree may be an implicit suffix tree
class SuffixTree
{
public:
	SuffixTree(std::string input, bool addSentinel, char sentinel = defaultSentinel)
		: s(std::move(input)), leafNodeSentinel(input.size() - 1 + addSentinel), endSentinel(sentinel)
	{
		if (addSentinel)
			s.push_back(endSentinel);
		auto state = ConstructionState(root.get());
		for (int i = 0; i < s.size(); ++i)
			extend(state, i);
	}

	static constexpr char defaultSentinel = '$'; // Must not be present in the input string

	int longestPrefMidSuf()
	{
		deleteAnnoying();
		int curBest = -1;
		Node* curNode = root.get();
		int curIdx = 0;
		for (int i = 0; i < s.size(); ++i) {
			const auto curChar = s[i];
			if (curIdx == curNode->rangeLen()) {
				if (curNode->children.count(curChar) == 0)
					break;
				curNode = curNode->children[curChar].get();
				if (curNode->hi == leafNodeSentinel)
					break;
				curIdx = 0;
			}
			if ((curIdx < curNode->rangeLen()) && (s[curNode->lo + curIdx] != curChar))
				break;
			++curIdx;
			if ((curIdx == curNode->rangeLen()) 
				&& (curNode->children.size() >= 2) 
				&& (curNode->children.count(endSentinel) > 0)) {
				curBest = i;
			}
		}
		return curBest;
	}

	void deleteAnnoying()
	{
		// Find annoying leaf
		Node* curNode = root.get();
		int curIdx = 0;
		while (curIdx < s.size()) {
			curNode = curNode->children[s[curIdx]].get();
			curIdx += curNode->rangeLen();
		}
		// Remove the annoying leaf
		while (curNode->children.size() == 0) {
			auto parent = curNode->parent;
			parent->children.erase(s[curNode->lo]);
			curNode = parent;
		}
	}

private:
	const int leafNodeSentinel;
	const int endSentinel;

	struct Node
	{
		Node(int left, int right, Node* parent)
			: lo(left), hi(right), parent(parent)
		{ }
		Node(int left, int right)
			: Node(left, right, nullptr)
		{ }
		// Constructs a node holding a range of length zero
		Node()
			: Node(-1, -2) 
		{ }

		int rangeLen() const { return hi - lo + 1; }

		// The edge leading to this node had the substring s[l..r] (inclusive) as its label
		int lo, hi;
		std::map<char, std::unique_ptr<Node>> children;
		Node* parent = nullptr;
		Node* suffLink = nullptr;
	};

	struct ConstructionState
	{
		ConstructionState(Node* start)
			: curNode(start)
		{ }

		int leaves = 0; // No. leaves in the tree (where root is never considered a leaf)
		Node* prevNode = nullptr;
		Node* curNode = nullptr; // Current active node in the tree
		int curNodeIdx = 0; // Current index in the current active node
	};

	void splitNode(ConstructionState& state, int idx, char charPushed)
	{
		const auto parent = state.curNode->parent;

		// Create a new node that will split our current node's edge
		auto splitter = std::make_unique<Node>(state.curNode->lo, state.curNode->lo + state.curNodeIdx - 1, parent);

		// Add a new leaf node
		splitter->children[charPushed] =
			std::make_unique<Node>(idx, leafNodeSentinel, splitter.get());

		// Make curNode the child of the new node and update curNode's data
		const auto parChar = s[state.curNode->lo];
		const auto splitChar = s[state.curNode->lo + state.curNodeIdx];
		splitter->children[splitChar] = std::move(parent->children[parChar]); // parent->children[parChar] == state.curNode
		state.curNode->parent = splitter.get();
		state.curNode->lo = state.curNode->lo + state.curNodeIdx;

		// Update curNode and set the previous node's suffix link, if necessary
		state.curNode = splitter.get();
		if (state.prevNode)
			state.prevNode->suffLink = splitter.get();
		// The newly introduces node is internal so it may need a suffix link
		state.prevNode = splitter.get();

		// Finally, make the new node the child of our previous parent
		parent->children[parChar] = std::move(splitter);
	}
	
	void traverse(ConstructionState& state)
	{
		if ((state.curNode->parent == root.get()) && (state.curNode->rangeLen() == 1)) {
			state.curNode = root.get();
			state.curNodeIdx = 0;
			state.prevNode->suffLink = root.get();
			state.prevNode = nullptr;
			return;
		}

		// We need to traverse up and back down
		const auto prevNode = state.curNode;
		const auto baseIdx = state.curNode->lo + (state.curNode->parent == root.get());
		auto lengthToTraverse = state.curNode->rangeLen() - (state.curNode->parent == root.get());
		state.curNode = (state.curNode->parent == root.get()) ? root->children[s[baseIdx]].get()
			: state.curNode->parent->suffLink->children[s[baseIdx]].get();
		for (int idx = state.curNode->rangeLen(); state.curNode->rangeLen() < lengthToTraverse; idx += state.curNode->rangeLen()) {
			lengthToTraverse -= state.curNode->rangeLen();
			state.curNode = state.curNode->children[s[baseIdx + idx]].get();
		}
		state.curNodeIdx = lengthToTraverse;
		if ((state.curNode->rangeLen() == lengthToTraverse) && (state.prevNode)) {
			// The node we need to attach a prefix link to already exists
			state.prevNode->suffLink = state.curNode;
			state.prevNode = nullptr;
		}
	}

	// Handles edge cases where we might need to go to an adjacent child node before continuing with checking
	void adjustNode(ConstructionState& state, const char charPushed)
	{
		if ((state.curNode != root.get()) 
			&& (state.curNodeIdx == 0) && (s[state.curNode->lo] != charPushed)) {
			state.curNode = state.curNode->parent;
			state.curNodeIdx = state.curNode->rangeLen();
		}
		if (state.curNodeIdx == state.curNode->rangeLen()
			&& (state.curNode->children.count(charPushed) > 0)) {
			state.curNode = state.curNode->children[charPushed].get();
			state.curNodeIdx = 0;
		}
	}

	// Detects end of type 2 extensions,
	//	handles the trivial cases of type 2 extension or prepares state to be ready for a non-trivial one
	// Returns true if no further type 2 extensions will be needed
	bool handleTrivial(ConstructionState& state, int idx, const char charPushed)
	{
		adjustNode(state, charPushed);
		if (state.curNode == root.get()) {
			// The proper child doesn't exist, or adjustNode would've gone there
			root->children[charPushed] = std::make_unique<Node>(idx, leafNodeSentinel, root.get());
			state.curNodeIdx = 0;
			++state.leaves;
			// If we were at the root, it means our inserted suffix was of length one, so we'll certainly be breaking now
			return true;
		}
		if ((state.curNodeIdx != state.curNode->rangeLen()) && (s[state.curNode->lo + state.curNodeIdx] == charPushed)) {
			// All sufixes from here on are contained in the tree already
			++state.curNodeIdx;
			return true;
		}

		return false;
	}

	// Main workhorse of the algorithm
	// Assuming the current tree represents the string s[0..i-1], extends the tree to represent the string s[0..i]
	void extend(ConstructionState& state, const int idx)
	{
		const auto charPushed = s[idx];
		for (int i = state.leaves; i <= idx; ++i) {
			if (handleTrivial(state, i, charPushed))
				break;
			// New node needs to be inserted
			if (state.curNodeIdx == state.curNode->rangeLen()) {
				// Simple insertion: we are at the end of an edge so no need for splitting edges
				state.curNode->children[charPushed] = std::make_unique<Node>(idx, leafNodeSentinel, state.curNode);
				state.curNode = state.curNode->suffLink; // Must be valid since curNode is not a newly inserted internal node
				state.curNodeIdx = state.curNode->rangeLen();
			}
			else {
				// We need to split an edge in the middle and create a new internal node
				splitNode(state, idx, charPushed);
				traverse(state);
			}
			++state.leaves;
		}
	}

	std::unique_ptr<Node> root = std::make_unique<Node>();
	std::string s;
};

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	//std::cin >> t;
	for (int i = 0; i < t; ++i) {
		std::string s;
		std::cin >> s;
		auto tree = SuffixTree(s, true);
		auto idx = tree.longestPrefMidSuf();
		if (idx < 0)
			std::cout << "Just a legend\n";
		else
			std::cout << s.substr(0, idx + 1) << '\n';
	}
}