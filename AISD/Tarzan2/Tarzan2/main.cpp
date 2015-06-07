#include <cstdio>
#include <cstring>
#include <ctime>

namespace nonstd {
    template<typename T>
    struct vector
    {
        T *storage_;
        size_t size_, reserved_;
        vector() : size_(0), reserved_(0), storage_(nullptr) {
        }
        ~vector() {
            delete[] storage_;
        }
        void push_back(const T &value) {
            resize(size_+1);
            storage_[size_-1] = value;
        }
        void pop_back() {
            resize(size_-1);
        }
        bool empty() {
            return size_ == 0;
        }
        T *begin() {
            return storage_;
        }
        T *end() {
            return storage_ + size_;
        }
        T &back() {
            return storage_[size_-1];
        }
        void reserve(size_t size) {
            if(size > reserved_) {
                size_t new_reserved = size*2;
                T *new_storage = new T[new_reserved];
                if(storage_) {
                    memcpy(new_storage, storage_, size_*sizeof(T));
                    delete storage_;
                }
                storage_ = new_storage;
                reserved_ = new_reserved;
            }
        }
        void resize(size_t size) {
            reserve(size);
            size_ = size;
        }
        void clear() {
            memset(storage_, 0, size_*sizeof(T));
            resize(0);
        }
        size_t size() {
            return size_;
        }
        T &operator[](size_t i) {
            return storage_[i];
        }
    };
}

struct Mask {
    static const int N = 200;
    bool bits[N];
    int n;
    Mask() {
        n = 0;
        for(int i = 0; i < N; ++i) {
            bits[i] = false;
        }
    }
    void set(int i, bool b) {
        if(b > bits[i])
            ++n;
        if(b < bits[i])
            --n;
        bits[i] = b;
    }
    bool get(int i) const {
        return bits[i];
    }
    Mask AND(const Mask &other) const {
        Mask m;
        for(int i = 0; i < N; ++i) {
            m.set(i, bits[i] && other.bits[i]);
        }
        return m;
    }
};

struct Komisja {
    Mask m[200];
};

int n, k, p;
nonstd::vector<Komisja> komisje;
int q;

//BronKerbosch1(R, P, X):
//    if P and X are both empty:
//        report R as a maximal clique
//    for each vertex v in P:
//        BronKerbosch1(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
//        P := P \ {v}
//        X := X ⋃ {v}

bool bh(Mask R, Mask P) {
    if(R.n >= p) {
        return true;
    }
    for(int v = 0; v < n; ++v) if(P.get(v)){
        Mask R_prim(R);
        R_prim.set(v, 1);
        if(bh(R_prim, P.AND(komisje[q].m[v]))) {
            return true;
        }
        P.set(v, 0);
    }
    return false;
}

int main() {
    while(scanf("%d %d %d", &n, &k, &p) > 0) {
        komisje.clear();
        komisje.resize(k);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                scanf("%d", &q);
                if(q) {
                    komisje[q - 1].m[i].set(j, 1);
                }
            }
        }
        int candidates = 0;
        for(int j = 0; j < k; ++j) {
            candidates = 0;
            for(int i = 0; i < n; ++i) {
                if(komisje[j].m[i].n >= p - 1)
                    ++candidates;
            }
            if(candidates >= p) {
                break;
            }
        }
        if(candidates < p) {
            printf("Nie\n");
            continue;
        }
        bool success = false;
        for(q = 0; q < k; ++q) {
            Mask P;
            for(int i = 0; i < n; ++i) {
                P.set(i, 1);
            }
            //bh_out = 0;
            success = bh(Mask(), P);
            if(success) break;
        }
        printf(success ? "Tak" : "Nie");
        printf("\n");
    }
}
