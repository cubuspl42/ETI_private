#include <iostream>
#include <string>
#include <vector>

using namespace std;

size_t string_hash(const string &s)
{
    size_t hash = 5381;
    int c;
    const char *str = &s[0];
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

typedef pair<pair<string, string>, int> P;

struct Bucket {
    vector<P> values;
};

struct hash_map {
    vector<Bucket> buckets;
    hash_map(size_t n) {
        buckets.resize(n);
    }
    void insert(const P &p) {
        Bucket &b = at(p.first.second);
        for(int i = 0; i < b.values.size(); ++i) {
            if(b.values[i].first == p.first) {
                b.values[i].second += p.second;
                return;
            }
        }
        b.values.push_back(p);
    }
    Bucket &at(const string &surname) {
        size_t index = string_hash(surname) % buckets.size();
        return buckets[index];
    }
};

int main(int argc, const char * argv[]) {
    int n;
    cin >> n;
    hash_map h(n);
    for(int i = 0; i<n; ++i) {
        string surname, name;
        int x;
        cin >> name >> surname >> x;
        h.insert(make_pair(make_pair(name, surname), x));
    }
    int k;
    cin >> k;
    for(int i = 0; i < k; ++i) {
        string surname;
        cin >> surname;
        Bucket &b = h.at(surname);
        int sum_names = 0, sum_films = 0;
        for(int j = 0; j < b.values.size(); ++j) {
            if(b.values[j].first.second == surname) {
                sum_names++;
                sum_films += b.values[j].second;
            }
        }
        cout << sum_names << ' ' << sum_films << endl;
    }
    return 0;
}
