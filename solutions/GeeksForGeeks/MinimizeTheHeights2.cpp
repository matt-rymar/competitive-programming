// { Driver Code Starts
// Initial template for C++

#include <bits/stdc++.h>
using namespace std;

 // } Driver Code Ends
// User function template for C++

class Solution {
  public:
    int getMinDiff(int arr[], int n, int k) 
    {
        std::sort(arr, arr + n);
        
        int res = arr[n - 1] - arr[0];
        for (int leftEnd = 0; leftEnd < (n - 1); ++leftEnd) {
            if (arr[leftEnd + 1] < k)
                continue;
                
            const auto minHeight = std::min(arr[0] + k, arr[leftEnd + 1] - k);
            const auto maxHeight = std::max(arr[leftEnd] + k, arr[n - 1] - k);
            
            res = std::min(res, maxHeight - minHeight);
        }
        
        return res;
    }
};

// { Driver Code Starts.
int main() {
    int t;
    cin >> t;
    while (t--) {
        int n, k;
        cin >> k;
        cin >> n;
        int arr[n];
        for (int i = 0; i < n; i++) {
            cin >> arr[i];
        }
        Solution ob;
        auto ans = ob.getMinDiff(arr, n, k);
        cout << ans << "\n";
    }
    return 0;
}  // } Driver Code Ends
