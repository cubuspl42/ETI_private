#include "sudoku.h"
#include "xml.h"
#include <stdio.h>
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
    unsigned r = 0, c = 0;
    for(int i=0;i<base_number_sq;++i) {
        if(!creates_conflict(sudoku, y, x, i+1, &c, &c)) {
            toggle_bit(&r, i);
        }
    }
    return r;
}

unsigned simple_hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x) {
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            if(sudoku.board[i][j])
                continue;
            unsigned numbers = possible_numbers(sudoku, i, j);
            if(has_single_bit(numbers)) {
                for(int k=0; k<base_number_sq; ++k) {
                    if(check_bit(numbers, k)) {
                        *hint_y = i;
                        *hint_x = j;
                        return k+1;
                    }
                }
            }
        }
    }
    return 0;
}

static unsigned naked_single(const Sudoku &sudoku, const unsigned forbidden_numbers_masks[base_number_sq][base_number_sq],
                             unsigned *hint_y, unsigned *hint_x) {
    for(int i = 0; i < base_number_sq; ++i) {
        for(int j = 0; j < base_number_sq; ++j) {
            if(sudoku.board[i][j])
                continue;
            unsigned allowed_numbers_mask = ~forbidden_numbers_masks[i][j] & (1<<base_number_sq)-1;
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
                fprintf(stderr, "hidden_single: row %d (candidates: %d)\n", i, row_data[k][i].num_candidates);
                return k+1;
            }
        }
        for(int j = 0; j < base_number_sq; ++j) {
            if(column_data[k][j].num_candidates == 1) {
                *hint_y = column_data[k][j].index, *hint_x = j;
                fprintf(stderr, "hidden_single: column %d (candidates: %d)\n", j, column_data[k][j].num_candidates);
                return k+1;
            }
        }
    }
    return 0;
}

unsigned pointing_pairs(const Sudoku &sudoku, unsigned forbidden_numbers_masks[base_number_sq][base_number_sq]) {
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

unsigned advanced_hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x) {
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
            fprintf(stderr, "naked_single: %d (%d, %d)\n", hint, *hint_y, *hint_x);
            return hint;
        }
        hint = hidden_single(sudoku, forbidden_numbers_masks, hint_y, hint_x);
        if(hint) {
            fprintf(stderr, "hidden_single: %d (%d, %d)\n", hint, *hint_y, *hint_x);
            return hint;
        }
        unsigned new_num_pointing_pairs = pointing_pairs(sudoku, forbidden_numbers_masks);
        fprintf(stderr, "num_pointing_pairs: %d -> %d\n", num_pointing_pairs, new_num_pointing_pairs);
        if(new_num_pointing_pairs > num_pointing_pairs) {
            progress = true;
            num_pointing_pairs = new_num_pointing_pairs;
        }
    }
    return 0;
}

unsigned hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x) {
    return advanced_hint(sudoku, hint_y, hint_x);
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

void perform_action(Sudoku &sudoku, const Action &action, unsigned board_assign) {
    if(action.is_comment) {
        toggle_bit(&sudoku.comments[action.y][action.x], action.comment.flipped_number - 1);
    } else {
        clear_bit(&sudoku.flags, CONFLICT_BIT);
        clear_bit(&sudoku.flags, POSSIBILITIES_BIT);
        clear_bit(&sudoku.flags, HINT_BIT);
        clear_bit(&sudoku.flags, HIGHLIGHT_BIT);
        sudoku.board[action.y][action.x] = board_assign;
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
}

bool redo(Sudoku &sudoku) {
    if(sudoku.last_action == sudoku.undo_stack.begin())
        return false;
    Action &action = *(--sudoku.last_action);
    perform_action(sudoku, action, action.board.number_after);
    return true;
}

bool undo(Sudoku &sudoku) {
    if(sudoku.last_action == sudoku.undo_stack.end())
        return false;
    Action &action = *sudoku.last_action;
    ++sudoku.last_action;
    perform_action(sudoku, action, action.board.number_before);
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

void load_xml_file(Sudoku &sudoku, const char *filename)
{
    reset_sudoku(sudoku);
    std::ifstream is(filename);
    expect_valid_declaration(is);
    AttrMap attrs;
    parse_start_tag(is, "sudoku", attrs);
    int active_state = atoi(attrs["active-state"].c_str());
    attrs.clear();
    while(try_parse_start_tag(is, "state", attrs)) {
        int curent_state = atoi(attrs["nr"].c_str());
        attrs.clear();
        parse_start_tag(is, "board", attrs);
        int board_numbers[base_number_sq*base_number_sq];
        unsigned n = parse_number_sequence(is, board_numbers, base_number_sq*base_number_sq);
        for(int k = 0; k < n; ++k) {
            int i = k/base_number_sq, j = k%base_number_sq;
            put_number(sudoku, i, j, board_numbers[k]);
        }
        parse_end_tag(is, "board");
        if(try_parse_start_tag(is, "comments", attrs)) {
            while(try_parse_start_tag(is, "comment", attrs)) {
                int i = atoi(attrs["row"].c_str()) - 1;
                int j = atoi(attrs["col"].c_str()) - 1;
                attrs.clear();
                int comment_numbers[base_number_sq];
                unsigned mask = 0, xor_mask = 0;
                unsigned n = parse_number_sequence(is, comment_numbers, base_number_sq);
                for(unsigned k = 0; k < n; ++k)
                    set_bit(&mask, comment_numbers[k]-1);
                xor_mask = mask ^ sudoku.comments[i][j];
                for(unsigned k = 0; k < base_number_sq; ++k) {
                    if(check_bit(xor_mask, k))
                        flip_comment(sudoku, i, j, k+1);
                }
                parse_end_tag(is, "comment");
            }
            parse_end_tag(is, "comments");
        }
        parse_end_tag(is, "state");
        if(active_state == curent_state)
            sudoku.last_action = sudoku.undo_stack.begin();
    }
    parse_end_tag(is, "sudoku");
}

void load_txt_file(Sudoku &sudoku, const char *filename) {
    reset_sudoku(sudoku);
    FILE *file = fopen(filename, "r");
    if(!file)
        return;
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            unsigned u = 0;
            fscanf(file, "%u", &u);
            sudoku.board[i][j] = u;
            sudoku.comments[i][j] = 0;
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
