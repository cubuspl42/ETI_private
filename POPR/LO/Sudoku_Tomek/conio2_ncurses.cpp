#include "conio2_ncurses.h"
#include <locale.h>
#include <stdlib.h>

void gotoxy(int x, int y) {
    move(y, x);
}

int wherex(void);

int wherey(void);

void _setcursortype(int cur_t);

void textattr(int newattr);

void textbackground(int newcolor) {
    
}

void textcolor(int newcolor) {
    
}

void clreol(void);

void clrscr(void) {
    clear();
}

void delline(void);

void insline(void);

int getche(void);

int kbhit(void);

int ungetch(int ch);

int cputs(const char *str) {
    return addstr(str);
}
int putch(int c) {
    return addch(c);
}

int gettext(int left, int top, int right, int bottom, void *dest);

int movetext(int left, int top, int right, int bottom, int destleft, int desttop);

int puttext(int left, int top, int right, int bottom, void *source);

void gettextinfo(struct text_info *info);

void textmode(int mode);

void settitle(const char *title) {
    
}

static int conio2_ncurses_init() {
    setlocale(LC_CTYPE, "");
    WINDOW * win = initscr();
    if (!win)
        fprintf(stderr, "Error initialising ncurses.\n");
    noecho();
    keypad(win, true);
    curs_set(0);
    return 0;
}

static int conio2_init = conio2_ncurses_init();
