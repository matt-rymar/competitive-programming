// Disjoint Set Union structure.
//
// For an example, see task 1468J--Road Reform on Codeforces.
class UnionFind {
 public:
  UnionFind(int n) : parent(n, -1), rank(n, 0), size(n, 1) {
    std::iota(parent.begin(), parent.end(), 0);
  }

  int getSet(int i) {
    if (i < parent.size())
      return findCompress(i);
    return -1;
  }

  int getSize(int i) {
    const auto set = getSet(i);
    if (set < parent.size())
      return size[set];
    return -1;
  }

  void unionize(int i, int j) {
    if (i == j)
      return;

    const auto root1 = getSet(i);
    const auto root2 = getSet(j);
    if (root1 == root2)
      return;

    const auto sz = getSize(root1) + getSize(root2);
    if (rank[root1] > rank[root2])
      parent[root2] = root1;
    else if (rank[root1] < rank[root2])
      parent[root1] = root2;
    else {
      parent[root2] = root1;
      ++rank[root1];
    }

    size[getSet(i)] = sz;
  }

 private:
  std::vector<int> parent;
  std::vector<int> rank;
  std::vector<int> size;

  int findCompress(int i) {
    // Iterative version.
    std::vector<int> stack{i};
    for (; parent[i] != i; i = parent[i])
      stack.push_back(parent[i]);

    const auto root = stack.back();
    for (auto i : stack)
      parent[i] = root;

    return root;
  }
};