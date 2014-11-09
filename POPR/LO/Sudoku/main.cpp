// -*- coding: utf-8 -*-

#include "list.h"

#include <curses.h>

#include <iostream>
#include <list>
#include <stdlib.h>
#include <stdio.h>

const unsigned timeout_value = 500;
const unsigned read_filename_timeout_value = 10000;
const unsigned base_number = 3;
const unsigned base_number_sq = base_number*base_number;
const unsigned board_y = 2;
const unsigned board_x = 0;
const unsigned board_padding_y = 1;
const unsigned board_padding_x = 2;
const unsigned number_margin_y = 1;
const unsigned number_margin_x = 3;
const unsigned comments_y = 1;
const unsigned comments_x = 0;
const unsigned possible_numbers_y = 21;
const unsigned possible_numbers_x = 0;
const unsigned filename_y = 22;
const unsigned filename_x = 0;
const unsigned help_y = 23;
const unsigned help_x = 0;
const int highlight_color_pair = 1;
const int comment_color_pair = 2;
const int conflict_color_pair = 3;

enum State {
    DEFAULT_STATE,
    COMMENT_EDIT_STATE,
    PROVIDE_FILENAME_STATE
};

const char* help[] ={
// DEFAULT_STATE:
    "[←↑→↓] Przesuwanie kursora \n"
    "[1..9] Wstaw/nadpisz liczbę \n"
    "[0] Usuń liczbę \n"
    "[k] Tryb edycji komentarzy \n"
    "[h] Podświetl wybraną liczbę \n",
// COMMENT_EDIT_STATE:
    "[←↑→↓] Przesuwanie kursora \n"
    "[1..9] Dodaj/usuń liczbę z komentarza \n"
    "[k] Tryb edycji planszy \n"
    "[h] Podświetl wybraną liczbę \n",
// PROVIDE_FILENAME_STATE:
    "[enter] Potwierdź \n"
    "[esc] Anuluj \n"
};

const char sudoku_template[] = 
"╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n"
"║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║ 1 │ 1 │ 1 ║\n"
"╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝\n";

struct Step {
    uint8_t y = 0, x = 0;
    bool is_comment = false;
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

template<typename T>
inline bool check_bit(T n, unsigned bit) {
    return n & (1 << bit);
}

template<typename T>
inline void toggle_bit(T *n, unsigned bit) {
    *n ^= (1 << bit);
}

template<typename T>
inline unsigned count_bits(T n) {
    unsigned c;
    for (c = 0; n; ++c)
        n &= n - 1;
    return c;
}

float clamp(int n, int lower, int upper) {
    return std::max(lower, std::min(n, upper));
}

WINDOW *init_ncurses() {
    setlocale(LC_CTYPE, "");
    WINDOW * win = initscr();
    if (!win) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }
    noecho();
    keypad(win, true);
    curs_set(0);
    timeout(timeout_value);
    start_color();
    use_default_colors();
    init_pair(highlight_color_pair, COLOR_WHITE, COLOR_BLUE);
    init_pair(comment_color_pair, COLOR_WHITE, COLOR_BLACK);
    init_pair(conflict_color_pair, COLOR_WHITE, COLOR_RED);
    return win;
}

bool creates_conflict(const uint8_t board[base_number_sq][base_number_sq], uint8_t y, uint8_t x, uint8_t number,
                      int8_t *conflict_y, int8_t *conflict_x) {
    int square_y = y/base_number*base_number;
    int square_x = x/base_number*base_number;
    for(int i = square_y; i < square_y+base_number; ++i) {
        for(int j = square_x; j < square_x+base_number; ++j) {
            if(y == i && x == j)
                continue;
            if(board[i][j] == number) {
                *conflict_y = i;
                *conflict_x = j;
                return true;
            }
        }
    }
    for(int i=0; i<base_number_sq; ++i) {
        if(y == i)
            continue;
        if(board[i][x] == number) {
            *conflict_y = i;
            *conflict_x = x;
            return true;
        }
    }
    for(int j=0; j<base_number_sq; ++j) {
        if(x == j)
            continue;
        if(board[y][j] == number) {
            *conflict_y = y;
            *conflict_x = j;
            return true;
        }
    }
    *conflict_y = *conflict_x = -1;
    return false;
}

uint16_t possible_numbers(const uint8_t board[base_number_sq][base_number_sq], uint8_t y, uint8_t x) {
    uint16_t r = 0;
    int8_t c;
    for(int i=0;i<base_number_sq;++i) {
        if(!creates_conflict(board, y, x, i+1, &c, &c)) {
            toggle_bit(&r, i);
        }
    }
    return r;
}

uint8_t simple_hint(const uint8_t board[base_number_sq][base_number_sq], uint8_t *hint_y, uint8_t *hint_x) {
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            if(board[i][j])
                continue;
            uint16_t numbers = possible_numbers(board, i, j);
            if(count_bits(numbers) == 1) {
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

uint8_t hint(const uint8_t board[base_number_sq][base_number_sq], uint8_t *hint_y, uint8_t *hint_x) {
    return simple_hint(board, hint_y, hint_x);
}

void perform(const Step &step, uint8_t board[base_number_sq][base_number_sq],
             uint16_t comments[base_number_sq][base_number_sq], uint8_t board_assign, unsigned *num_steps) {
    if(step.is_comment) {
        toggle_bit(&comments[step.y][step.x], step.comment.flipped_number - 1);
    } else {
        board[step.y][step.x] = board_assign;
        ++*num_steps;
    }
}

void dostep(list<Step> *undo_stack, list<Step>::iterator *last_op, uint8_t board[base_number_sq][base_number_sq],
            uint16_t comments[base_number_sq][base_number_sq], const Step &step, unsigned *num_steps) {
    while(*last_op != undo_stack->begin()) {
        undo_stack->pop_front();
    }
    undo_stack->push_front(step);
    *last_op = undo_stack->begin();
    perform(step, board, comments, step.board.number_after, num_steps);
}

bool redo(list<Step> *undo_stack, list<Step>::iterator *last_op, uint8_t board[base_number_sq][base_number_sq],
          uint16_t comments[base_number_sq][base_number_sq], unsigned *num_steps) {
    if(*last_op == undo_stack->begin())
        return false;
    Step &step = *(--*last_op);
    perform(step, board, comments, step.board.number_after, num_steps);
    return true;
}

bool undo(list<Step> *undo_stack, list<Step>::iterator *last_op, uint8_t board[base_number_sq][base_number_sq],
          uint16_t comments[base_number_sq][base_number_sq], unsigned *num_steps) {
    if(*last_op == undo_stack->end())
        return false;
    Step &step = **last_op;
    ++*last_op;
    perform(step, board, comments, step.board.number_before, num_steps);
    return true;
}

void put_number(list<Step> *undo_stack, list<Step>::iterator *last_op, uint8_t board[base_number_sq][base_number_sq],
                int y, int x, int number, unsigned *num_steps) {
    assert(number >= 0 && number <= base_number_sq); // 0 means "delete number"
    if(board[y][x] == number)
        return;
    Step step;
    step.y = y, step.x = x;
    step.board.number_before = board[y][x], step.board.number_after = number;
    dostep(undo_stack, last_op, board, nullptr, step, num_steps);
}

void flip_comment(list<Step> *undo_stack, list<Step>::iterator *last_op, uint16_t comments[base_number_sq][base_number_sq],
                uint8_t y, uint8_t x, uint8_t number, unsigned *num_steps) {
    assert(number >= 1 && number <= base_number_sq);
    Step step;
    step.y = y, step.x = x, step.is_comment = true;
    step.comment.flipped_number = number;
    dostep(undo_stack, last_op, nullptr, comments, step, num_steps);
}

void move_pointer(uint8_t *pointer_y, uint8_t *pointer_x, int dy, int dx, int8_t *conflict_y, uint8_t *highlight_number) {
    int py = *pointer_y, px = *pointer_x;
    py += dy, px += dx;
    *pointer_y = clamp(py, 0, base_number_sq-1);
    *pointer_x = clamp(px, 0, base_number_sq-1);
    *highlight_number = 0;
    *conflict_y = -1;
}

void read_filename(char *filename, size_t buffer_size, const char *message) {
    clear(), echo(), curs_set(1);
    timeout(UINT_MAX);
    printw(message);
    getnstr(filename, buffer_size);
    timeout(timeout_value);
    noecho(), curs_set(0);
}

bool load_file(const char *filename, uint8_t board[base_number_sq][base_number_sq],
               uint16_t comments[base_number_sq][base_number_sq], list<Step> *undo_stack) {
    FILE *file = fopen(filename, "r");
    if(!file)
        return false;
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            unsigned u = 0;
            fscanf(file, "%u", &u);
            board[i][j] = u;
            comments[i][j] = 0;
        }
    }
    undo_stack->clear();
    fclose(file);
    return true;
}

bool save_file(const char *filename, const uint8_t board[base_number_sq][base_number_sq],
               const uint16_t comments[base_number_sq][base_number_sq], const list<Step> *undo_stack) {
    FILE *file = fopen(filename, "w");
    if(!file)
        return false;
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            unsigned u = board[i][j];
            fprintf(file, "%u ", u);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    return true;
}

void load_file_dialog(uint8_t board[base_number_sq][base_number_sq],
                      uint16_t comments[base_number_sq][base_number_sq], list<Step> *undo_stack) {
    char filename[512] = "";
    read_filename(filename, sizeof(filename),
                  "Uwaga! Wczytanie pliku zaskutkuje utratą niezapisanych danych\n"
                  "Podaj nazwę pliku wejściowego:\n");
    load_file(filename, board, comments, undo_stack);
}

void save_file_dialog(const uint8_t board[base_number_sq][base_number_sq],
                      const uint16_t comments[base_number_sq][base_number_sq], const list<Step> *undo_stack) {
    char filename[512] = "";
    read_filename(filename, sizeof(filename),
                  "Podaj nazwę pliku wyjściowego:\n");
    save_file(filename, board, comments, undo_stack);
}

void print_header(time_t start, unsigned num_steps) {
    time_t now;
    time(&now);
    int seconds = (int)difftime(now, start);
    int ss = seconds % 60;
    int mm = seconds / 60;
    mvprintw(0, 0, "Czas: %d:%d Liczba kroków: %d", mm, ss, num_steps);
}

void print_board(uint8_t board[base_number_sq][base_number_sq], uint16_t comments[base_number_sq][base_number_sq],
                 uint8_t pointer_y, uint8_t pointer_x, int8_t conflict_y, int8_t conflict_x, uint8_t highlight_number,
                 uint8_t hint_y, uint8_t hint_x, uint8_t hint_number) {
    mvprintw(board_y, board_x, sudoku_template);
    int y = board_y+board_padding_y;
    for(int i=0; i<base_number_sq; ++i, y+=number_margin_y+1) {
        int x = board_x+board_padding_x;
        for(int j=0; j<base_number_sq; ++j, x+=number_margin_x+1) {
            uint8_t n = board[i][j];
            chtype style = 0;
            if (hint_number && i == hint_y && j == hint_x)
                n = hint_number, style |= A_BLINK;
            if(highlight_number && n == highlight_number)
                style |= COLOR_PAIR(highlight_color_pair);
            if (i == pointer_y && j == pointer_x)
                style |= A_UNDERLINE;
            if(i == conflict_y && j == conflict_x)
                //mvaddch(y, x-1, '!' | A_BLINK | A_BOLD);
                style |= COLOR_PAIR(conflict_color_pair);
            char c = n ? n+'0' : ' ';
            mvaddch(y, x, c | style);
            
            if(comments[i][j])
                mvaddch(y, x+1, '*' | A_BOLD);
        }
    }
}

void print_comments(uint16_t comments[base_number_sq][base_number_sq], uint8_t y, uint8_t x, bool bold) {
    const char *pre = "Komentarz:";
    int attr = bold ? A_BOLD : A_NORMAL;
    attron(attr);
    mvprintw(comments_y, comments_x, pre);
    attroff(attr);
    char buffer[32], *ptr = buffer;
    ptr += sprintf(ptr, " { ");
    for(int i=0; i<base_number_sq; ++i) {
        if(check_bit(comments[y][x], i)) {
            ptr += sprintf(ptr, "%d ", i+1);
        }
    }
    ptr += sprintf(ptr, "}");
    mvprintw(comments_y, comments_x+strlen(pre), buffer);
}

void print_possible_numbers(uint16_t possible) {
    if(!possible)
        return;
    char buffer[32], *ptr = buffer;
    ptr += sprintf(ptr, "Możliwości: [ ");
    for(int i=0; i<base_number_sq; ++i) {
        if(check_bit(possible, i)) {
            ptr += sprintf(ptr, "%d ", i+1);
        }
    }
    ptr += sprintf(ptr, "]");
    mvprintw(possible_numbers_y, possible_numbers_x, buffer);
}

void print_help(State state, uint8_t y, uint8_t x) {
    mvprintw(y, x, help[state]);
}

void game() {
    time_t start;
    time(&start);
    
    uint8_t board[base_number_sq][base_number_sq] = {{0}};
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            board[i][j] = rand()%20+1;
            if(board[i][j] > 9)
                board[i][j] = 0;
        }
    }
    uint16_t comments[base_number_sq][base_number_sq] = {{0}};
    uint8_t pointer_y = 0, pointer_x = 0;
    int8_t conflict_y = -1, conflict_x = -1; // if negative, there's no conflict
    uint8_t hint_y = 0, hint_x = 0, hint_number = 0;
    uint8_t highlight_number = 0;
    uint16_t possible = 0;
    State state = DEFAULT_STATE;
    list<Step> undo_stack;
    list<Step>::iterator last_op = undo_stack.end();
    unsigned num_steps = 0;
    
    int key = 0;
    do {
        if(key >= '1' && key <= '9') {
            int number = key - '0';
            if(state == DEFAULT_STATE) {
                if(!creates_conflict(board, pointer_y, pointer_x, number, &conflict_y, &conflict_x)) {
                    put_number(&undo_stack, &last_op, board, pointer_y, pointer_x, number, &num_steps);
                }
            } else if(state == COMMENT_EDIT_STATE) {
                flip_comment(&undo_stack, &last_op, comments, pointer_y, pointer_x, number, &num_steps);
            }
        } else switch(key) {
            case KEY_UP:
                move_pointer(&pointer_y, &pointer_x, -1, 0, &conflict_y, &highlight_number);
                break;
            case KEY_DOWN:
                move_pointer(&pointer_y, &pointer_x, 1, 0, &conflict_y, &highlight_number);
                break;
            case KEY_LEFT:
                move_pointer(&pointer_y, &pointer_x, 0, -1, &conflict_y, &highlight_number);
                break;
            case KEY_RIGHT:
                move_pointer(&pointer_y, &pointer_x, 0, 1, &conflict_y, &highlight_number);
                break;
            case KEY_BACKSPACE:
            case KEY_DC:
            case '0':
                if(state == DEFAULT_STATE && board[pointer_y][pointer_x]) {
                    conflict_y = -1;
                    put_number(&undo_stack, &last_op, board, pointer_y, pointer_x, 0, &num_steps);
                }
                break;
            case 'u':
                undo(&undo_stack, &last_op, board, comments, &num_steps);
                break;
            case 'r':
                redo(&undo_stack, &last_op, board, comments, &num_steps);
                break;
            case 'k':
                if(state == DEFAULT_STATE) {
                    conflict_y = -1;
                    state = COMMENT_EDIT_STATE;
                } else if(state == COMMENT_EDIT_STATE) {
                    state = DEFAULT_STATE;
                }
                break;
            case 'h':
                conflict_y = -1;
                highlight_number = board[pointer_y][pointer_x];
                break;
            case 'l':
                possible = possible_numbers(board, pointer_y, pointer_x);
                break;
            case 'o':
                load_file_dialog(board, comments, &undo_stack);
                break;
            case 'i':
                load_file("default.txt", board, comments, &undo_stack);
                break;
            case 's':
                save_file_dialog(board, comments, &undo_stack);
                break;
            case 'p':
                hint_number = hint(board, &hint_y, &hint_x);
                break;
            default:
                break;
        }
        clear();
        print_header(start, num_steps);
        print_board(board, comments, pointer_y, pointer_x, conflict_y, conflict_x, highlight_number,
                    hint_y, hint_x, hint_number);
        print_help(state, help_y, help_x);
        print_comments(comments, pointer_y, pointer_x, state == COMMENT_EDIT_STATE);
        print_possible_numbers(possible);
        refresh();
    } while((key = getch()) != 'q');
}

int main(void) {
    WINDOW *mainwin = init_ncurses();
    game();
    delwin(mainwin);
    endwin();
    refresh();
    return EXIT_SUCCESS;
}
