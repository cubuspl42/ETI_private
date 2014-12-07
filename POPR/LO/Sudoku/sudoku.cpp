#include "sudoku.h"
#include "algorithm.h"
#include "xml.h"
#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <fstream>

static void square_coords(unsigned y, unsigned x, unsigned *square_y, unsigned *square_x) {
    *square_y = y/base_number*base_number;
    *square_x = x/base_number*base_number;
}

void reset_sudoku(Sudoku &sudoku) {
    sudoku.undo_stack.clear();
    sudoku.last_action = sudoku.undo_stack.end();
    sudoku.num_actions = 0;
    sudoku.flags = 0;
    sudoku.active_state_index = 0;
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            sudoku.board[i][j] = sudoku.comments[i][j] = 0;
        }
    }
    for(int i=0; i<base_number_sq; ++i) {
        sudoku.row_numbers_masks[i] = sudoku.column_numbers_masks[i] = 0;
    }
    for(int i=0; i<base_number; ++i) {
        for(int j=0; j<base_number; ++j) {
            sudoku.square_numbers_masks[i][j] = 0;
        }
    }
}

bool creates_conflict(const Sudoku &sudoku, unsigned y, unsigned x, unsigned number,
                      unsigned *conflict_y, unsigned *conflict_x) {
    int square_y = y/base_number*base_number;
    int square_x = x/base_number*base_number;
    for(int i = square_y; i < square_y+base_number; ++i) {
        for(int j = square_x; j < square_x+base_number; ++j) {
            if(y == i && x == j)
                continue;
            if(sudoku.board[i][j] == number) {
                *conflict_y = i;
                *conflict_x = j;
                return true;
            }
        }
    }
    for(int i=0; i<base_number_sq; ++i) {
        if(y == i)
            continue;
        if(sudoku.board[i][x] == number) {
            *conflict_y = i;
            *conflict_x = x;
            return true;
        }
    }
    for(int j=0; j<base_number_sq; ++j) {
        if(x == j)
            continue;
        if(sudoku.board[y][j] == number) {
            *conflict_y = y;
            *conflict_x = j;
            return true;
        }
    }
    *conflict_y = *conflict_x = -1;
    return false;
}

unsigned possible_numbers(const Sudoku &sudoku, unsigned y, unsigned x) {
    unsigned sq_i = y/base_number, sq_j=x/base_number;
    return  ~sudoku.row_numbers_masks[y] &
            ~sudoku.column_numbers_masks[x] &
            ~sudoku.square_numbers_masks[sq_i][sq_j] &
            base_mask;
}

static unsigned naked_single(const Sudoku &sudoku, const unsigned forbidden_numbers_masks[base_number_sq][base_number_sq],
                             unsigned *hint_y, unsigned *hint_x) {
    for(int i = 0; i < base_number_sq; ++i) {
        for(int j = 0; j < base_number_sq; ++j) {
            if(sudoku.board[i][j])
                continue;
            unsigned allowed_numbers_mask = ~forbidden_numbers_masks[i][j] & base_mask;
            if(has_single_bit(allowed_numbers_mask)) {
                *hint_y = i;
                *hint_x = j;
                return lsb_bit_index(allowed_numbers_mask)+1;
            }
        }
    }
    return 0;
}

static unsigned hidden_single(const Sudoku &sudoku, const unsigned forbidden_numbers_masks[base_number_sq][base_number_sq],
                              unsigned *hint_y, unsigned *hint_x) {
    for(int sq_i = 0; sq_i < base_number; ++sq_i) {
        for(int sq_j = 0; sq_j < base_number; ++sq_j) { // dla każdego kwadratu 3x3 (sq_i, sq_j)
            unsigned sq_y=sq_i*base_number, sq_x=sq_j*base_number;
            for(int k = 0; k < base_number_sq; ++k) {
                int num_candidates = 0;
                unsigned y, x;
                for(int i = sq_y; i < sq_y+base_number; ++i) {
                    for(int j = sq_x; j < sq_x+base_number; ++j) {
                        if(sudoku.board[i][j])
                            continue;
                        if(check_bit(~forbidden_numbers_masks[i][j], k)) {
                            ++num_candidates;
                            y = i, x = j;
                        }
                    }
                }
                if(num_candidates == 1) {
                    *hint_y = y, *hint_x = x;
                    fprintf(stderr, "hidden_single: square [%d, %d]\n", sq_i, sq_j);
                    return k+1;
                }
            }
        }
    }
    struct {
        unsigned num_candidates = 0;
        unsigned index = 0;
    } row_data[base_number_sq][base_number_sq], column_data[base_number_sq][base_number_sq];
    for(int i = 0; i < base_number_sq; ++i) {
        for(int j = 0; j < base_number_sq; ++j) {
            if(sudoku.board[i][j])
                continue;
            for(int k = 0; k < base_number_sq; ++k) {
                if(check_bit(~forbidden_numbers_masks[i][j], k)) {
                    ++row_data[k][i].num_candidates;
                    ++column_data[k][j].num_candidates;
                    row_data[k][i].index = j;
                    column_data[k][j].index = i;
                }
            }
        }
    }
    for(int k = 0; k < base_number_sq; ++k) {
        for(int i = 0; i < base_number_sq; ++i) {
            if(row_data[k][i].num_candidates == 1) {
                *hint_y = i, *hint_x = row_data[k][i].index;
                fprintf(stderr, "hidden_single: row %d\n", i);
                return k+1;
            }
        }
        for(int j = 0; j < base_number_sq; ++j) {
            if(column_data[k][j].num_candidates == 1) {
                *hint_y = column_data[k][j].index, *hint_x = j;
                fprintf(stderr, "hidden_single: column %d\n", j);
                return k+1;
            }
        }
    }
    return 0;
}

static unsigned pointing_pairs(const Sudoku &sudoku, unsigned forbidden_numbers_masks[base_number_sq][base_number_sq]) {
    unsigned num_pointing_pairs = 0; // tak na prawdę, mogą to być również "pointing triples"...
    for(int sq_i = 0; sq_i < base_number; ++sq_i) {
        for(int sq_j = 0; sq_j < base_number; ++sq_j) {
            fprintf(stderr, "square [%d, %d]\n", sq_i, sq_j);
            unsigned sq_y=sq_i*base_number, sq_x=sq_j*base_number;
            for(int k = 0; k < base_number_sq; ++k) {
                unsigned row_mask = 0, column_mask = 0;
                for(int i = sq_y; i < sq_y+base_number; ++i) {
                    for(int j = sq_x; j < sq_x+base_number; ++j) {
                        if(sudoku.board[i][j])
                            continue;
                        if(check_bit(~forbidden_numbers_masks[i][j], k)) {
                            set_bit(&row_mask, i);
                            set_bit(&column_mask, j);
                        }
                    }
                }
                if(has_single_bit(row_mask)) {
                    ++num_pointing_pairs;
                    unsigned i = lsb_bit_index(row_mask);
                    fprintf(stderr, "pointing_pair: %d (row %d)\n", k+1, i);
                    for(int j = 0; j < base_number_sq; ++j) {
                        if(j < sq_x || j >= sq_x+base_number)
                            set_bit(&forbidden_numbers_masks[i][j], k);
                    }
                }
                if(has_single_bit(column_mask)) {
                    ++num_pointing_pairs;
                    unsigned j = lsb_bit_index(column_mask);
                    fprintf(stderr, "pointing_pair: %d (column %d)\n", k+1, j);
                    for(int i = 0; i < base_number_sq; ++i) {
                        if(i < sq_y || i >= sq_y+base_number)
                            set_bit(&forbidden_numbers_masks[i][j], k);
                    }
                }
            }
        }
    }
    return num_pointing_pairs;
}

static const unsigned max_brute_force_iterations = 2*1024*1024;

bool brute_force(Sudoku &sudoku, unsigned *num_iterations, unsigned *hint, unsigned *hint_y, unsigned *hint_x) {
    if(*num_iterations > max_brute_force_iterations)
        return false;
    ++*num_iterations;
    int y = -1, x = -1;
    for(int i = 0; i < base_number_sq && y < 0; ++i) {
        for(int j = 0; j < base_number_sq && y < 0; ++j) {
            if(!sudoku.board[i][j])
                y = i, x = j;
        }
    }
    if(y < 0) {
        return true;
    }
    unsigned mask = possible_numbers(sudoku, y, x), tmp;
    for(int k = 0; k < base_number_sq; ++k) {
        if(check_bit(mask, k) && !creates_conflict(sudoku, y, x, k+1, &tmp, &tmp)) {
            unsigned old = sudoku.board[y][x];
            sudoku.board[y][x] = k+1;
            bool success = brute_force(sudoku, num_iterations, hint, hint_y, hint_x);
            sudoku.board[y][x] = old;
            if(success) {
                *hint_y = y;
                *hint_x = x;
                *hint = k+1;
                return true;
            }
        }
    }
    return false;
}

unsigned hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x) {
    fprintf(stderr, "hint begin\n\n");
    unsigned forbidden_numbers_masks[base_number_sq][base_number_sq] = {{0}};
    
    // na wstępie: zaznaczamy wpływ już wypełnionych pól
    for(int i = 0; i < base_number_sq; ++i) {
        for(int j = 0; j < base_number_sq; ++j) {
            unsigned n = sudoku.board[i][j];
            if(n) {
                unsigned square_y, square_x;
                square_coords(i, j, &square_y, &square_x);
                for(int x = 0; x < base_number_sq; ++x)
                    set_bit(&forbidden_numbers_masks[i][x], n-1);
                for(int y = 0; y < base_number_sq; ++y)
                    set_bit(&forbidden_numbers_masks[y][j], n-1);
                for(int y = square_y; y < square_y+base_number; ++y)
                    for(int x = square_x; x < square_x+base_number; ++x)
                        set_bit(&forbidden_numbers_masks[y][x], n-1);
            }
        }
    }
    
    // główna pętla, która trwa tak długo jak znajdujemy nowe "pointing pairs"
    unsigned hint = 0;
    unsigned num_pointing_pairs = 0;
    bool progress = true;
    while(progress) {
        fprintf(stderr, "...\n");
        progress = false;
        hint = naked_single(sudoku, forbidden_numbers_masks, hint_y, hint_x);
        if(hint) {
            fprintf(stderr, "naked_single: %d (%d, %d)\n\n", hint, *hint_y, *hint_x);
            return hint;
        }
        hint = hidden_single(sudoku, forbidden_numbers_masks, hint_y, hint_x);
        if(hint) {
            fprintf(stderr, "hidden_single: %d (%d, %d)\n\n", hint, *hint_y, *hint_x);
            return hint;
        }
        unsigned new_num_pointing_pairs = pointing_pairs(sudoku, forbidden_numbers_masks);
        fprintf(stderr, "num_pointing_pairs: %d -> %d\n", num_pointing_pairs, new_num_pointing_pairs);
        if(new_num_pointing_pairs > num_pointing_pairs) {
            progress = true;
            num_pointing_pairs = new_num_pointing_pairs;
        }
    }
    
    unsigned num_iterations = 0;
    using namespace std::chrono;
    high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    bool success = brute_force(sudoku, &num_iterations, &hint, hint_y, hint_x);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    fprintf(stderr, "brute_forced: %d (num_iterations: %d, time: %d ms)\n\n", hint, num_iterations, (unsigned)ms);
    if(success) {
        if(num_iterations > 1)
            return hint;
    }
    
    fprintf(stderr, "hint failed\n\n");
    return 0;
}

void give_hint(Sudoku &sudoku) {
    unsigned hint_y = 0, hint_x = 0;
    sudoku.hint_number = hint(sudoku, &hint_y, &hint_x);
    if(sudoku.hint_number) {
        set_bit(&sudoku.flags, HINT_BIT);
        sudoku.hint_y = hint_y;
        sudoku.hint_x = hint_x;
    }
}

void perform_action(Sudoku &sudoku, const Action &action, unsigned board_new_number) {
    if(action.is_comment) {
        toggle_bit(&sudoku.comments[action.y][action.x], action.comment.flipped_number - 1);
    } else {
        clear_bit(&sudoku.flags, CONFLICT_BIT);
        clear_bit(&sudoku.flags, POSSIBILITIES_BIT);
        clear_bit(&sudoku.flags, HINT_BIT);
        clear_bit(&sudoku.flags, HIGHLIGHT_BIT);
        unsigned y = action.y, x = action.x;
        unsigned sq_y = y/base_number, sq_x = x/base_number;
        auto &board_current_number = sudoku.board[y][x];
        if(board_current_number) {
            clear_bit(&sudoku.row_numbers_masks[y], board_current_number-1);
            clear_bit(&sudoku.column_numbers_masks[x], board_current_number-1);
            clear_bit(&sudoku.square_numbers_masks[sq_y][sq_x], board_current_number-1);
        }
        if(board_new_number) {
            set_bit(&sudoku.row_numbers_masks[y], board_new_number-1);
            set_bit(&sudoku.column_numbers_masks[x], board_new_number-1);
            set_bit(&sudoku.square_numbers_masks[sq_y][sq_x], board_new_number-1);
        }
        board_current_number = board_new_number;
        ++sudoku.num_actions;
    }
}

void do_action(Sudoku &sudoku, const Action &action) {
    while(sudoku.last_action != sudoku.undo_stack.begin()) {
        sudoku.undo_stack.pop_front();
    }
    sudoku.undo_stack.push_front(action);
    sudoku.last_action = sudoku.undo_stack.begin();
    perform_action(sudoku, action, action.board.number_after);
    ++sudoku.active_state_index;
}

bool redo(Sudoku &sudoku) {
    if(sudoku.last_action == sudoku.undo_stack.begin())
        return false;
    Action &action = *(--sudoku.last_action);
    perform_action(sudoku, action, action.board.number_after);
    ++sudoku.active_state_index;
    return true;
}

bool undo(Sudoku &sudoku) {
    if(sudoku.last_action == sudoku.undo_stack.end())
        return false;
    Action &action = *sudoku.last_action;
    ++sudoku.last_action;
    perform_action(sudoku, action, action.board.number_before);
    --sudoku.active_state_index;
    return true;
}

void put_number(Sudoku &sudoku, unsigned y, unsigned x, unsigned number) {
    assert(number >= 0 && number <= base_number_sq);
    if(sudoku.board[y][x] == number)
        return;
    unsigned conflict_y = 0, conflict_x = 0;
    if(number && creates_conflict(sudoku, y, x, number, &conflict_y, &conflict_x)) {
        set_bit(&sudoku.flags, CONFLICT_BIT);
        sudoku.conflict_y = conflict_y;
        sudoku.conflict_x = conflict_x;
        return;
    }
    Action action;
    action.y = y, action.x = x;
    action.board.number_before = sudoku.board[y][x];
    action.board.number_after = number;
    do_action(sudoku, action);
}

void accept_hint(Sudoku &sudoku) {
    if(check_bit(sudoku.flags, HINT_BIT)) {
        put_number(sudoku, sudoku.hint_y, sudoku.hint_x, sudoku.hint_number);
    }
}

void reject_hint(Sudoku &sudoku) {
    clear_bit(&sudoku.flags, HINT_BIT);
}

void highlight_number(Sudoku &sudoku, unsigned y, unsigned x) {
    unsigned n = sudoku.board[y][x];
    if(n) {
        set_bit(&sudoku.flags, HIGHLIGHT_BIT);
        sudoku.highlighted_number = n;
    }
}

void list_possibilities(Sudoku &sudoku, unsigned y, unsigned x) {
    set_bit(&sudoku.flags, POSSIBILITIES_BIT);
    sudoku.possibilities = possible_numbers(sudoku, y, x);
}

void flip_comment(Sudoku &sudoku, unsigned y, unsigned x, unsigned number) {
    assert(number >= 1 && number <= base_number_sq);
    Action action;
    action.y = y, action.x = x, action.is_comment = true;
    action.comment.flipped_number = number;
    do_action(sudoku, action);
}

void move_pointer(Sudoku &sudoku, int dy, int dx) {
    int py = sudoku.pointer_y;
    int px = sudoku.pointer_x;
    py += dy, px += dx;
    py = clamp(py, 0, base_number_sq-1);
    px = clamp(px, 0, base_number_sq-1);
    if(sudoku.pointer_y != py || sudoku.pointer_x != px) {
        sudoku.pointer_y = py;
        sudoku.pointer_x = px;
        clear_bit(&sudoku.flags, HIGHLIGHT_BIT);
        clear_bit(&sudoku.flags, CONFLICT_BIT);
        if(!dev_mode)
            clear_bit(&sudoku.flags, POSSIBILITIES_BIT);
        else list_possibilities(sudoku, sudoku.pointer_y, sudoku.pointer_x);
    }
}

static unsigned parse_number_sequence(std::istream& is, int numbers[], unsigned max_num_numbers)
{
    unsigned n = max_num_numbers;
    for(int i = 0; i < max_num_numbers; ++i) {
        skip_whitespace(is);
        char c = is.get();
        if(c == '-') {
            numbers[i] = 0;
            skip_whitespace(is);
        } else if(c >= '0' && c <= '9') {
            numbers[i] = c-'0';
            skip_whitespace(is);
        } else {
            n = i;
            is.putback(c);
            break;
        }
    }
    return n;
}

static void try_load_comments(State &state, AttrMap &attrs, std::ifstream &is) {
    attrs.clear();
    if(try_parse_start_tag(is, "comments", attrs)) {
        while(try_parse_start_tag(is, "comment", attrs)) {
            int i = atoi(attrs["row"].c_str()) - 1;
            int j = atoi(attrs["col"].c_str()) - 1;
            attrs.clear();
            int comment_numbers[base_number_sq];
            unsigned n = parse_number_sequence(is, comment_numbers, base_number_sq);
            for(unsigned k = 0; k < n; ++k) {
                unsigned c = comment_numbers[k];
                if(c)
                    set_bit(&state.comments[i][j], c-1);
            }
            parse_end_tag(is, "comment");
        }
        parse_end_tag(is, "comments");
    }
}

void load_xml_file(Sudoku &sudoku, const char *filename)
{
    ktl::list<State> states;
    std::ifstream is(filename);
    if(!is.good())
        return;
    reset_sudoku(sudoku);
    
    expect_valid_declaration(is);
    AttrMap attrs;
    parse_start_tag(is, "sudoku", attrs);
    int active_state = atoi(attrs["active-state"].c_str());
    attrs.clear();
    while(try_parse_start_tag(is, "state", attrs)) {
        State state;
        state.nr = atoi(attrs["nr"].c_str());
        
        try_load_comments(state, attrs, is);
        
        attrs.clear();
        parse_start_tag(is, "board", attrs);
        parse_number_sequence(is, state.board_numbers, base_number_sq*base_number_sq);
        parse_end_tag(is, "board");
        
        try_load_comments(state, attrs, is);
        
        parse_end_tag(is, "state");
        states.push_back(state);
    }
    parse_end_tag(is, "sudoku");

    sort(states.begin(), states.end(), [](const State &s1, const State &s2){
        return s1.nr < s2.nr;
    });
    
    auto active_action = sudoku.undo_stack.end();
    for(const State &state : states) {
        for(int k = 0; k < base_number_sq*base_number_sq; ++k) {
            int i = k/base_number_sq, j = k%base_number_sq;
            put_number(sudoku, i, j, state.board_numbers[k]);
        }
        for(int i = 0; i < base_number_sq; ++i) {
            for(int j = 0; j < base_number_sq; ++j) {
                unsigned xor_mask = sudoku.comments[i][j] ^ state.comments[i][j];
                for(int k = 0; k < base_number_sq; ++k) {
                    if(check_bit(xor_mask, k)) {
                        flip_comment(sudoku, i, j, k+1);
                    }
                }
            }
        }
        if(state.nr == active_state)
            active_action = sudoku.undo_stack.begin();
    }
    if(active_action != sudoku.undo_stack.end()) {
        while(sudoku.last_action != active_action)
            undo(sudoku);
    }
}

static void write_state(std::ostream &os, uint8_t board[base_number_sq][base_number_sq],
                        uint16_t comments[base_number_sq][base_number_sq], unsigned state_nr) {
    write_indentation(os, 1);
    os << "<state nr =\"" << state_nr << "\">\n";
    write_indentation(os, 2);
    os << "<board>\n";
    for(int i = 0; i < base_number_sq; ++i) {
        write_indentation(os, 3);
        for(int j = 0; j < base_number_sq; ++j) {
            unsigned n = board[i][j];
            if(n)
                os << n << ' ';
            else os << "- ";
        }
        os << "\n";
    }
    write_indentation(os, 2);
    os << "</board>\n";
    unsigned has_comments = 0;
    for(int i = 0; i < base_number_sq; ++i)
        for(int j = 0; j < base_number_sq; ++j)
            has_comments += comments[i][j];
    if((bool)has_comments) {
        write_indentation(os, 2);
        os << "<comments>\n";
        for(int i = 0; i < base_number_sq; ++i) {
            for(int j = 0; j < base_number_sq; ++j) {
                unsigned c = comments[i][j];
                if(c) {
                    write_indentation(os, 3);
                    os << "<comment row=\"" << i+1 << "\" col=\"" << j+1 << "\">";
                    for(int k = 0; k < base_number_sq; ++k) {
                        if(check_bit(c, k)) {
                            os << k+1 << ' ';
                        }
                    }
                    os << "</comment>\n";
                }
            }
        }
        write_indentation(os, 2);
        os << "</comments>\n";
    }
    write_indentation(os, 1);
    os << "</state>\n";
}

void save_xml_file(Sudoku &sudoku, const char *filename)
{
    std::ofstream os(filename);
    write_declaration(os);
    os << "<sudoku active-state=\"" << std::max(1u, sudoku.active_state_index) << "\">\n";
    unsigned state_nr = 0;
    auto iter = sudoku.undo_stack.end();
    uint8_t board[base_number_sq][base_number_sq] = {{0}};
    uint16_t comments[base_number_sq][base_number_sq] = {{0}};
    do {
        --iter;
        Action &action = *iter;
        if(action.is_comment) {
            toggle_bit(&comments[action.y][action.x], action.comment.flipped_number - 1);
        } else {
            board[action.y][action.x] = action.board.number_after;
        }
        write_state(os, board, comments, ++state_nr);
    } while(iter != sudoku.undo_stack.begin());
    os << "</sudoku>\n";
}

void load_txt_file(Sudoku &sudoku, const char *filename) {
    FILE *file = fopen(filename, "r");
    if(!file)
        return;
    reset_sudoku(sudoku);
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            unsigned u = 0;
            fscanf(file, "%u", &u);
            put_number(sudoku, i, j, u);
        }
    }
    fclose(file);
}

void json_dump(Sudoku &sudoku, const char *filename) {
    FILE *file = fopen(filename, "w");
    if(!file)
        return;
    fprintf(file, "[");
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            unsigned u = sudoku.board[i][j];
            fprintf(file, "%u", u);
            fprintf(file, ",");
        }
    }
    fprintf(file, "]");
    fclose(file);
}
