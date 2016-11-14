#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>

using namespace std;

void nullprintf(...) {
}

#define DBG 1
#if DBG
#define dprintf printf
#else
#define dprintf nullprintf
#endif

typedef tuple<int, int, int, int, int, int> T;

struct Record {
    int a0;
    int a1;
    int a2;
    int a3;
    int a4;
    int x;

    bool operator<(const Record &o) {
        T t1 {a0, a1, a2, a3, a4, x};
        T t2 {o.a0, o.a1, o.a2, o.a3, o.a4, o.x};
        return t1 < t2;
    }
};

// const int b = 1024;
// const int n = 4096;

const int b = 4;
const int n = 4;

struct Page {
    Record *records;
    size_t capacity;
    size_t size;
};

int fpeek(FILE *stream)
{
    int c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

struct Tape {
    FILE *file;

    bool more() const {
        return fpeek(file) != EOF;
    }
};

void read_page(Tape &tape, Page &page) {
    page.size = fread(page.records, sizeof(Record), page.capacity, tape.file);
}

void write_page(Tape &tape, const Page &page) {
    fwrite(page.records, sizeof(Record), page.size, tape.file);
}

struct Reader {
    Tape tape;
    Page buf;
    int p = 0;

    Reader(Tape tape, Page buf) : tape(tape), buf(buf) {
        fseek(tape.file, 0, SEEK_SET);
        read_page(tape, buf);
    }

    bool more() const {
        return p < (int) buf.size || tape.more();
    }    

    Record read() {
        if(p < (int) buf.size) {
            return buf.records[p++];
        } else {
            read_page(tape, buf);            
            assert(buf.size > 0);
            p = 0;
            return buf.records[p++];
        }
    }

    Record peek() const {
        if(p < (int) buf.size) {
            return buf.records[p];
        } else {
            Record r;
            fread(&r, sizeof(Record), 1, tape.file); // TODO: it's not peek
            return r;
        }
    }
};

struct Writer {
    Tape tape;
    Page buf;
    int p = 0;

    Writer(Tape tape, Page buf) : tape(tape), buf(buf) {
        fseek(tape.file, 0, SEEK_SET);
    }

    void write(Record r) {
        if(p < (int) buf.size) {
            buf.records[p++] = r;
        } else {
            assert(p == (int) buf.size);
            write_page(tape, buf);
            p = 0;
            buf.records[p++] = r;
        }
    }

    Tape release() {
        write_page(tape, buf);
        return tape;
    }
};

template <typename C, typename P>
C filter(C const & container, P pred) {
    C filtered(container);
    filtered.erase(remove_if(filtered.begin(), filtered.end(), pred), filtered.end());
    return filtered;
}

template <typename C, typename F>
auto argmin(C container, F f) -> decltype(*container.begin()) {
    using T = typename C::value_type;
    auto it = min_element(container.begin(), container.end(), [=](const T& a, const T& b) {
        auto c = f(a);
        auto d = f(b);
        return c < d;        
    });
    assert(it != container.end());
    return *it;
}

Tape make_tape() {
    static int i = 0;
    string s = to_string(i++);
    string path = "/tmp/" + s; 
    dprintf("> make_tape [%s]\n", path.c_str());
    Tape t;
    t.file = fopen(path.c_str(), "w+b");
    if(!t.file)
        printf("errno: %s\n", strerror(errno));
    assert(t.file);
    return t;
}

Page make_big_buf(vector<Record> &opmem) {
    assert(opmem.size() == n * b);
    Page p;
    p.records = opmem.data();
    p.capacity = opmem.size();
    p.size = 0;
    return p;
}

void print_record(Record r) {
    cout << r.a0; //  << "." << r.a0 << "." << r.a1 << "." << r.a2 << "." << r.a3 << "." << r.a4 << "." << r.x;
}

void print_buf(Page buf) {
    cout << "[";
    for(int i = 0; i < (int) buf.size; ++i) {
        print_record(buf.records[i]);
        cout << " ";
    }
    cout << "]" << endl;
}

Tape sort_head(vector<Record> &opmem, Tape &t) {
    dprintf("> sort_head\n");
    Page buf = make_big_buf(opmem);
    read_page(t, buf);
    print_buf(buf);
    sort(buf.records, buf.records + buf.size);
    print_buf(buf);
    Tape t2 = make_tape();
    write_page(t2, buf);
    return t2;
}

vector<Tape> make_series(vector<Record> &opmem, Tape &t) {
    dprintf("> make_series\n");
    vector<Tape> series;
    while(t.more()) {
        series.push_back(sort_head(opmem, t));
    }
    return series;
}

void merge(vector<Reader> readers, Writer &writer) {
    dprintf("> merge\n");
    while(!readers.empty()) {
        readers = filter(readers, [=](const Reader &r) {
            return !r.more();
        });
        if(!readers.empty()) {
            Reader &rm = argmin(readers, [=](const Reader &r) {
                return r.peek();
            });
            print_record(rm.peek());
            cout << endl;
            writer.write(rm.read());
        }
    }
}

vector<Reader> make_readers(const vector<Page> &pages, const vector<Tape> &series, size_t i, size_t m) {
    vector<Reader> readers;
    for(size_t j = 0; j < m; ++j) {
        readers.push_back(Reader(series[i + j], pages[j]));
    }
    return readers;
}

Tape merge_head(const vector<Page> &pages, const vector<Tape> &series, size_t i, size_t m) {
    dprintf("> merge_head\n");
    vector<Reader> readers = make_readers(pages, series, i, m);
    Tape tape = make_tape();
    Writer writer(tape, pages.back());
    merge(readers, writer);
    return writer.release();
}

vector<Tape> merge_all(const vector<Page> &pages, const vector<Tape> &series) {
    dprintf("> merge_all\n");
    vector<Tape> all;
    for(size_t i = 0; i < series.size(); i += (n - 1)) {
        size_t m = min((size_t) n - 1, series.size() - i);
        all.push_back(merge_head(pages, series, i, m));
    }
    return all;
}

Page make_buf(Record *records, size_t size) {
   Page page;
   page.records = records;
   page.capacity = page.size = size;
   return page;
}

vector<Page> make_pages(vector<Record> &opmem) {
    assert(opmem.size() == n * b);
    vector<Page> pages;
    for(int i = 0; i < n; ++i) {
        Page page = make_buf(opmem.data() + i * b, b);
        pages.push_back(page);
    }
    return pages;
}

Tape merge_series(vector<Record> &opmem, vector<Tape> series) {
    dprintf("> merge_series\n");
    while(series.size() > 1) {
        vector<Page> pages = make_pages(opmem);
        series = merge_all(pages, move(series));
    }
    assert(series.size() == 1);
    return move(series.front());
}

void print_tape(Tape &t, Page buf) {
    Reader rd(t, buf);
    cout << "/";
    while(rd.more()) {
        Record r = rd.read();
        print_record(r);
        cout << " ";
    }
    cout << "/" << endl;
}

int main(int argc, const char *argv[]) {
    Tape t;
    t.file = fopen("/tmp/t0", "rb");

    vector<Record> opmem(n * b);
    Page buf0 = make_buf(opmem.data(), b);
    vector<Tape> tapes = make_series(opmem, t);
    for(Tape &t : tapes) print_tape(t, buf0);
    Tape t2 = merge_series(opmem, move(tapes));
    print_tape(t2, buf0);

    return 0;
}
