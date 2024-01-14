class UnionFind {
 public:
  int getSet(int i) {
    if (exists(i))
      return findCompress(i);

    parent[i] = i;
    rank[i] = 0;
    size[i] = 1;

    return i;
  }

  int getSize(int i) {
    if (!exists(i))
      return 0;
    return size[getSet(i)];
  }

  void unionize(int i, int j) {
    if (i == j)
      return;

    auto root1 = getSet(i);
    auto root2 = getSet(j);

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

  bool exists(int i) { return parent.count(i) > 0; }

 private:
  std::unordered_map<int, int> parent;
  std::unordered_map<int, int> rank;
  std::unordered_map<int, int> size;

  int findCompress(int i) {
    // Iterative version
    std::vector<int> stack{i};
    for (; parent[i] != i; i = parent[i])
      stack.push_back(parent[i]);
    auto root = stack.back();
    for (auto i : stack)
      parent[i] = root;
    return root;

    /* // RECURSIVE VERSION
    if (parent[i] == i)
            return i;
    parent[i] = findCompress(parent[i]);
    return parent[i];
    */
  }
};