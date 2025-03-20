#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    static vector<vector<string>> groupAnagrams(vector<string>& strs) {
        
        std::unordered_map<std::string, std::vector<string>> anagrams;

        for (auto& word : strs) {
            std::string key(word);
            std::sort(key.begin(), key.end());
            anagrams[key].push_back(word);
        }

        std::vector<std::vector<std::string>> ret;
        for (auto& [key, values] : anagrams) {
            ret.push_back(values);
        }

        return ret;
    }
};

int main() {
    std::vector<std::string> words = {"eat", "tea", "tan", "ate", "nat", "bat"};

    Solution sol;
    vector<vector<string>> anagrams = sol.groupAnagrams(words);

    for (auto& ana : anagrams) {
        for (auto& wd : ana) {
            std::cout << wd << " ";
        }
        std::cout << std::endl;
    }
}