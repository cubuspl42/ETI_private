// -*- coding: utf-8 -*-

#include "list.h"

#include <curses.h>

#include <iostream>
#include <list>
#include <stdlib.h>
#include <stdio.h>

const bool dev_mode = true;

const unsigned timeout_value = 500;
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
const unsigned help_y = 23;
const unsigned help_x = 0;
const int highlight_color_pair = 1;
const int comment_color_pair = 2;
const int conflict_color_pair = 3;

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
    uint16_t comments[base_number_sq][base_number_sq] = {{0}};
    uint8_t pointer_y = 0, pointer_x = 0;
    uint8_t conflict_y = 0, conflict_x = 0;
    uint8_t hint_y = 0, hint_x = 0, hint_number = 0;
    uint8_t highlighted_number = 0;
    uint16_t possibilities = 0;
    list<Step> undo_stack;
    list<Step>::iterator last_step;
    unsigned num_steps = 0;
};

template<typename T>
inline bool check_bit(T n, unsigned bit) {
    return n & (1 << bit);
}


template<typename T>
inline void set_bit(T *n, unsigned bit) {
    *n |= 1 << bit;
}

template<typename T>
inline void clear_bit(T *n, unsigned bit) {
    *n &= ~(1 << bit);
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

template<typename T>
inline bool has_single_bit(T n) {
   return n && !(n & (n - 1));
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

uint8_t simple_hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x) {
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            if(sudoku.board[i][j])
                continue;
            unsigned numbers = possible_numbers(sudoku, i, j);
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

unsigned advanced_hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x) {
    unsigned row_numbers_masks[base_number_sq] = {0}; // maska zawieranych liczb przez dany rząd
    unsigned column_numbers_masks[base_number_sq] = {0}; // ... daną kolumnę ...
    unsigned square_numbers_masks[base_number][base_number] = {{0}}; // ... i kwadrat 3x3
    
    // konstruowanie masek
    for(int sq_i = 0; sq_i < base_number; ++sq_i) {
        int sq_y=sq_i*base_number;
        for(int sq_j = 0; sq_j < base_number; ++sq_j) {
            int sq_x=sq_j*base_number;
            for(int i = sq_y; i < sq_y+base_number; ++i) {
                for(int j = sq_x; j < sq_x+base_number; ++j) {
                    unsigned n = sudoku.board[i][j];
                    if(n) {
                        set_bit(&row_numbers_masks[i], n-1);
                        set_bit(&column_numbers_masks[j], n-1);
                        set_bit(&square_numbers_masks[sq_i][sq_j], n-1);
                    }
                }
            }
        }
    }
    
    // główna pętla, która trwa tak długo jak wykluczamy jakieś kolumny lub rzędy dla jakiejkolwiek liczby
    bool progress = true;
    while(progress) {
        fprintf(stderr, "...\n");
        progress = false;
        for(int sq_i = 0; sq_i < base_number; ++sq_i) {
            unsigned sq_y=sq_i*base_number;
            for(int sq_j = 0; sq_j < base_number; ++sq_j) { // dla każdego kwadratu 3x3 (sq_i, sq_j)
                fprintf(stderr, "[%d,%d]\n", sq_i, sq_j);
                unsigned sq_x=sq_j*base_number;
                unsigned number_rows_masks[base_number_sq] = {0}; // maska rzędów w kwadracie, w których dozwolona jest dana liczba
                unsigned number_columns_masks[base_number_sq] = {0}; // ... kolumn ...
                for(int i = sq_y; i < sq_y+base_number; ++i) {
                    for(int j = sq_x; j < sq_x+base_number; ++j) { // dla każdego pola (i, j) w tym kwadracie
                        unsigned n = sudoku.board[i][j];
                        if(n)
                            continue; // obchodzą nas tylko niewypełnione pola
                        unsigned field_mask =   ~row_numbers_masks[i] & // maska dozwolonych liczb dla pola (i, j)
                                                ~column_numbers_masks[j] &
                                                ~square_numbers_masks[sq_i][sq_j] &
                                                (1 << (base_number_sq)) - 1;
                        bool bingo = has_single_bit(field_mask);
                        for(int k = 0; k < base_number_sq; ++k) { // dla każdej możliwej liczby
                            if(check_bit(field_mask, k)) { // jeśli jest dozwolna na tym polu
                                if(bingo) { // jeśli tylko ona jest dozwolona: koniec
                                    *hint_y = i;
                                    *hint_x = j;
                                    fprintf(stderr, "Bingo: %d\n\n", k+1);
                                    return k+1;
                                } else { // w innym przypadku, zaznaczamy, że jest dozwolona w danym rzędzie / kolumnie
                                    //fprintf(stderr, "Liczba %d jest dozwolona na polu do (%d,%d)\n", k+1, i, j);
                                    set_bit(&number_rows_masks[k], i);
                                    set_bit(&number_columns_masks[k], j);
                                }
                            }
                        }
                    }
                }
                for(int k = 0; k < base_number_sq; ++k) { // dla każdej możliwej liczby
                    bool single_row = has_single_bit(number_rows_masks[k]);
                    bool single_column = has_single_bit(number_columns_masks[k]);
                    if(!single_row && !single_column) // nic nie możemy zrobić
                        continue;
                    unsigned single_row_i = 0, single_column_j = 0;
                    if(single_row) { // jeśli w kwadracie jest ona dozwolona tylko w jednym rzędzie
                        for(int i = 0; i < base_number_sq; ++i) {
                            if(check_bit(number_rows_masks[k], i)) {
                                single_row_i = i; // zapisujemy indeks tego rzędu
                                set_bit(&row_numbers_masks[i], k); // stwierdzamy, że się w nim zawiera
                                progress = true; // niech trwa główna pętla
                                fprintf(stderr, "Skreslamy %d w rzedzie %d\n", k+1, i+1);
                            }
                        }
                    }
                    if(single_column) { // ... analogicznie
                        for(int j = 0; j < base_number_sq; ++j) {
                            if(check_bit(number_columns_masks[k], j)) {
                                single_column_j = j;
                                set_bit(&column_numbers_masks[j], k);
                                progress = true;
                                fprintf(stderr, "Skreslamy %d w kolumnie %d\n", k+1, j+1);
                            }
                        }
                    }
                    if(single_column && single_row) { // liczba jest dozwolona tylko w jedym polu: koniec
                        *hint_y = single_row_i;
                        *hint_x = single_column_j;
                        fprintf(stderr, "Droga eliminacji: %d\n\n", k+1);
                        return k+1;
                    } else break; // jest dozwolona tylko w jednym rzędzie lub kolumnie: kontynuujemy główną pętlę
                }
            }
        }
    }
    return 0;
}

uint8_t hint(Sudoku &sudoku, unsigned *hint_y, unsigned *hint_x) {
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

void perform(Sudoku &sudoku, const Step &step, unsigned board_assign) {
    if(step.is_comment) {
        toggle_bit(&sudoku.comments[step.y][step.x], step.comment.flipped_number - 1);
    } else {
        clear_bit(&sudoku.flags, CONFLICT_BIT);
        clear_bit(&sudoku.flags, POSSIBILITIES_BIT);
        clear_bit(&sudoku.flags, HINT_BIT);
        clear_bit(&sudoku.flags, HIGHLIGHT_BIT);
        sudoku.board[step.y][step.x] = board_assign;
        ++sudoku.num_steps;
    }
}

void dostep(Sudoku &sudoku, const Step &step) {
    while(sudoku.last_step != sudoku.undo_stack.begin()) {
        sudoku.undo_stack.pop_front();
    }
    sudoku.undo_stack.push_front(step);
    sudoku.last_step = sudoku.undo_stack.begin();
    perform(sudoku, step, step.board.number_after);
}

bool redo(Sudoku &sudoku) {
    if(sudoku.last_step == sudoku.undo_stack.begin())
        return false;
    Step &step = *(--sudoku.last_step);
    perform(sudoku, step, step.board.number_after);
    return true;
}

bool undo(Sudoku &sudoku) {
    if(sudoku.last_step == sudoku.undo_stack.end())
        return false;
    Step &step = *sudoku.last_step;
    ++sudoku.last_step;
    perform(sudoku, step, step.board.number_before);
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
    Step step;
    step.y = y, step.x = x;
    step.board.number_before = sudoku.board[y][x];
    step.board.number_after = number;
    dostep(sudoku, step);
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
    Step step;
    step.y = y, step.x = x, step.is_comment = true;
    step.comment.flipped_number = number;
    dostep(sudoku, step);
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

void read_filename(char *filename, size_t buffer_size, const char *message) {
    clear(), echo(), curs_set(1);
    timeout(UINT_MAX);
    printw(message);
    getnstr(filename, buffer_size);
    timeout(timeout_value);
    noecho(), curs_set(0);
}

bool load_file(Sudoku &sudoku, const char *filename) {
    FILE *file = fopen(filename, "r");
    if(!file)
        return false;
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            unsigned u = 0;
            fscanf(file, "%u", &u);
            sudoku.board[i][j] = u;
            sudoku.comments[i][j] = 0;
        }
    }
    sudoku.undo_stack.clear();
    sudoku.last_step = sudoku.undo_stack.end();
    sudoku.num_steps = 0;
    sudoku.flags = 0;
    fclose(file);
    return true;
}

bool save_file(const Sudoku &sudoku, const char *filename) {
    FILE *file = fopen(filename, "w");
    if(!file)
        return false;
    for(int i=0; i<base_number_sq; ++i) {
        for(int j=0; j<base_number_sq; ++j) {
            unsigned u = sudoku.board[i][j];
            fprintf(file, "%u ", u);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    return true;
}

void load_file_dialog(Sudoku &sudoku) {
    char filename[512] = "";
    read_filename(filename, sizeof(filename),
                  "Uwaga! Wczytanie pliku zaskutkuje utratą niezapisanych danych\n"
                  "Podaj nazwę pliku wejściowego:\n");
    load_file(sudoku, filename);
}

void save_file_dialog(const Sudoku &sudoku) {
    char filename[512] = "";
    read_filename(filename, sizeof(filename),
                  "Podaj nazwę pliku wyjściowego:\n");
    save_file(sudoku, filename);
}

void print_header(time_t start, unsigned num_steps) {
    time_t now;
    time(&now);
    int seconds = (int)difftime(now, start);
    int ss = seconds % 60;
    int mm = seconds / 60;
    mvprintw(0, 0, "Czas: %d:%d Liczba kroków: %d", mm, ss, num_steps);
}

void print_board(const Sudoku &sudoku) {
    mvprintw(board_y, board_x, sudoku_template);
    int y = board_y+board_padding_y;
    for(int i=0; i<base_number_sq; ++i, y+=number_margin_y+1) {
        int x = board_x+board_padding_x;
        for(int j=0; j<base_number_sq; ++j, x+=number_margin_x+1) {
            unsigned n = sudoku.board[i][j];
            chtype style = 0;
            if (check_bit(sudoku.flags, HINT_BIT) && i == sudoku.hint_y && j == sudoku.hint_x)
                n = sudoku.hint_number, style |= A_BLINK;
            if(check_bit(sudoku.flags, HIGHLIGHT_BIT) && n == sudoku.highlighted_number)
                style |= COLOR_PAIR(highlight_color_pair);
            if (i == sudoku.pointer_y && j == sudoku.pointer_x)
                style |= A_UNDERLINE;
            if(check_bit(sudoku.flags, CONFLICT_BIT) && i == sudoku.conflict_y && j == sudoku.conflict_x)
                style |= COLOR_PAIR(conflict_color_pair);
            char c = n ? n+'0' : ' ';
            mvaddch(y, x, c | style);
            if(sudoku.comments[i][j])
                mvaddch(y, x+1, '*' | A_BOLD);
        }
    }
}

void print_comments(const Sudoku &sudoku) {
    static const char label[] = "Komentarz:";
    int attr = check_bit(sudoku.flags, COMMENT_EDIT_BIT)  ? A_BOLD : A_NORMAL;
    attron(attr);
    mvprintw(comments_y, comments_x, label);
    attroff(attr);
    char buffer[32], *ptr = buffer;
    ptr += sprintf(ptr, " { ");
    for(int i=0; i<base_number_sq; ++i) {
        if(check_bit(sudoku.comments[sudoku.pointer_y][sudoku.pointer_x], i)) {
            ptr += sprintf(ptr, "%d ", i+1);
        }
    }
    ptr += sprintf(ptr, "}");
    mvprintw(comments_y, comments_x+sizeof(label)-1, buffer);
}

void print_possible_numbers(const Sudoku &sudoku) {
    if(!check_bit(sudoku.flags, POSSIBILITIES_BIT))
        return;
    char buffer[32], *ptr = buffer;
    ptr += sprintf(ptr, "Możliwości: [ ");
    for(int i=0; i<base_number_sq; ++i) {
        if(check_bit(sudoku.possibilities, i)) {
            ptr += sprintf(ptr, "%d ", i+1);
        }
    }
    ptr += sprintf(ptr, "]");
    mvprintw(possible_numbers_y, possible_numbers_x, buffer);
}

void print_help(Sudoku &sudoku) {
    unsigned y = help_y, x = help_x;
    mvprintw(y, x, "[←↑→↓] Przesuwanie kursora"), ++y;
    mvprintw(y, x, "[h] Podświetl wybraną liczbę"), ++y;
    if(check_bit(sudoku.flags, COMMENT_EDIT_BIT)) {
         mvprintw(y, x, "[1..9] Dodaj/usuń liczbę z komentarza"), ++y;
         mvprintw(y, x, "[k] Tryb edycji planszy"), ++y;
    } else {
         mvprintw(y, x, "[1..9] Wstaw/nadpisz liczbę"), ++y;
         mvprintw(y, x, "[0] Usuń liczbę"), ++y;
         mvprintw(y, x, "[k] Tryb edycji komentarzy"), ++y;
    }
}

void game() {
    time_t start;
    time(&start);
    
    Sudoku sudoku;
    sudoku.last_step = sudoku.undo_stack.end();
    
    int key = 0;
    do {
        if(key >= '0' && key <= '9') {
            int number = key - '0';
            if(check_bit(sudoku.flags, COMMENT_EDIT_BIT)) {
                if(number)
                    flip_comment(sudoku, sudoku.pointer_y, sudoku.pointer_x, number);
            } else {
                put_number(sudoku, sudoku.pointer_y, sudoku.pointer_x, number);
            }
        } else switch(key) {
            case KEY_UP:
                move_pointer(sudoku, -1, 0);
                break;
            case KEY_DOWN:
                move_pointer(sudoku, 1, 0);
                break;
            case KEY_LEFT:
                move_pointer(sudoku, 0, -1);
                break;
            case KEY_RIGHT:
                move_pointer(sudoku, 0, 1);
                break;
            case KEY_BACKSPACE:
            case KEY_DC:
            case 'u':
                undo(sudoku);
                break;
            case 'r':
                redo(sudoku);
                break;
            case 'k':
                toggle_bit(&sudoku.flags, COMMENT_EDIT_BIT);
                break;
            case 'h':
                highlight_number(sudoku, sudoku.pointer_y, sudoku.pointer_x);
                break;
            case 'l':
                list_possibilities(sudoku, sudoku.pointer_y, sudoku.pointer_x);
                break;
            case 'o':
                load_file_dialog(sudoku);
                break;
            case 'i':
                load_file(sudoku, "default.txt");
                break;
            case 's':
                save_file_dialog(sudoku);
                break;
            case 'p':
                give_hint(sudoku);
                break;
            case '[':
                accept_hint(sudoku);
                break;
            case ']':
                give_hint(sudoku);
                accept_hint(sudoku);
                break;
            default:
                break;
        }
        clear();
        print_header(start, sudoku.num_steps);
        print_board(sudoku);
        print_help(sudoku);
        print_comments(sudoku);
        print_possible_numbers(sudoku);
        //refresh();
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
