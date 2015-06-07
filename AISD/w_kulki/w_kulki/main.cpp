#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, const char * argv[]) {
    int t;
    cin >> t;
    for(int ti = 0; ti < t; ++ti) {
        int d;
        cin >> d;
        vector<int> p(d);
        for(int i = 0; i < d; ++i) {
            cin >> p[i];
        }
        
        int x = 1000000000;
        int maximum = *max_element(p.begin(), p.end());
        for(int k = 1; k <= maximum; ++k) {
            int moves = k;
            for(int i = 0; i < d; ++i) {
                moves += p[i] / k + bool(p[i] % k) - 1;
            }
            x = moves < x ? moves : x;
        }
        
        cout << "Case #" << ti+1 << ": " << x << endl;
    }
    return 0;
}
