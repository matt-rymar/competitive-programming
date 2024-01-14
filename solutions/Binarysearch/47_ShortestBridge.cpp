std::vector<std::pair<int, int>> getAdjacent(const std::vector<std::vector<int>>& matrix, const int i, const int j)
{
    std::vector<std::pair<int, int>> res;
    if (i > 0)
        res.emplace_back(i - 1, j);
    if ((i + 1) < (matrix.size()))
        res.emplace_back(i + 1, j);
    if (j > 0)
        res.emplace_back(i, j - 1);
    if ((j + 1) < (matrix.front().size()))
        res.emplace_back(i, j + 1);
    return res;
}

std::set<std::pair<int, int>> connectedComponent(const std::vector<std::vector<int>>& matrix)
{
    int i = 0, j = 0;
    for (; (i < matrix.size()) && (matrix[i][j] != 1); ++i) {
        for (j = 0; (j < matrix[i].size()) && (matrix[i][j] != 1); ++j)
            ;
        if (j >= matrix[i].size())
            j = 0;
        else
            break;
    }

    std::queue<std::pair<int, int>> q;
    q.emplace(i, j);
    std::set<std::pair<int, int>> visited;
    visited.emplace(i, j);
    while (!q.empty()) {
        auto [i, j] = q.front();
        q.pop();
        for (auto [m, n] : getAdjacent(matrix, i, j)) {
            if ((matrix[m][n] != 1) || (visited.count({m, n}) > 0))
                continue;
            visited.emplace(m, n);
            q.emplace(m, n);
        }
    }

    return visited;
}

int solve(vector<vector<int>>& matrix) 
{
    auto visited = connectedComponent(matrix);
    auto dists = std::map<std::pair<int, int>, int>();
    auto q = std::queue<std::pair<int, int>>();
    for (auto p : visited) {
        dists[p] = 0;
        q.push(p);
    }
    while (!q.empty()) {
        auto [i, j] = q.front();
        q.pop();
        for (auto r : getAdjacent(matrix, i, j)) {
            if (visited.count(r) > 0)
                continue;
            visited.insert(r);
            dists[r] = dists[{i, j}] + 1;
            q.push(r);
            if (matrix[r.first][r.second] == 1)
                return dists[r] - 1;
        }
    }
    return -1;
}