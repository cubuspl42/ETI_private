#ifndef sudoku_h
#define sudoku_h

#include "common.h"
#include "list.h"
#include <stdint.h>

const unsigned base_number = 3;
const unsigned base_number_sq = base_number*base_number;
const unsigned base_mask = (1<<base_number_sq)-1;

struct State {
    int nr = 0;
    int board_numbers[base_number_sq*base_number_sq] = {0};
    uint16_t comments[base_number_sq][base_number_sq] = {{0}};
};

struct Action {
    bool is_comment = false;
    uint8_t y = 0, x = 0;
    union {
        struct {
            uint8_t number_before;
            uint8_t number_after;
        } board;
        struct {
            uint8_t flipped_number;
        } comment;
    };
};

enum Flags {
    COMMENT_EDIT_BIT,
    CONFLICT_BIT,
    HIGHLIGHT_BIT,
    POSSIBILITIES_BIT,
    HINT_BIT,
};

struct Sudoku {
    uint8_t flags = 0;
    uint8_t board[base_number_sq][base_number_sq] = {{0}};
    uint16_t row_numbers_masks[base_number_sq] = {0};
    uint16_t column_numbers_masks[base_number_sq] = {0};
    uint16_t square_numbers_masks[base_number][base_number] = {{0}};
    uint16_t comments[base_number_sq][base_number_sq] = {{0}};
    uint8_t pointer_y = 0, pointer_x = 0;
    uint8_t conflict_y = 0, conflict_x = 0;
    uint8_t highlighted_number = 0;
    uint16_t possibilities = 0;
    uint8_t hint_y = 0, hint_x = 0, hint_number = 0;
    list<Action> undo_stack;
    list<Action>::iterator last_action;
    unsigned active_state_index = 0;
    unsigned num_actions = 0;
};

void reset_sudoku(Sudoku &sudoku);
bool creates_conflict(const Sudoku &sudoku, unsigned y, unsigned x, unsigned number,
                      unsigned *conflict_y, unsigned *conflict_x);
unsigned possible_numbers(const Sudoku &sudoku, unsigned y, unsigned x);
unsigned simple_hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x);
unsigned advanced_hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x);
unsigned hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x);
void give_hint(Sudoku &sudoku);
void perform_action(Sudoku &sudoku, const Action &action, unsigned board_assign);
void do_action(Sudoku &sudoku, const Action &action);
bool redo(Sudoku &sudoku);
bool undo(Sudoku &sudoku);
void put_number(Sudoku &sudoku, unsigned y, unsigned x, unsigned number);
void accept_hint(Sudoku &sudoku);
void reject_hint(Sudoku &sudoku);
void highlight_number(Sudoku &sudoku, unsigned y, unsigned x);
void list_possibilities(Sudoku &sudoku, unsigned y, unsigned x);
void flip_comment(Sudoku &sudoku, unsigned y, unsigned x, unsigned number);
void move_pointer(Sudoku &sudoku, int dy, int dx);
void load_xml_file(Sudoku &sudoku, const char *filename);
void save_xml_file(Sudoku &sudoku, const char *filename);
void load_txt_file(Sudoku &sudoku, const char *filename);
void json_dump(Sudoku &sudoku, const char *filename);

#endif