// Trie for strings containing *lowercase* ASCII letters, i.e. [a-z].
//
// For an example, see
//      -- Google Kickstart 2020A--Bundling
//      -- Codeforces 1476E--Pattern Matching
struct Trie {
 public:
  // Adds all the strings in the vector to the trie.
  // The id of each string is the index in the original vector.
  Trie(const std::vector<std::string>& vec) {
    for (int i = 0; i < vec.size(); ++i)
      insert(vec[i], i);
  }

  // Pushes the string onto the Trie, with a given ID.
  void insert(std::string_view sv, int id) {
    Node* curNode = &root;
    for (auto c : sv) {
      ++curNode->sizeWithDuplicates;
      const auto idx = c - baseLetter;
      if (!(curNode->children[idx]))
        curNode->children[idx] = std::make_unique<Node>();
      curNode = curNode->children[idx].get();
    }
    ++curNode->sizeWithDuplicates;
    curNode->tick = true;
    curNode->id = id;
  }

  // Checks whether the string exists in the Trie. If yes, returns the ID of the
  // string,=.
  std::optional<int> exists(std::string_view sv) const {
    const auto node = getNode(sv);
    if (node && node->tick)
      return node->id;
    return std::nullopt;
  }

 private:
  static constexpr char baseLetter = 'a';

  struct Node {
    static constexpr size_t alphabetSize = 26;

    std::vector<std::unique_ptr<Node>> children =
        std::vector<std::unique_ptr<Node>>(alphabetSize);
    bool tick = false;
    int id = -1;
    // Stores the number of strings in the current subtrie, counting duplicates.
    // That is, it basically stores how many times insert() has reached the
    // current node.
    int sizeWithDuplicates = 0;
  };
  Node root;

  // Returns the Node corresponding to the given string (or nullptr if the
  // string does not exist in the trie).
  const Node* getNode(std::string_view sv) const {
    const Node* curNode = &root;
    for (auto c : sv) {
      const auto idx = c - baseLetter;
      if (!(curNode->children[idx]))
        return nullptr;
      curNode = curNode->children[idx].get();
    }
    return curNode;
  }
};