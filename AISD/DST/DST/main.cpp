#include <iostream>

template<typename T1, typename T2>
struct pair {
    inline pair(const T1 &lhs, const T2 &rhs) : first(lhs), second(rhs) {};
    T1 first;
    T2 second;
};

template<typename T1, typename T2>
pair<T1, T2> make_pair(const T1 &lhs, const T2 &rhs) {
    return pair<T1, T2>(lhs, rhs);
}

template<typename mapped_type>
class trie {
public:
    typedef pair<size_t, mapped_type> value_type;
private:
    struct Node {
        Node(const value_type &val, size_t num_children) : _val(val) {
            _children = new Node*[num_children];
            for(unsigned i = 0; i < num_children; ++i) {
                _children[i] = nullptr;
            }
        }
        
        ~Node() {
            delete[] _children;
        }
        
        value_type _val;
        Node **_children;
    };
    
    Node *_root;
    size_t _n, _k;
public:
    class iterator {
    public:
        inline iterator(Node **node_array, size_t node_index) : _node_array(node_array), _node_index(node_index) {}
        bool operator==(const iterator &other) {
            return _node_array == other._node_array && _node_index == other._node_index;
        }
    private:
        Node **_node_array;
        size_t _node_index;
        friend class trie;
    };
    
private:
    pair<iterator, bool> insert(Node *node, size_t remainder, size_t quotient, const value_type &val) {
        if(node->_children[remainder]) {
            Node *child = node->_children[remainder];
            if(child->_val.first == val.first) {
                return make_pair(iterator(node->_children, remainder), false);
            } else {
                return insert(child, quotient % _k, quotient / _k, val);
            }
        } else {
            node->_children[remainder] = new Node(val, _k);
            return make_pair(iterator(node->_children, remainder), true);
        }
    }
    
    iterator find(Node *node, size_t remainder, size_t quotient, size_t key) {
        Node *child = node->_children[remainder];
        if(child) {
            if(child->_val.first == key) {
                return iterator(node->_children, remainder);
            } else {
                return find(child, quotient % _k, quotient / _k, key);
            }
        } else {
            return end();
        }
    }
    
    void destroy_leaf(Node *&node, value_type &orig_val) {
        for(unsigned i = 0; i < _k; ++i) {
            if(node->_children[i]) {
                destroy_leaf(node->_children[i], orig_val);
                return;
            }
        }
        orig_val = node->_val;
        delete node;
        node = nullptr;
    }
    
    template<class F>
    void transverse(Node *node, const F &f) {
        if(node) {
            f(node->_val);
            for(unsigned i = 0; i < _k; ++i) {
                transverse(node->_children[i], f);
            }
        }
    }
    
public:
    inline trie(size_t n, size_t k) : _root(nullptr), _n(n), _k(k) {}
    
    ~trie() {
        delete _root;
    }
    
    iterator begin() {
        return iterator(&_root, 0);
    }
    
    iterator end() {
        return iterator(nullptr, 0);
    }
    
    pair<iterator, bool> insert(const value_type &val) {
        if(!_root) {
            _root = new Node(val, _n);
            return make_pair(begin(), true);
        } else {
            size_t remainder = val.first % _n, quotient = val.first / _n;
            return insert(_root, remainder, quotient, val);
        }
    }
    
    iterator find(size_t key) {
        if(!_root) {
            return end();
        } else if(_root->_val.first == key) {
            return begin();
        } else {
            size_t remainder = key % _n, quotient = key / _n;
            return find(_root, remainder, quotient, key);
        }
    }
    
    size_t erase(size_t key) {
        iterator it = find(key);
        if(it == end()) {
            return 0;
        } else {
            Node *&node = it._node_array[it._node_index];
            for(unsigned i = 0; i < (node == _root ? _n : _k); ++i) {
                if(node->_children[i]) {
                    destroy_leaf(node->_children[i], node->_val);
                    return 1;
                }
            }
            delete node;
            node = nullptr;
            return 1;
        }
    }
    
    template<class F>
    void transverse(const F &f) {
        if(_root) {
            f(_root->_val);
            for(unsigned i = 0; i < _n; ++i) {
                transverse(_root->_children[i], f);
            }
        }
    }
};

struct Empty {};
typedef trie<Empty> Trie;

void print_element(const Trie::value_type &val) {
    std::cout << val.first << " ";
}

int main(int argc, const char * argv[]) {
    int t;
    std::cin >> t;
    int minimum, maximum;
    std::cin >> minimum >> maximum;
    int n, k;
    std::cin >> n >> k;
    Trie _trie(n, k);
    
    for(int i = 0; i < t; ++i) {
        char command;
        std::cin >> command;
        int x = -1;
        if(command != 'P') {
            std::cin >> x;
        }
        switch (command) {
            case 'I': {
                pair<Trie::iterator, bool> p = _trie.insert(make_pair((size_t)x, Empty()));
                if(!p.second) {
                    std::cout << x << " exist" << std::endl;
                }
            }
                break;
            case 'L': {
                Trie::iterator it = _trie.find(x);
                if(it == _trie.end()) {
                    std::cout << x << " not exist" << std::endl;
                } else {
                    std::cout << x << " exist" << std::endl;
                }
            }
                break;
            case 'D': {
                if(!_trie.erase(x)) {
                    std::cout << x << " not exist" << std::endl;
                }
            }
                break;
            case 'P': {
                _trie.transverse(print_element);
                std::cout << std::endl;
            }
                break;
            default:
                break;
        }
        
    }
    
    return 0;
}
