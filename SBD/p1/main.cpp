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
#include <cmath>

using namespace std;

const int rand_max = 16;

void nullprintf(...) {
}

#define DBG 0
#if DBG
#define dprintf printf
#else
#define dprintf nullprintf
#endif

struct Config {
    int n_rand = 0;
    string filename;
    unsigned b = 1024;
    unsigned n = 4096;
    bool print_intermediate = false;
};

Config cfg;

struct Metrics {
    bool enabled = false;
    int n_reads = 0;
    int n_writes = 0;
    int n_tapes = 0;
    int max_tapes = 0;
};

Metrics metrics;

inline long double lg(const long double x){
    return  log(x) * M_LOG2E;
}

double rw_estimate(double N, double n, double b) {
    return (double) ((N / (b * lg(n))) * lg(N / b));
}

void print_metrics() {
    cout << "Number of reads/writes: " << metrics.n_reads << "/" << metrics.n_writes << endl;
    cout << "Max. number of tapes: " << metrics.max_tapes << endl;
}

struct Record {
    int64_t a0;
    int64_t a1;
    int64_t a2;
    int64_t a3;
    int64_t a4;
    int64_t x;
};

int64_t g(Record r) {
    return
            r.a0 +
            r.a1 * r.x +
            r.a2 * r.x * r.x +
            r.a3 * r.x * r.x * r.x +
            r.a4 * r.x * r.x * r.x * r.x;
}
bool operator<(Record a, Record b) {
    int64_t y1 = g(a);
    int64_t y2 = g(b);
    return y1 < y2;
}

const Record r_min {INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN};
const Record r_max {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX};

struct Buffer {
    Record *records = nullptr;
    size_t capacity = 0;
    size_t size = 0;
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

    void read(Buffer &page) {
        page.size = fread(page.records, sizeof(Record), page.capacity, file.get());
        if(metrics.enabled) ++metrics.n_reads;
    }

    void write(const Buffer &page) {
        fwrite(page.records, sizeof(Record), page.size, file.get());
        if(metrics.enabled) ++metrics.n_writes;
    }
};

struct Reader {
    Tape &tape;
    Buffer buf;
    int p = 0;

    Reader(Tape &_tape, Buffer _buf) : tape(_tape), buf(_buf) {
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
    Buffer buf;

    Writer(Tape &tape, Buffer buf) : tape(tape), buf(buf) {
        assert(buf.capacity > 0);
        tape.reset();
    }

    void write(Record r) {
        if(buf.size >= buf.capacity) {
            assert(buf.size == buf.capacity);
            tape.write(buf);
            buf.size = 0;
        }
        buf.records[buf.size++] = r;
    }

    void close() {
        tape.write(buf);
    }
};

bool tape_sorted(Tape &t, Buffer buf) {
    metrics.enabled = false;
    Reader rd(t, buf);
    int64_t g_prev = INT64_MIN;
    while(rd.more()) {
        Record r = rd.read();
        int64_t g_r = g(r);
        if(g_r < g_prev) {
            metrics.enabled = true;
            return false;
        }
        g_prev = g_r;
    }
    metrics.enabled = true;
    return true;
}

Buffer make_big_buf(vector<Record> &opmem) {
    assert(opmem.size() == cfg.n * cfg.b);
    Buffer p;
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

void print_tape(Tape &t, Buffer buf) {
    assert(metrics.enabled);
    metrics.enabled = false;

    Reader rd(t, buf);
    cout << "[" << endl;
    while(rd.more()) {
        Record r = rd.read();
        print_record_n(r);
    }
    cout << "]" << endl;

    metrics.enabled = true;
}

Tape sort_head_inmem(vector<Record> &opmem, Tape &t) {
    dprintf("> sort_head_inmem\n");
    Buffer buf = make_big_buf(opmem);
    t.read(buf);
    sort(buf.records, buf.records + buf.size);
    Tape t2;
    t2.write(buf);

    if(cfg.print_intermediate) {
        cout << "Sorting (n * b) records in memory:" << endl;
        print_tape(t2, buf);
    }

    return t2;
}

vector<Tape> make_series(vector<Record> &opmem, Tape in_t) {
    dprintf("> make_series\n");
    in_t.reset();
    vector<Tape> series;
    while(in_t.more()) {
        series.push_back(sort_head_inmem(opmem, in_t));
    }
    return series;
}

Record min_record(vector<Reader> &readers) {
    int64_t g_rmin = INT64_MAX;
    Reader *rd_rmin = nullptr;
    for(Reader &r: readers) {
        if(r.more()) {
            Record rec = r.peek();
            if(g(rec) < g_rmin) {
                g_rmin = g(rec);
                rd_rmin = &r;
            }
        }
    }
    assert(rd_rmin && g_rmin < INT64_MAX);
    return rd_rmin->read();
}

void merge(vector<Reader> readers, Writer &writer) {
    dprintf("> merge\n");
    while(std::any_of(readers.begin(), readers.end(), [=](const Reader &r){
        return r.more();
    })) {
        Record r = min_record(readers);
        writer.write(r);
    }
}

vector<Reader> make_readers(const vector<Buffer> &pages, vector<Tape> &series,size_t m) {
    vector<Reader> readers;
    for(size_t j = 0; j < m; ++j) {
        readers.push_back(Reader(series[j], move(pages[j])));
    }
    return readers;
}

Tape merge_head(const vector<Buffer> &pages, vector<Tape> &series, size_t m) {
    dprintf("> merge_head\n");

    vector<Reader> readers = make_readers(pages, series, m);
    Tape tape;
    Writer writer(tape, pages.back());
    merge(readers, writer);

    writer.close();
    assert(tape_sorted(tape, pages.front()));

    if(cfg.print_intermediate) {
        cout << "Merging (n - 1) series:" << endl;
        print_tape(tape, pages.front());
        print_metrics();
    }

    return tape;
}

vector<Tape> merge_all(const vector<Buffer> &pages, vector<Tape> &series) {
    dprintf("> merge_all\n");
    vector<Tape> all;
    while(!series.empty()) {
        size_t m = min((size_t) cfg.n - 1, series.size());
        all.push_back(merge_head(pages, series, m));
        auto it = series.begin();
        series.erase(it, it + m);
    }
    return all;
}

Buffer make_buf(Record *records, size_t size) {
   Buffer page;
   page.records = records;
   page.capacity = size;
   return page;
}

vector<Buffer> make_pages(vector<Record> &opmem) {
    assert(opmem.size() == cfg.n * cfg.b);
    vector<Buffer> pages;
    for(unsigned i = 0; i < cfg.n; ++i) {
        Buffer page = make_buf(opmem.data() + i * cfg.b, cfg.b);
        pages.push_back(page);
    }
    return pages;
}

Tape merge_series(vector<Record> &opmem, vector<Tape> series) {
    dprintf("> merge_series\n");
    int cycle_no = 0;
    while(series.size() > 1) {
        vector<Buffer> pages = make_pages(opmem);
        series = merge_all(pages, series);

        cout << "End of cycle " << ++cycle_no << "; " << series.size() << " series" << endl;
        print_metrics();
    }
    assert(series.size() == 1);
    return move(series.front());
}

int read_input(Tape &t, istream &is, Buffer buf) {
    Writer writer(t, buf);
    int n = 0;
    while(is.good()) {
        Record r;
        read_record(r, is);
        writer.write(r);
        ++n;
    }
    writer.close();
    return n;
}

int read_file(Tape &t, string filename, Buffer buf) {
    ifstream ifs(filename);
    assert(ifs.good());
    return read_input(t, ifs, buf);
}

int rnd() {
    return rand() % rand_max;
}

int make_random_input(Tape &t, Buffer buf) {
    Writer writer(t, buf);
    for(int i = 0; i < cfg.n_rand; ++i) {
        Record r {rnd(), rnd(), rnd(), rnd(), rnd(), rnd()};
        writer.write(r);
    }
    writer.close();
    return cfg.n_rand;
}

void parse_argv(int argc, const char **argv) {
    for(int i = 0; i < argc; ++i) {
        if(string{argv[i]} == "-p") cfg.print_intermediate = true;
        if(i < argc - 1) {
            if(string{argv[i]} == "-r") cfg.n_rand = atoi(argv[i+1]);
            if(string{argv[i]} == "-f") cfg.filename = argv[i+1];
            if(string{argv[i]} == "-b") cfg.b = atoi(argv[i+1]);
            if(string{argv[i]} == "-n") cfg.n = atoi(argv[i+1]);
        }
    }
}

/**
 * Flags:
 * -r <n> -- generate <n> random records
 * -f <filename> -- read input from file <filename>
 * -b <value> -- specify records/page number
 * -n <value> -- specify number of buffers
 * -p -- print intermediate states
 */
int main(int argc, const char *argv[]) {
    parse_argv(argc, argv);

    vector<Record> opmem(cfg.n * cfg.b);
    Buffer buf0 = make_buf(opmem.data(), cfg.b);

    Tape in_t;
    int N = -1;
    if(cfg.filename.size()) {
        N = read_file(in_t, cfg.filename, buf0);
    } else if(cfg.n_rand) {
        N = make_random_input(in_t, buf0);
    } else {
        N = read_input(in_t, cin, buf0);
    }

    metrics.enabled = true;

    cout << "**** N = " << N << ", n = " << cfg.n << ", b = " << cfg.b << " ****" << endl;
    cout << "==== Input tape before sorting: ====" << endl;
    print_tape(in_t, buf0);

    cout << "==== Phase 1: in-memory sorting ====" << endl;
    vector<Tape> tapes = make_series(opmem, move(in_t));

    cout << "~~~~ Number of series: " << tapes.size() << endl;
    print_metrics();

    cout << "==== Phase 2: merging series ====" << endl;
    print_metrics();
    Tape t2 = merge_series(opmem, move(tapes));
    assert(tape_sorted(t2, buf0));

    cout << "==== Final tape: ====" << endl;
    print_tape(t2, buf0);

    cout << endl;

    double e = rw_estimate(N, cfg.n, cfg.b);
    cout << "Estimated (theoretic) number of read/writes: " << e << "/" << e << endl;
    print_metrics();

    return 0;
}
