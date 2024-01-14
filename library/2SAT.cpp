std::vector<std::vector<int>> getReverseAdj(
    const std::vector<std::vector<int>>& adj) {
  auto res = std::vector<std::vector<int>>(adj.size());
  for (int i = 0; i < adj.size(); ++i) {
    for (auto j : adj[i])
      res[j].push_back(i);
  }
  return res;
}

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

std::pair<std::vector<int>, std::vector<int>> sccs(
    const std::vector<std::vector<int>>& adj) {
  std::stack<int> order;
  auto visited = std::vector<bool>(adj.size(), false);
  for (int i = 0; i < adj.size(); ++i) {
    if (visited[i])
      continue;
    sccsForwardDfs(adj, visited, order, i);
  }

  auto revAdj = getReverseAdj(adj);
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

inline int getNegVar(int x, int noVariables) {
  if (x > noVariables)
    return x - noVariables;
  return x + noVariables;
}

// Returns the implication graph corresponding to the clause set.
std::vector<std::vector<int>> getImplicationGraph(
    const std::vector<std::pair<int, int>>& clauses, int noVariables) {
  // We need a vertex for each variable and its negation (plus one for
  // convenience of indexing since our variable indices are in the range [1..n])
  // For a variable i, its negation is the vertex i+noVariables
  // Adj may contain duplicates but it shouldn't matter in the context of an
  // implication graph.
  //	(while I got MLEs when using std::unordered_set)
  auto adj = std::vector<std::vector<int>>(2 * noVariables + 1);
  for (auto [x, y] : clauses) {
    if (x < 0)
      x = -x + noVariables;
    if (y < 0)
      y = -y + noVariables;
    adj[getNegVar(x, noVariables)].push_back(y);
    adj[getNegVar(y, noVariables)].push_back(x);
  }
  return adj;
}

// TODO: change the unordered_map into a vector (with non-rep entries
// meaningless)? Possibly faster for comp. prog.
std::unordered_map<int, int> repToTopOrd(const std::vector<int>& topOrd) {
  std::unordered_map<int, int> res;
  for (int i = 0; i < topOrd.size(); ++i)
    res[topOrd[i]] = i;
  return res;
}

// Solves the 2-Sat instance given in the vector.
// Each entry of the input vector is a pair of variables, indexed in [1..n].
// If the variable is >0, then the variable appears non-negated in the clause
// and otherwise it's negated For example to translate (not p or q) and (q or
// r), you'd input [{-1, 2}, {2, 3}] Returns a satisfying assignment, if one
// exists--that is, it returns a vector v of size n+1 where
//	v[i] == true <=> i-th variable should be set to true.
std::optional<std::vector<bool>> twoSat(
    const std::vector<std::pair<int, int>>& clauses, int noVariables) {
  auto adj = getImplicationGraph(clauses, noVariables);
  auto [who, topSort] = sccs(adj);
  auto repToOrd = repToTopOrd(topSort);
  auto res = std::vector<bool>{false};
  res.reserve(noVariables + 1);
  for (int i = 1; i <= noVariables; ++i) {
    auto neg = i + noVariables;
    if (who[i] == who[neg])
      return {};
    if (repToOrd[who[i]] < repToOrd[who[neg]])
      res.push_back(false);
    else
      res.push_back(true);
  }
  return res;
}