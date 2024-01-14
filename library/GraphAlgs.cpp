// ------------------------------------------------------------
// Reverse adjacency list stuff.
// ------------------------------------------------------------

// Returns the reverse adjacency list of a graph.
std::vector<std::vector<int>> getReverseAdj(
    const std::vector<std::vector<int>>& adj) {
  auto res = std::vector<std::vector<int>>(adj.size());
  for (int i = 0; i < adj.size(); ++i) {
    for (auto j : adj[i])
      res[j].push_back(i);
  }
  return res;
}

// ------------------------------------------------------------
// Topological sort stuff.
// ------------------------------------------------------------

std::vector<int> inDegrees(const std::vector<std::vector<int>>& adj) {
  auto res = std::vector<int>(adj.size());
  for (const auto& v : adj) {
    for (auto i : v)
      ++res[i];
  }
  return res;
}

// Sorts the input dag topologically.
// Returns a list of vertices in a topological order (or nothing, if input graph
// isn't a dag).
std::optional<std::vector<int>> topologicalSort(
    const std::vector<std::vector<int>>& adj) {
  auto degs = inDegrees(adj);
  std::queue<int> degZero;
  for (int i = 0; i < degs.size(); ++i) {
    if (degs[i] == 0)
      degZero.push(i);
  }

  std::vector<int> res;
  res.reserve(adj.size());
  int count = 0;
  for (; !degZero.empty(); ++count) {
    const auto cur = degZero.front();
    degZero.pop();
    res.push_back(cur);
    for (auto v : adj[cur]) {
      --degs[v];
      if (degs[v] == 0)
        degZero.push(v);
    }
  }

  if (count < adj.size())
    return {};
  return res;
}

// ------------------------------------------------------------
// Generalized topological sort stuff.
// ------------------------------------------------------------

// Returns the indegrees of all vertices i where reps[i] == i (see the
// topologicalSort function below). The first element of the pair is the number
// of vertices in the graph (i.e. number of i's for which reps[i] == i).
std::pair<int, std::vector<int>> getInDegrees(
    const std::vector<std::vector<int>>& adj, const std::vector<int>& reps) {
  int count = 0;
  auto res = std::vector<int>(adj.size());

  for (int i = 0; i < adj.size(); ++i) {
    if (i != reps[i])
      continue;
    ++count;
    for (auto j : adj[i]) {
      if (reps[j] != i)  // Ensure no self loops added to the graph.
        ++res[reps[j]];
    }
  }

  return {count, res};
}

// Returns a queue of vertices with i == reps[i] and inDegrees[i] == 0.
std::queue<int> getDegZero(const std::vector<int>& inDegrees,
                           const std::vector<int>& reps) {
  std::queue<int> res;
  for (int i = 0; i < inDegrees.size(); ++i) {
    if ((i == reps[i]) && (inDegrees[i] == 0))
      res.push(i);
  }
  return res;
}

// A generalization of topological sort where we only consider the vertices i
// where reps[i] == i ("reps" stands for "representants"). That is, if a vertex
// i (with reps[i] == i) has an edge to j, we consider it an edge from i to
// reps[j].'
//
// Note: ignores self loops in the graph, i.e., edges i->j for which reps[i] ==
// reps[j].
//
// For an example, see
// https://leetcode.com/problems/sort-items-by-groups-respecting-dependencies
std::optional<std::vector<int>> topologicalSort(
    const std::vector<std::vector<int>>& adj, const std::vector<int>& reps) {
  auto [noVertices, inDegrees] = getInDegrees(adj, reps);
  auto degZero = getDegZero(inDegrees, reps);

  std::vector<int> res;
  while (!degZero.empty()) {
    const auto cur = degZero.front();
    degZero.pop();

    res.push_back(reps[cur]);
    for (auto i : adj[cur]) {
      if ((--inDegrees[reps[i]]) == 0)
        degZero.push(reps[i]);
    }
  }

  if (res.size() != noVertices)
    return {};
  return res;
}

// ------------------------------------------------------------
// Dijkstra stuff.
// ------------------------------------------------------------

struct Neighbor {
  int id;
  int w;
};

struct NodeDist {
  int v;
  int64_t d;
};

bool operator<(const NodeDist& nd1, const NodeDist& nd2) {
  return nd1.d > nd2.d;
}

int64_t dijkstra(const std::vector<std::vector<Neighbor>>& adj, int s, int t) {
  auto dists = std::vector<int64_t>(adj.size(), -1);
  dists[s] = 0;
  auto flags = std::vector<bool>(adj.size(), false);
  std::priority_queue<NodeDist> q;
  q.push(NodeDist{int(s), 0});
  while (!q.empty()) {
    const auto nd = q.top();
    q.pop();
    if (nd.v == t)
      return dists[t];
    if (flags[nd.v])
      continue;
    flags[nd.v] = true;
    for (auto neigh : adj[nd.v]) {
      if ((dists[neigh.id] < 0) || (dists[nd.v] + neigh.w < dists[neigh.id])) {
        dists[neigh.id] = dists[nd.v] + neigh.w;
        q.push(NodeDist{neigh.id, dists[neigh.id]});
      }
    }
  }
  return dists[t];
}

std::vector<int64_t> dijkstra(const std::vector<std::vector<Neighbor>>& adj,
                              int s) {
  auto dists = std::vector<int64_t>(adj.size(), -1);
  dists[s] = 0;
  auto flags = std::vector<bool>(adj.size(), false);
  std::priority_queue<NodeDist> q;
  q.push(NodeDist{int(s), 0});
  while (!q.empty()) {
    auto nd = q.top();
    q.pop();
    if (flags[nd.v])
      continue;
    flags[nd.v] = true;
    for (auto neigh : adj[nd.v]) {
      if ((dists[neigh.id] < 0) || (dists[nd.v] + neigh.w < dists[neigh.id])) {
        dists[neigh.id] = dists[nd.v] + neigh.w;
        q.push(NodeDist{neigh.id, dists[neigh.id]});
      }
    }
  }
  return dists;
}

std::vector<std::vector<int64_t>> dijkstra_apsp(
    const std::vector<std::vector<Neighbor>>& adj) {
  std::vector<std::vector<int64_t>> res;
  res.reserve(adj.size());
  for (int s = 0; s < adj.size(); ++s)
    res.push_back(dijkstra(adj, s));
  return res;
}

// ------------------------------------------------------------
// Bipartitedness check stuff.
// ------------------------------------------------------------

// Checks whether the (undirected) graph given by adj is bipartite.
bool isBipartite(const std::vector<std::vector<int>>& adj) {
  enum class Color { red, blue, none };
  auto colors = std::vector<Color>(adj.size(), Color::none);
  auto bfs = [&](int i) -> bool {
    std::queue<int> q;
    q.push(i);
    colors[i] = Color::red;
    while (!q.empty()) {
      auto cur = q.front();
      q.pop();
      for (auto i : adj[cur]) {
        if (colors[i] == colors[cur])
          return false;
        if (colors[i] == Color::none) {
          colors[i] = (colors[cur] == Color::red) ? Color::blue : Color::red;
          q.push(i);
        }
      }
    }
    return true;
  };

  for (int i = 0; i < adj.size(); ++i) {
    if (colors[i] != Color::none)
      continue;
    if (!bfs(i))
      return false;
  }

  return true;
}

// ------------------------------------------------------------
// Floyd-Warshall stuff.
// ------------------------------------------------------------

// Returns the matrix of distances between each pair of nodes in the graph
// represented by the matrix of weights. If a negative-weight cycle exists,
// returns a std::nullopt.
//
// Note: assumes that the graph does not contain self-loops, i.e.
// weightMatrix[i][i] == 0 for all i.
std::optional<std::vector<std::vector<int>>> floydWarshall(
    std::vector<std::vector<int>> weightMatrix) {
  // Just to be sure weightMatrix[i][i] is sensibly initialized.
  for (int i = 0; i < weightMatrix.size(); ++i)
    weightMatrix[i][i] = 0;

  auto prev = weightMatrix;
  // Compute the shortest path lengths.
  for (int i = 0; i < weightMatrix.size(); ++i) {
    for (int j = 0; j < weightMatrix.size(); ++j) {
      for (int k = 0; k < weightMatrix.size(); ++k) {
        weightMatrix[j][k] = prev[j][k];
        if ((prev[j][i] != POS_INF<int>)&&(prev[i][k] != POS_INF<int>)&&(
                (prev[j][i] + prev[i][k]) < prev[j][k]))
          weightMatrix[j][k] = prev[j][i] + prev[i][k];
      }
    }
    prev = weightMatrix;
  }
  // Check for negative cycles by checking if we found a better path from a
  // vertex to itself.
  for (int i = 0; i < weightMatrix.size(); ++i) {
    if (prev[i][i] < 0)
      return std::nullopt;
  }
  return prev;
}

// ------------------------------------------------------------
// SCC Stuff.
// ------------------------------------------------------------

void sccsForwardDfs(const std::vector<std::vector<int>>& adj,
                    std::vector<bool>& visited, std::stack<int>& ord, int u) {
  visited[u] = true;
  for (auto i : adj[u]) {
    if (visited[i])
      continue;
    sccsForwardDfs(adj, visited, ord, i);
  }
  ord.push(u);
}

void sccsBackwardDfs(const std::vector<std::vector<int>>& adj,
                     std::vector<int>& who, int rep, int u) {
  who[u] = rep;
  for (auto i : adj[u]) {
    if (who[i] >= 0)
      continue;
    sccsBackwardDfs(adj, who, rep, i);
  }
}

// Kosaraju's Algorithm.
std::pair<std::vector<int>, std::vector<int>> stronglyConnectedComponents(
    const std::vector<std::vector<int>>& adj) {
  std::stack<int> order;
  auto visited = std::vector<bool>(adj.size(), false);
  for (int i = 0; i < adj.size(); ++i) {
    if (visited[i])
      continue;
    sccsForwardDfs(adj, visited, order, i);
  }

  const auto revAdj = getReverseAdj(adj);
  auto who = std::vector<int>(adj.size(), -1);
  std::vector<int> topOrder;
  while (!order.empty()) {
    auto cur = order.top();
    order.pop();
    if (who[cur] >= 0)
      continue;
    sccsBackwardDfs(revAdj, who, cur, cur);
    topOrder.push_back(cur);
  }
  return {who, topOrder};
}

/*
// Returns a vector vec where vec[i] = size of SCC if i = who[i] and vec[i] = 0
otherwise.
// WARNING: Untested.
std::vector<int> getSccSizes(const std::vector<int>& who)
{
    auto res = std::vector<int>(who.size());
    for (auto i : who)
        ++res[i];
    return res;
}

// Returns a vector that contains the graph over the SCCs, but not compressed,
i.e., it has just as many vertices as adj, but only the ones such that who[i] =
i are meaningful.
// Note that it does not include duplicate edges.
// WARNING: Untested.
std::vector<std::unordered_set<int>> getKindaSccGraph(const
std::vector<std::vector<int>>& adj, const std::vector<int>& who)
{
    auto res = std::vector<std::unordered_set<int>>(adj.size());
    for (int i = 0; i < adj.size(); ++i) {
        for (auto j : adj[i])
            res[who[i]].insert(who[j]);
    }
    return res;
}
*/

// ------------------------------------------------------------
// Euler Tour (on trees) stuff.
// ------------------------------------------------------------

struct EulerTourData {
  EulerTourData(int n) : parent(n, -1), firstVisit(n, -1), lastVisit(n, -1) {}

  struct TourVisit {
    int vertex;
    int depth;
  };

  std::vector<TourVisit> eulerTour;

  // Parent array built during the traversal of the tree.
  std::vector<int> parent;

  // For the following, firstVisit[i] = first time vertex i appears in
  // eulerTour. Hence, eulerTour[firstVisit[i]].vertex = i and similarly,
  // eulerTour[lastVisit[i]].vertex = i.
  std::vector<int> firstVisit;
  std::vector<int> lastVisit;
};

// Performs an Euler Tour on (what is assummed to be) a tree with a given root.
//
// For example, see Codeforces Task 1467E--Distinctive Roots in a Tree
EulerTourData eulerTour(const std::vector<std::vector<int>>& adj,
                        const int root = 0) {
  auto res = EulerTourData(adj.size());

  auto tour =
      y_combinator([&](auto tour, const int cur, const int depth) -> void {
        res.firstVisit[cur] = res.eulerTour.size();

        const auto curInfo =
            EulerTourData::TourVisit{.vertex = cur, .depth = depth};
        res.eulerTour.push_back(curInfo);

        for (auto i : adj[cur]) {
          if (i == res.parent[cur])
            continue;
          res.parent[i] = cur;
          tour(i, depth + 1);
          res.eulerTour.push_back(curInfo);
        }

        res.lastVisit[cur] = res.eulerTour.size() - 1;
      });
  tour(root, 0);

  return res;
}

// ------------------------------------------------------------
// LCA queries in O(1) stuff.
// ------------------------------------------------------------

// Supports O(1) LCA queries on a tree. (With O(nlogn) precomputation time.)\
//
// For example, see Codeforces 191C--Fools and Roads
class LCA {
 public:
  LCA(const std::vector<std::vector<int>>& adj, EulerTourData tourData)
      : rmq(tourVisitMin), tour(std::move(tourData)) {
    rmq.construct(tour.eulerTour);
  }

  LCA(const std::vector<std::vector<int>>& adj, int root = 0)
      : LCA(adj, eulerTour(adj, root)) {}

  // Returns the lowest common ancestor of nodes u and v.
  int lca(int u, int v) const {
    if ((!inGraph(u)) || (!inGraph(v)))
      return -1;
    const auto [left, right] =
        std::minmax(tour.firstVisit[u], tour.firstVisit[v]);
    return rmq.getMin(left, right).vertex;
  }

  // Returns the depth of the node u.
  int depth(int u) const {
    if (!inGraph(u))
      return -1;
    return tour.eulerTour[tour.firstVisit[u]].depth;
  }

 private:
  using TourVisit = EulerTourData::TourVisit;
  static constexpr auto tourVisitMin = [](const TourVisit& t1,
                                          const TourVisit& t2) {
    if (t1.depth < t2.depth)
      return t1;
    return t2;
  };

  bool inGraph(int u) const { return (u >= 0) && (u < tour.firstVisit.size()); }

  EulerTourData tour;
  RMQ<TourVisit, decltype(tourVisitMin)> rmq;
};

// ------------------------------------------------------------
// Euler cycle stuff.
// ------------------------------------------------------------

// Finds an Euler cycle.
// Note: assumes that the graph is Eulerian, otherwise the result is undefined.
std::vector<int> eulerCycle(std::vector<std::unordered_set<int>> adj,
                            bool directed) {
  if (adj.empty())
    return {};

  auto res = std::vector<int>();

  auto st = std::stack<int>();
  st.push(0);
  while (!st.empty()) {
    const auto cur = st.top();
    if (adj[cur].empty()) {
      res.push_back(cur);
      st.pop();
    } else {
      const auto next = *adj[cur].cbegin();
      adj[cur].erase(next);
      if (!directed)
        adj[next].erase(cur);
      st.push(next);
    }
  }

  std::reverse(res.begin(), res.end());

  return res;
}

// A convenience overload for a graph in the typical adjacency list form.
// See the main overload for a more detailed description.
//
// For example uses, see:
//	- Leetcode 753. Cracking the Safe
// https://leetcode.com/problems/cracking-the-safe/
//	- CSES 1691--Mail Delivery https://cses.fi/problemset/task/1691/
std::vector<int> eulerCycle(const std::vector<std::vector<int>>& adj,
                            bool directed) {
  auto asSet = std::vector<std::unordered_set<int>>();
  for (const auto& v : adj)
    asSet.emplace_back(v.cbegin(), v.cend());
  return eulerCycle(std::move(asSet), directed);
}

// ------------------------------------------------------------
// Find bridges stuff.
// ------------------------------------------------------------

// Returns all bridges in the graph.
//
// For an example, see Codeforces Task 118E--Bertown Roads
std::vector<std::pair<int, int>> getBridges(
    const std::vector<std::vector<int>>& adj) {
  auto arrival = std::vector<int>(adj.size(), -1);
  auto low = std::vector<int>(adj.size(), -1);
  int curTime = 0;
  std::vector<std::pair<int, int>> res;

  auto dfs = y_combinator([&](auto dfs, int cur, int parent) -> void {
    if (arrival[cur] >= 0)
      return;
    arrival[cur] = low[cur] = curTime++;
    for (auto i : adj[cur]) {
      if (i == parent)
        continue;
      if (arrival[i] >= 0) {
        low[cur] = std::min(low[cur], arrival[i]);
        continue;
      }
      dfs(i, cur);
      low[cur] = std::min(low[cur], low[i]);
      if (low[i] > arrival[cur])
        res.emplace_back(cur, i);
    }
  });

  for (int i = 0; i < adj.size(); ++i)
    dfs(i, -1);

  return res;
}

// ------------------------------------------------------------
// Find cut vertices stuff.
// ------------------------------------------------------------

// Returns all cut vertices in the graph
/
    // For an example use, see https://www.spoj.com/problems/SUBMERGE/
    std::unordered_set<int> getCutVertices(
        const std::vector<std::vector<int>>& adj) {
  auto arrival = std::vector<int>(adj.size(), -1);
  auto low = std::vector<int>(adj.size(), -1);
  int curTime = 0;
  int rootComponents = 0;
  std::unordered_set<int> res;

  auto dfs = y_combinator([&](auto dfs, int cur, int parent) -> void {
    if (arrival[cur] >= 0)
      return;
    arrival[cur] = low[cur] = curTime++;
    for (auto i : adj[cur]) {
      if (i == parent)
        continue;
      if (arrival[i] >= 0) {
        low[cur] = std::min(low[cur], arrival[i]);
        continue;
      }
      if (arrival[cur] == 0)
        ++rootComponents;
      dfs(i, cur);
      low[cur] = std::min(low[cur], low[i]);
      if ((low[i] >= arrival[cur]) &&
          ((arrival[cur] != 0) || (rootComponents > 1)))
        res.insert(cur);
    }
  });

  for (int i = 0; i < adj.size(); ++i)
    dfs(i, -1);

  return res;
}

// ------------------------------------------------------------
// Find cycle in undirected stuff (because once I wasted time on stupid bugs
// writing this).
// ------------------------------------------------------------

std::optional<std::pair<int, int>> getCycle_recursive(
    const std::vector<std::vector<int>>& adj, int cur,
    std::vector<int>& parent) {
  for (auto i : adj[cur]) {
    if (i == parent[cur])
      continue;
    if (parent[i] >= 0)
      return {{i, cur}};
    parent[i] = cur;
    if (auto cyc = getCycle_recursive(adj, i, parent); cyc)
      return cyc;
  }
  return {};
}

// Returns an arbitrary cycle in the input (undirected) graph (or std::nullopt
// if no cycles exist).
//
// Note: doesn't work for directed graphs.
std::optional<std::vector<int>> getCycle(
    const std::vector<std::vector<int>>& adj) {
  auto parent = std::vector<int>(adj.size(), -1);
  std::optional<std::pair<int, int>> endpoints;
  for (int i = 0; i < adj.size(); ++i) {
    if (parent[i] >= 0)
      continue;
    parent[i] = i;
    endpoints = getCycle_recursive(adj, i, parent);
    if (endpoints)
      break;
  }
  if (!endpoints)
    return {};
  auto [from, to] = *endpoints;
  std::vector<int> res{to};
  for (auto cur = parent[to]; res.back() != from; cur = parent[cur])
    res.push_back(cur);
  return res;
}
