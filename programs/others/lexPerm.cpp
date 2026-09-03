#include<bits/stdc++.h>
using namespace std;

class Solution {
public:
    string lexGreaterPermutation(string s, string target) {
        unordered_map<char, int> map;
        for (auto c:s) {
            if (map.at(c)) {
                map.at(c) = map.at(c) + 1;
            } else {
                map.insert({c, 1});
            }
        }
    }
};

int main() {
    Solution sol;
    string s = "ababababababababababababababababababababababababab";
    string t = "bababababababababababababababababababababababababa";
    cout << sol.lexGreaterPermutation(s, t) <<endl;
}