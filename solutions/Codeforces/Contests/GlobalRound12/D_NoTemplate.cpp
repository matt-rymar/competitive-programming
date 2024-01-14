#include <iostream>
#include <vector>

template <typename T>
std::vector<T> readVecSz(int sz)
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
std::vector<T> readVec()
{
	int sz;
	std::cin >> sz;
	return readVecSz<T>(sz);
}

// For every i, res[i] = j, where j > i is the closest index j such that vec[i] > vec[j] (or vec.size() if no such j exists)
template<typename T>
std::vector<int> nextSmaller(const std::vector<T>& vec)
{
	auto res = std::vector<int>(vec.size(), vec.size());
	for (int i = (int)vec.size() - 2; i >= 0; --i) {
		auto j = i + 1;
		for (; (j < vec.size()) && (vec[i] <= vec[j]); j = res[j])
			;
		res[i] = j;
	}
	return res;
}

// For every i, res[i] = j, where j < i is the closest index j such that vec[i] > vec[j] (or -1 if no such j exists)
template<typename T>
std::vector<int> prevSmaller(const std::vector<T>& vec)
{
	auto res = std::vector<int>(vec.size(), -1);
	for (int i = 1; i < vec.size(); ++i) {
		auto j = i - 1;
		for (; (j >= 0) && (vec[i] <= vec[j]); j = res[j])
			;
		res[i] = j;
	}
	return res;
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	int t = 1;
	std::cin >> t;
	for (int i = 0; i < t; ++i) {
		auto vec = readVec<int>();
		auto prev = prevSmaller(vec);
		auto next = nextSmaller(vec);
		auto maxRange = std::vector<int>(vec.size() + 1, 0);
		for (int i = 0; i < vec.size(); ++i)
			maxRange[vec[i]] = std::max(maxRange[vec[i]], next[i] - prev[i] - 1);
		for (int i = 2; i < maxRange.size(); ++i)
			maxRange[i] = std::min(maxRange[i], maxRange[i - 1]);
		for (int k = 1; k <= vec.size(); ++k)
			std::cout << (maxRange[vec.size() - k + 1] >= k);
		std::cout << '\n';
	}
}