#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <climits>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <fstream>

using namespace std;

void nullprintf(...) {
}

#define DBG 1
#if DBG
#define dprintf printf
#else
#define dprintf nullprintf
#endif

struct Metrics {
    int n_reads = 0;
    int n_writes = 0;
    int n_tapes = 0;
    int max_tapes = 0;
};

Metrics metrics;

struct Record {
    int a0;
    int a1;
    int a2;
    int a3;
    int a4;
    int x;
};

int64_t g(Record r) {
    return
            r.a0 +
            r.a1 * r.x +
            r.a2 * r.x * r.x +
            r.a3 * r.x * r.x * r.x * r.x +
            r.a4 * r.x * r.x * r.x * r.x * r.x;
}

bool operator<(Record a, Record b) {
    int64_t y1 = g(a);
    int64_t y2 = g(b);
    return y1 < y2;
}

// const int b = 1024;
// const int n = 4096;

const int b = 4;
const int n = 4;

const Record r_max {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX};

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

FILE *file_open() {
    ++metrics.n_tapes;
    metrics.max_tapes = max(metrics.max_tapes, metrics.n_tapes);
    return tmpfile();
}

void file_close(FILE *file) {
    --metrics.n_tapes;
    (void) fclose(file);
}

class Tape {
    unique_ptr<FILE, decltype(&file_close)> file { file_open(), file_close };
public:
    bool more() const {
        return fpeek(file.get()) != EOF;
    }

    void reset() {
        fseek(file.get(), 0, SEEK_SET);
    }

    void read(Page &page) {
        page.size = fread(page.records, sizeof(Record), page.capacity, file.get());
        ++metrics.n_reads;
    }

    void write(const Page &page) {
        fwrite(page.records, sizeof(Record), page.size, file.get());
        ++metrics.n_writes;
    }
};

struct Reader {
    Tape &tape;
    Page buf;
    int p = 0;

    Reader(Tape &tape, Page buf) : tape(tape), buf(buf) {
        tape.reset();
        tape.read(buf);
    }

    bool more() const {
        return p < (int) buf.size || tape.more();
    }

    void _read_page() {
        assert(more());
        assert(p <= (int) buf.size);
        if(p >= (int) buf.size) {
            tape.read(buf);
            assert(buf.size > 0);
            p = 0;
        }
    }

    Record read() {
        _read_page();
        return buf.records[p++];
    }

    Record peek() {
        _read_page();
        return buf.records[p];
    }
};

struct Writer {
    Tape &tape;
    Page buf;
    int p = 0;

    Writer(Tape &tape, Page buf) : tape(tape), buf(buf) {
        tape.reset();
    }

    void write(Record r) {
        if(p < (int) buf.size) {
            buf.records[p++] = r;
        } else {
            assert(p == (int) buf.size);
            tape.write(buf);
            p = 0;
            buf.records[p++] = r;
        }
    }

    void close() {
        tape.write(buf);
    }
};

bool tape_sorted(Tape &t, Page buf) {
    Reader rd(t, buf);
    Record prev = rd.peek();
    while(rd.more()) {
        if(rd.read() < prev) {
            return false;
        }
    }
    return true;
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
    cout << "("
         << r.a0 << "," << r.a1 << "," << r.a2 << "," << r.a3 << "," << r.a4 << "," << r.x
         << "); g(r) = " << g(r);
}

void read_record(Record &r, istream &is) {
    is >> r.a0 >> r.a1 >> r.a2 >> r.a3 >> r.a4 >> r.x;
}

void print_record_n(Record r) {
    print_record(r);
    cout << endl;
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
    t.read(buf);
    print_buf(buf);
    sort(buf.records, buf.records + buf.size);
    print_buf(buf);
    Tape t2;
    t2.write(buf);
    return t2;
}

vector<Tape> make_series(vector<Record> &opmem, Tape &t) {
    dprintf("> make_series\n");
    t.reset();
    vector<Tape> series;
    while(t.more()) {
        series.push_back(sort_head(opmem, t));
    }
    return series;
}

Reader &min_reader(vector<Reader> &readers) {
    dprintf("> min_reader\n");
    Reader *rm = NULL;
    Record recm = r_max;
    for(Reader &r: readers) {
        if(r.more()) {
            Record rec = r.peek();
            print_record_n(rec);
            if(rec < recm) {
                rm = &r;
                recm = rec;
            }
        }
    }
    assert(rm);
    dprintf("< min_reader\n");
    return *rm;
}

void merge(vector<Reader> readers, Writer &writer) {
    dprintf("> merge\n");
    while(std::any_of(readers.begin(), readers.end(), [=](const Reader &r){
        return r.more();
    })) {
        Reader &rm = min_reader(readers);
        print_record_n(rm.peek());
        writer.write(rm.read());
    }
}

vector<Reader> make_readers(const vector<Page> &pages, vector<Tape> &series, size_t i, size_t m) {
    vector<Reader> readers;
    for(size_t j = 0; j < m; ++j) {
        readers.push_back(Reader(series[i + j], move(pages[j])));
    }
    return readers;
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

Tape merge_head(const vector<Page> &pages, vector<Tape> &series, size_t i, size_t m) {
    dprintf("> merge_head\n");
    for(Tape &t : series) {
        print_tape(t, pages.front());
        assert(tape_sorted(t, pages.front()));
    }
    vector<Reader> readers = make_readers(pages, series, i, m);
    Tape tape;
    Writer writer(tape, pages.back());
    merge(readers, writer);

    writer.close();
    assert(tape_sorted(tape, pages.front()));

    return tape;
}

vector<Tape> merge_all(const vector<Page> &pages, vector<Tape> &series) {
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
        series = merge_all(pages, series);
    }
    assert(series.size() == 1);
    return move(series.front());
}

void read_input(Tape &t, istream &is, Page buf) {
    Writer writer(t, buf);
    while(is.good()) {
        Record r;
        read_record(r, is);
        writer.write(r);
    }
    writer.close();
}

void read_stdin(Tape &t, istream &is, Page buf) {
    read_input(t, cin, buf);
}

void read_file(Tape &t, const char *filename, Page buf) {
    ifstream ifs(filename);
    assert(ifs.good());
    read_input(t, cin, buf);
}

void make_random_input(Tape &t, Page buf) {
    Writer writer(t, buf);
    const int n_rand = 128;
    for(int i = 0; i < n_rand; ++i) {
        Record r {rand(), rand(), rand(), rand(), rand(), rand()};
        writer.write(r);
    }
    writer.close();
}

int main(int argc, const char *argv[]) {
    vector<Record> opmem(n * b);
    Page buf0 = make_buf(opmem.data(), b);

    Tape t;
//    make_random_input(t, buf0);
    read_input(t, cin, buf0);

    print_tape(t, buf0);

    vector<Tape> tapes = make_series(opmem, t);
    for(Tape &tt : tapes) {
        print_tape(tt, buf0);
        assert(tape_sorted(tt, buf0));
    }
    Tape t2 = merge_series(opmem, move(tapes));
    print_tape(t2, buf0);
    assert(tape_sorted(t2, buf0));

    cout << "Number of reads/writes: " << metrics.n_reads << "/" << metrics.n_writes << endl;
    cout << "Max. number of tapes: " << metrics.n_tapes << endl;

    return 0;
}
