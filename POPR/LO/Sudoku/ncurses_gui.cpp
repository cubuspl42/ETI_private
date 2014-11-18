#include "ncurses_gui.h"
#include "sudoku.h"
#include <curses.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>

const unsigned timeout_value = 500;
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

static WINDOW *init_ncurses() {
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

static void read_filename(char *filename, size_t buffer_size, const char *message) {
    clear(), echo(), curs_set(1);
    timeout(UINT_MAX);
    printw(message);
    getnstr(filename, buffer_size);
    timeout(timeout_value);
    noecho(), curs_set(0);
}

static bool load_file(Sudoku &sudoku, const char *filename) {
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
    reset_sudoku(sudoku);
    fclose(file);
    return true;
}

static bool save_file(const Sudoku &sudoku, const char *filename) {
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

static void load_file_dialog(Sudoku &sudoku) {
    char filename[512] = "";
    read_filename(filename, sizeof(filename),
                  "Uwaga! Wczytanie pliku zaskutkuje utratą niezapisanych danych\n"
                  "Podaj nazwę pliku wejściowego:\n");
    load_xml_file(sudoku, filename);
}

static void save_file_dialog(const Sudoku &sudoku) {
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

static void print_board(const Sudoku &sudoku) {
    mvprintw(board_y, board_x, sudoku_template);
    int y = board_y+board_padding_y;
    for(int i=0; i<base_number_sq; ++i, y+=number_margin_y+1) {
        int x = board_x+board_padding_x;
        for(int j=0; j<base_number_sq; ++j, x+=number_margin_x+1) {
            unsigned n = sudoku.board[i][j];
            chtype style = 0;
            if(check_bit(sudoku.flags, HIGHLIGHT_BIT) && n == sudoku.highlighted_number)
                style |= COLOR_PAIR(highlight_color_pair);
            if (i == sudoku.pointer_y && j == sudoku.pointer_x)
                style |= A_UNDERLINE;
            if(check_bit(sudoku.flags, CONFLICT_BIT) && i == sudoku.conflict_y && j == sudoku.conflict_x)
                style |= COLOR_PAIR(conflict_color_pair);
            if (check_bit(sudoku.flags, HINT_BIT) && i == sudoku.hint_y && j == sudoku.hint_x)
                n = sudoku.hint_number, style = A_BLINK;
            char c = n ? n+'0' : ' ';
            mvaddch(y, x, c | style);
            if(sudoku.comments[i][j])
                mvaddch(y, x+1, '*' | A_BOLD);
        }
    }
}

static void print_comments(const Sudoku &sudoku) {
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

static void print_possible_numbers(const Sudoku &sudoku) {
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

static void print_help(Sudoku &sudoku) {
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

void ncurses_gui_loop() {
    WINDOW *mainwin = init_ncurses();

    time_t start;
    time(&start);
    
    Sudoku sudoku;
    reset_sudoku(sudoku);
    
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
        print_header(start, sudoku.num_actions);
        print_board(sudoku);
        print_help(sudoku);
        print_comments(sudoku);
        print_possible_numbers(sudoku);
        //refresh();
    } while((key = getch()) != 'q');
    
    delwin(mainwin);
    endwin();
    refresh();
}
