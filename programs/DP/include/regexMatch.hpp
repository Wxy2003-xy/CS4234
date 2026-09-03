#pragma once

#include <bits/stdc++.h>
using namespace std;

inline void print_dp(int* dp, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cout << dp[i * m + j] << " ";
        }
        cout << endl;
    }
}

class RegexMatcher {
public:
    RegexMatcher() : dp(NULL) {}

    ~RegexMatcher() {
        free(dp);
    }

    int match(string s, string p) {
        int n = s.length();
        int m = p.length();
        int k = m + 1;
        init_dp(n, m, 0);
        dp[0] = 1;
        for (int i = 1; i <= n; i++) {
            dp[i * k] = 0;
        }
        for (int i = 1; i <= m; i++) {
            if (p[i - 1] == '*') {
                if (i > 1) {
                    dp[i] = dp[i - 2];
                }
            } else {
                dp[i] = 0;
            }
        }
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                if (p[j - 1] == '*') {
                    if (j > 1) {
                        dp[i * k + j] = dp[i * k + j - 2]
                            || (dp[(i - 1) * k + j]
                                && (p[j - 2] == '.'
                                    || s[i - 1] == p[j - 2]));
                    }
                } else {
                    dp[i * k + j] = dp[(i - 1) * k + j - 1]
                        && (p[j - 1] == '.'
                            || p[j - 1] == s[i - 1]);
                }
            }
        }
        print_dp(dp, n + 1, m + 1);
        return dp[n * k + m];
    }

    bool isMatch(string s, string p) {
        return match(std::move(s), std::move(p));
    }

private:
    int* dp;

    void init_dp(int string_len, int pattern_len, bool compress) {
        free(dp);
        if (!compress) {
            int* alloc = (int*)malloc(
                sizeof(int) * (string_len + 1) * (pattern_len + 1)
            );
            if (alloc == NULL) {
                exit(0);
            }
            dp = alloc;
            return;
        }

        int str_len_int_count = ceil(
            string_len / (8 * sizeof(int))
        );
        int pat_len_int_count = ceil(
            pattern_len / (8 * sizeof(int))
        );
        int* alloc = (int*)malloc(
            sizeof(int) * str_len_int_count * pat_len_int_count
        );
        if (alloc == NULL) {
            exit(0);
        }
        dp = alloc;
    }

    bool access_bit(int src, int logical_idx_ofst) {
        return 0;
    }
};
