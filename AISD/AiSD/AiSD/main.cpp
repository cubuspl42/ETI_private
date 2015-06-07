#include "algorithm.hpp"

#include <cassert>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

typedef nonstd::vector<double> Matrix;

unsigned t;
unsigned ti;
unsigned max_num_villages, n;
nonstd::vector<nonstd::pair<int, int>> villages;
Matrix dist;

double &a(nonstd::vector<double> &v, unsigned n, unsigned y, unsigned x) {
    return v[y * n + x];
}

#define DIST(u, v) a(dist, n, u, v)

double tsp_best;

void tsp_r(unsigned s, double d, uint64_t mask) {
    if(d > tsp_best)
        return;
    mask |= 1 << s;
    if((~mask & ((1 << n) - 1)) == 0) {
        d += DIST(0, s);
        if(d < tsp_best)
            tsp_best = d;
    } else for(unsigned i = 0; i < n; ++i) {
        if(!(mask & (1 << i))) {
            tsp_r(i, d + DIST(s, i), mask);
        }
    }
}

void test() {
    scanf("%d", &max_num_villages);
    assert(max_num_villages <= 64);
    villages.clear();
    n = 0;
    
    getchar(), getchar(); // " ("
    
    for(unsigned i = 0; i < max_num_villages; ++i) {
        bool found = false;
        int y, x;
        scanf("(%d,%d)", &x, &y);
        getchar(); // "," / ")"
        
        for(unsigned j = 0; j < n; ++j) {
            int vy = villages[j].first, vx = villages[j].second;
            if(vy == y && vx == x) {
                found = true;
                break;
            }
        }
        
        if(!found) {
            villages.push_back(nonstd::make_pair(y, x));
            ++n;
        }
    }
    
    dist.resize(n*n);
    
    for(unsigned i = 0; i < n; ++i) {
        for(unsigned j = 0; j < n; ++j) {
            if(i != j) {
                int x0 = villages[i].second, y0 = villages[i].first;
                int x1 = villages[j].second, y1 = villages[j].first;
                int dx = x1 - x0, dy = y1 - y0;
                double d = sqrt((double)(dx*dx + dy*dy));
                if(d < 1000) {
                    DIST(i, j) = d;
                } else if (dy == 0) {
                    DIST(i, j) = abs(dx);
                } else {
                    DIST(i, j) = abs(x0) + abs(dy) + abs(x1);
                }
            }
        }
    }
    
    tsp_best = 1000000000.0;
    tsp_r(0, 0.0f, 0);
    printf("%d\n", (int)floor(tsp_best));
}

int main(int argc, const char * argv[]) {
    std::ios::sync_with_stdio(false);
    scanf("%d", &t);
    villages.reserve(64);
    dist.reserve(48*48);
    for(ti = 0; ti < t; ++ti) {
        test();
    }
    return 0;
}
