#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <utility>

using namespace std;

// class Solution {
// public:
//     string longestPalindrome(string s) {
//         string longest = "";
//         for (int left = 0; left < s.size(); left++) {
//             for (int right = s.size()-1; right >= left; right--) {
//                 string parlindrome(s.begin()+left, s.begin()+right+1);
//                 if (isParlindrome(parlindrome) && longest.size() < parlindrome.size()) {
//                     longest = parlindrome;
//                     break;
//                 }
//             }
//         }
//         return longest;
//     }
// private:
//     bool isParlindrome(const string& s) {
//         int right = s.size() - 1;
//         for (int i = 0; i < s.size()/2; i++) {
//             if (s[i] != s[right--]) return false;
//         }
//         return true;
//     }
// };


// int main() {

//     string s = "a";
//     Solution sol;
//     cout << sol.longestPalindrome(s) << endl;
// }

// class Solution {
// public:
//     vector<int> twoSum(vector<int>& nums, int target) {
//         map<int, int> tmps;
//         for (int i = 0; i < nums.size(); i++) {
//             tmps.insert({target - nums[i], i});
//         }
//         for (int i = 0; i < nums.size(); i++) {
//             auto it = tmps.find(nums[i]);
//             if (it != tmps.end()) {
//                 return {i, it->second};
//             }
//         }
//         return vector<int>();
//     }
// };

// int main() {

//     vector<int> s = {3,2,4};
//     Solution sol;
//     auto res = sol.twoSum(s, 6);
//     cout << res[0] << " " << res[1] << endl;
// }

#include <set>
#include <iostream>
#include <string>

using namespace std;

class Solution {
    public:
        int removeDuplicates(vector<int>& nums) {
            
            if (nums.size() <= 2) return 2;

            pair<int, int> previous = {nums[0], nums[1]};
            int right = 2;
            int i = 2
            for (; i < nums.size(), right < nums.size(); ++i) {
                
                if (previous.first == previous.second) {
                    while (right < nums.size() && previous.second == nums[right]) {
                        right++;
                    }
                }
                nums[i] = nums[right];
                previous.first = previous.second;
                previous.second = nums[i];
            }

            return i;
        }
    };


int main() {
    vector<int> c = {0,0,1,1,1,1,2,3,3};
    Solution sol;
    cout << sol.removeDuplicates(c) << endl;
}
