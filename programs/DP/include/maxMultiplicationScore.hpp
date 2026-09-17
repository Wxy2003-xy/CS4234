#pragma once

#include <bits/stdc++.h>

using namespace std;

int maximumScore(vector<int>& nums, vector<int>& multipliers) {
    int n=nums.size();int m=multipliers.size();
    vector<vector<int>> dp(n, vector<int>(n, -1e9));
    for (int l=0;l<n;l++){
        for(int r=n-1;r>=0;r--){
            int removed = l + (n - 1 - r);
            if(r<l||removed>m){continue;}
            if (removed == 0) {
                dp[0][n-1] = 0;
                continue;
            }
            int k = removed - 1;
             if (l == 0) {
                dp[l][r]=dp[l][r+1]+nums[r+1]*multipliers[r+1];
            } else if (r == n-1) {
                dp[l][r]=dp[l-1][r]+nums[l-1]*multipliers[l-1];
            } else {
                dp[l][r]=max(dp[l-1][r]+nums[l-1]*multipliers[k],dp[l][r+1]+nums[r+1]*multipliers[k]);
            }
        }
    }   
    int res = -1e9;
    for(int i=0;i<n;i++) {
        for(int j=0;j<n;j++) {
            res=max(res,dp[i][j]);
        }
    }
    return res;
}