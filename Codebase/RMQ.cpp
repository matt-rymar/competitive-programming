template <typename T>
int logFloor(T n) {
  if (n <= 0)
    return -1;
  int res = -1;
  while (n) {
    ++res;
    n >>= 1;
  }
  return res;
}

// Allows us to provide a convenient default for the RMQ operation while
// allowing the use of a general idempotent operation.
template <typename T>
struct MinWrapper {
  const T& operator()(const T& t1, const T& t2) const {
    return std::min(t1, t2);
  }
};

// Allows for O(1) range "minimum" queries over a sequence a_1, ..., a_n, for a
// binary idempotent operation op, where "minimum" min(i, j) of range
// a_i,...,a_j, i <= j is defined to be
//		a_i, if i == j
//		op(a_i, min(i+1, j)), if i < j).
//
// Does O(nlgn) precomputation and uses O(nlgn) space
//
// For example, see Codeforces 191C--Fools and Roads.
template <typename T = int, typename BinaryIdempotentOp = MinWrapper<T>>
class RMQ {
 public:
  // Creates an empty RMQ structure.
  RMQ(BinaryIdempotentOp operation = MinWrapper<T>{})
      : op(std::move(operation)) {}

  // Automatically constructs the RMQ structure based on the sequence and the
  // operation given.
  RMQ(std::vector<T> vec, BinaryIdempotentOp operation = MinWrapper<T>{})
      : RMQ(operation) {
    construct(std::move(vec));
  }

  // (Re-) constructs the sparse table corresponding to the sequence in vec.
  void construct(std::vector<T> vec) {
    fillLogs(vec.size() + 1);
    mins = std::vector<std::vector<T>>(logs[vec.size()] + 1,
                                       std::vector<T>(vec.size()));
    mins[0] = std::move(vec);
    for (int i = 1; i < mins.size(); ++i) {
      for (int j = 0; j < mins[i].size(); ++j) {
        const int upper =
            std::min(int(mins[i].size()) - 1, j + ((int(1)) << (i - 1)));
        mins[i][j] = op(mins[i - 1][j], mins[i - 1][upper]);
      }
    }
  }

  // Returns the "minimum" value (according to the operation) in the range [lo,
  // hi].
  T getMin(const int lo, const int hi) const {
    const auto loggo = logs[hi - lo + 1];
    return op(mins[loggo][lo], mins[loggo][hi - (int(1) << loggo) + 1]);
  }

 private:
  inline static std::vector<int> logs = std::vector<int>{-1, 0};

  // Our main sparse table of size O(nlogn) where first dimension are the
  // lengths.
  std::vector<std::vector<T>> mins;
  BinaryIdempotentOp op;

  // Fills the static logs vector so that it has size >= n+1, i.e. contains
  // log-floors for (at least) all integers up to n inclusive.
  void fillLogs(int n) {
    for (int i = logs.size(); i <= n; ++i)
      logs.push_back(logs[i / 2] + 1);
  }
};