#include <cassert>
#include <climits>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct Edge : vector<int> {
    int w;
};

typedef vector<Edge> Graph;

int &a(vector<int> &v, unsigned w, unsigned y, unsigned x) {
    return v[w * y + x];
}



void fw(vector<int> &dist, Graph &g) {
#define DIST(u, v) a(dist, n, u, v)
    unsigned n = (unsigned)g.size();
    dist.resize(n*n);
    for(unsigned i = 0; i < dist.size(); ++i) {
        dist[i] = INT_MAX;
    }
    for(unsigned u = 0; u < n; ++u) {
        for(unsigned v = 0; v < g[u].size(); ++v) {
            DIST(u, v) = g[u].w;
        }
    }
    for(unsigned k = 0; k < n; ++k) {
        for(unsigned i = 0; i < n; ++i) {
            for(unsigned j = 0; j < n; ++j) {
                if(DIST(i, j) > DIST(i, k) + DIST(k, j)) {
                    DIST(i, j) = DIST(i, k) + DIST(k, j);
                }
            }
        }
    }
#undef DIST
}

int main(int argc, const char * argv[]) {
    return 0;
}
