// Original task: Codeforces 963D
// https://codeforces.com/contest/963/problem/D

// NOTE: DO NOT TRY TO STORE AN EMPTY STRING OR YOU'LL GET ERRORS.
class AhoCorasick {
 public:
  // Initializes the underlying Aho-Corasick trie from the input strings.
  //
  // Note: duplicates in the input vec lead to slightly weird behavior:
  //  only the last copy of the string is stored and only that one will be
  //  reported as a match.
  AhoCorasick(const std::vector<std::string>& vec) : sz(vec.size()) {
    for (int i = 0; i < vec.size(); ++i)
      push(vec[i], i);
    ahoInitialize();
  }

  // For each string from the dictionary (as given during construction; see the
  // constructor comment about the caveat with duplicates), returns all the
  // indices in the input where the string matches (the index points to the last
  // character of the match, to be precise).
  std::vector<std::vector<int>> feed(std::string_view sv) {
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

    // If tick is true, the node corresponds to a word in the dictionary.
    bool tick = false;
    std::vector<std::unique_ptr<Node>> children =
        std::vector<std::unique_ptr<Node>>(alphabetSize);
    // Where we do transition upon reading a character: if children[i] exists
    // then transition[i] is the same; otherwise the value is (pre-) calculated
    // using fail links.
    std::vector<Node*> transition = std::vector<Node*>(alphabetSize, nullptr);
    // Points to the first suffix of our node that exists in the tree.
    Node* suffLink = nullptr;
    // First node with a tick that is reachable over suffix links.
    Node* dictLink = nullptr;
  };

  void push(std::string_view sv, int idx) {
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

  Node* findNode(std::string_view sv) {
    auto curNode = root.get();
    for (auto c : sv) {
      auto idx = c - 'a';
      if (!(curNode->children[idx]))
        return nullptr;
      curNode = curNode->children[idx].get();
    }
    return curNode;
  }

  // Initializes the transition array and the suffLink for the root and the
  // suffLinks for its children.
  //
  // Returns a queue containing root's (non-null) children.
  std::queue<Node*> initRoot() {
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

  // Computes the suffix links, transitions and the dictionary links for all
  // nodes in the trie.
  void ahoInitialize() {
    auto q = initRoot();
    while (!q.empty()) {
      // Loop invariants (cur = q.front()):
      //  - cur has its suffLink correctly initilized
      //  - all nodes in the tree with depth smaller than cur are fully
      //  initialized
      // Furthermore, we know that cur != root.get() so cur->suffLink is
      // non-null.
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