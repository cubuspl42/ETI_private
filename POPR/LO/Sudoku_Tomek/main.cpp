#include "conio2.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// -------- Definicje stałych --------

#define GRID_WIDTH 9
#define UNSET_FIELD 0
#define TIME_X 2
#define TIME_Y 0
#define GRID_X 2
#define GRID_Y 2
#define HELP_X
#define HELP_X
#define COUNTER_X 2
#define COUNTER_Y 22
#define NONCONFLICTING_NUMBERS_X 11
#define NONCONFLICTING_NUMBERS_Y 22

#ifndef KEY_UP
#define KEY_UP 0x48 // y--
#endif

#ifndef KEY_DOWN
#define KEY_DOWN 0x50 // y++
#endif

#ifndef KEY_DOWN
#define KEY_LEFT 0x4b // x--
#endif

#ifndef KEY_RIGHT
#define KEY_RIGHT 0x4d // x++
#endif

// -------- Lista undo/redo --------

struct UndoableMove {
    UndoableMove* next;
    UndoableMove* prev;
    int i, j;
    int numberBefore;
    int numberAfter;
    int changedComment;
};

void appendUndoableMove(UndoableMove *move, int i, int j, int numberBefore, int numberAfter, int changedComment) {
    UndoableMove *newMove = (UndoableMove*)malloc(sizeof(UndoableMove));
    newMove->prev = NULL;
    newMove->next = NULL;
    newMove->i = i;
    newMove->j = j;
    newMove->numberBefore = numberBefore;
    newMove->numberAfter = numberAfter;
    newMove->changedComment = changedComment;
    // Powiązania elementów listy
    newMove->prev = move;
    move->next = newMove;
}

void undoMove(UndoableMove **mostRecentMove, int grid[GRID_WIDTH][GRID_WIDTH],
              bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], int *counter) {
    UndoableMove *move = *mostRecentMove;
    bool canUndo = move->prev != NULL;
    if(canUndo) {
        ++(*counter);
        if(move->numberBefore != -1 && move->numberAfter != -1)
            grid[move->i][move->j] = move->numberBefore;
        if(move->changedComment != -1) {
            comments[move->i][move->j][move->changedComment] = !comments[move->i][move->j][move->changedComment];
        }
        *mostRecentMove = move->prev;
    }
}

void redoMove(UndoableMove **mostRecentMove, int grid[GRID_WIDTH][GRID_WIDTH],
              bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], int *counter) {
    UndoableMove *move = *mostRecentMove;
    bool canRedo = move->next != NULL;
    if(canRedo) {
        ++(*counter);
        move = move->next;
        if(move->numberBefore != -1 && move->numberAfter != -1)
            grid[move->i][move->j] = move->numberAfter;
        if(move->changedComment != -1) {
            comments[move->i][move->j][move->changedComment] = !comments[move->i][move->j][move->changedComment];
        }
        *mostRecentMove = move;
    }
}

void doMove(UndoableMove **mostRecentMove, int grid[GRID_WIDTH][GRID_WIDTH],
            bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], int *counter,
            int i, int j, int numberBefore, int numberAfter, int changedComment) {
    // Usunięcie "ogona" listy
    UndoableMove *move = *mostRecentMove;
    while(move) {
        move = move->next;
        free(move);
    }
    
    appendUndoableMove(*mostRecentMove, i, j, numberBefore, numberAfter, changedComment);
    redoMove(mostRecentMove, grid, comments, counter);
}

// -------- Funkcje rysujące --------

void putLine(int x, int y, const char *line) {
    gotoxy(x, y);
    cputs(line);
}

void showGrid(int x, int y, const int grid[GRID_WIDTH][GRID_WIDTH], int cursorI, int cursorJ) {
    int x_ = x;
    int y_ = y;
    putLine(x, y++, "╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╟───┼───┼───╫───┼───┼───╫───┼───┼───╢");
    putLine(x, y++, "║   │   │   ║   │   │   ║   │   │   ║");
    putLine(x, y++, "╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝");
    
    y = y_ + 1;
    for(int i = 0; i < GRID_WIDTH; ++i) {
        x = x_ + 2;
        for(int j = 0; j < GRID_WIDTH; ++j) {
            gotoxy(x, y);
            if(grid[i][j])
                putch(grid[i][j]+'0');
            if(i == cursorI && j == cursorJ) {
                gotoxy(x-1, y);
                putch('>');
            }
            x += 4;
        }
        y += 2;
    }
}

void showNonconflictingNumbers(int x, int y, const int numbers[GRID_WIDTH], int n) {
    gotoxy(x, y);
    putch('(');
    gotoxy(x+2*GRID_WIDTH+2, y);
    putch(')');
    x += 2;
    for(int i = 0; i < n; ++i) {
        gotoxy(x, y);
        putch(numbers[i]+'0');
        x += 2;
    }
}

void showCounter(int x, int y, int counter) {
    char buffer[16]; // Bufor 16-bajtowy (z zapasem)
    sprintf(buffer, "[%d]", counter);
    gotoxy(x, y);
    cputs(buffer);
}

void showTime(int x, int y, time_t startTime, time_t currentTime) {
    char buffer[16]; // Bufor 16-bajtowy (z zapasem)
    int diff = difftime(currentTime, startTime); // różnica w sekundach
    sprintf(buffer, "%d:%d", diff / 60, diff % 60);
    gotoxy(x, y);
    cputs(buffer);
}


// -------- Logika gry --------

// Wypełnia tablicę liczbami niepowodującymi konfliktu w polu (i, j)
// Zwracamy liczbę elementów w tablicy
int nonconflictingNumbers(int numbers[GRID_WIDTH], int i, int j, const int grid[GRID_WIDTH][GRID_WIDTH]) {
    int numberOfNonconflictingNumbers = 0;
    for(int number = 1; number <= GRID_WIDTH; ++number) {
        bool nonconflicting = true;
        // Sprawdzamy, czy powoduje konflikt w kolumnie
        for(int k = 0; nonconflicting && k < GRID_WIDTH; ++k) {
            if(grid[k][j] == number)
                nonconflicting = false;
        }
        // Sprawdzamy, czy powoduje konflikt w rzędzie
        for(int k = 0; nonconflicting && k < GRID_WIDTH; ++k) {
            if(grid[i][k] == number)
                nonconflicting = false;
        }
        // Sprawdzamy, czy powoduje konflikt w podkwadracie
        int subsquareI = i/3; // Indeks pionowy podkwadratu (0, 1, lub 2)
        int subsquareJ = j/3; // Indeks poziomy podkwadratu (0, 1, lub 2)
        for(int k = 0; k < 3; ++k) {
            for(int l = 0; nonconflicting && l < 3; ++l) {
                int numberI = subsquareI*3+k;
                int numberJ = subsquareJ*3+l;
                if (grid[numberI][numberJ] == number) {
                    nonconflicting = false;
                }
            }
        }
        // Jeżeli nie powoduje konfliktu, dodajemy do tablicy
        if(nonconflicting) {
            numbers[numberOfNonconflictingNumbers++] = number;
        }
    }
    return numberOfNonconflictingNumbers;
}

// Zwraca (poprzez wskaźniki) koordynaty pola, dla którego podpowiedź jest jednoznaczna oraz sugerowaną liczbę.
// Jeśli nie da się jej wyznaczyć, zwraca trzy razy -1
void giveHint(int *hintI, int *hintJ, int *hintNumber, const int grid[GRID_WIDTH][GRID_WIDTH]) {
    for(int i = 0; i < GRID_WIDTH; ++i) {
        for(int j = 0; j < GRID_WIDTH; ++j) {
            int numbers[GRID_WIDTH];
            int numberOfNonconflictingNumbers = nonconflictingNumbers(numbers, i, j, grid);
            if(numberOfNonconflictingNumbers == 1) {
                *hintI = i;
                *hintJ = j;
                *hintNumber = numbers[0];
            }
        }
    }
    *hintI = -1;
    *hintJ = -1;
    *hintNumber = -1;
}

void insertNumber(int grid[GRID_WIDTH][GRID_WIDTH], UndoableMove **mostRecentMove, int *counter, int i, int j, int number) {
    bool conflict = true;
    int numbers[GRID_WIDTH];
    int n = nonconflictingNumbers(numbers, i, j, grid);
    for(int i = 0; i < n; ++i) {
        if(numbers[i] == number)
            conflict = false;
    }
    if(!conflict && grid[i][j] != number) {
        int numberBefore = grid[i][j];
        int numberAfter = number;
        int changedComment = -1; // Nie zmieniamy komentarza
        doMove(mostRecentMove, grid, NULL, counter, i, j, numberBefore, numberAfter, changedComment);
    }
}

void deleteNumber(int grid[GRID_WIDTH][GRID_WIDTH], UndoableMove **mostRecentMove, int *counter, int i, int j, int number) {
    if(grid[i][j] != UNSET_FIELD) {
        int numberBefore = grid[i][j];
        int numberAfter = UNSET_FIELD;
        int changedComment = -1; // Nie zmieniamy komentarza
        doMove(mostRecentMove, grid, NULL, counter, i, j, numberBefore, numberAfter, changedComment);
    }
}

void changeComment(bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], UndoableMove **mostRecentMove, int *counter,
                   int i, int j, int number) {
    int numberBefore = -1; // Nie zmieniamy liczby na planszy
    int numberAfter = -1;
    int changedComment = number;
    doMove(mostRecentMove, NULL, comments, counter, i, j, numberBefore, numberAfter, changedComment);
}

// -------- Odczyt/zapis --------

void readGridFromFile(int grid[GRID_WIDTH][GRID_WIDTH], const char *filename) {
    FILE *file = fopen("r", filename);
    for(int i = 0; i < GRID_WIDTH; ++i) {
        for(int j = 0; j < GRID_WIDTH; ++j) {
            fscanf(file, "%d", &grid[i][j]);
        }
    }
    fclose(file);
}

void saveGridToFile(int grid[GRID_WIDTH][GRID_WIDTH], const char *filename) {
    FILE *file = fopen("w", filename);
    for(int i = 0; i < GRID_WIDTH; ++i) {
        for(int j = 0; j < GRID_WIDTH; ++j) {
            fprintf(file, "%d", grid[i][j]);
        }
    }
    fclose(file);
}

// -------- Funkcja główna --------

int main()
{
    UndoableMove undoListHead;
    undoListHead.prev = NULL;
    undoListHead.next = NULL;
    UndoableMove *mostRecentMove = &undoListHead;
    int cursorI = 0;
    int cursorJ = 0;
    int counter = 0;
    int grid[GRID_WIDTH][GRID_WIDTH] = {{UNSET_FIELD}};
    bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH] = {{{false}}};
    
    settitle("Tomasz Rogowski NR_INDEKSU");
    textbackground(BLACK);
    clrscr();
    showGrid(GRID_X, GRID_Y, grid, cursorI, cursorJ);
    
    time_t startTime = time(NULL);
    int ch = 0;
    do {
        ch = getch();
        clrscr();
        
        //		textcolor(7);
        //		gotoxy(55, 1);
        //		cputs("q = wyjscie");
        //		gotoxy(55, 2);
        //		cputs("strzalki = poruszanie");
        //		gotoxy(55, 3);
        //		cputs("spacja = zmiana koloru");
        //		gotoxy(x, y);
        //		textcolor(attr);
        //		putch('*');
        
        if(isdigit(ch) && ch != '0') {
            insertNumber(grid, &mostRecentMove, &counter, cursorI, cursorJ, ch - '0');
        } else {
            switch (ch) {
                case 'u':
                    undoMove(&mostRecentMove, grid, comments, &counter);
                    break;
                case 'r':
                    redoMove(&mostRecentMove, grid, comments, &counter);
                    break;
                case 'l': {
                    int numbers[GRID_WIDTH];
                    int n = nonconflictingNumbers(numbers, cursorI, cursorJ, grid);
                    showNonconflictingNumbers(NONCONFLICTING_NUMBERS_X, NONCONFLICTING_NUMBERS_Y, numbers, n);
                    break;
                }
                case KEY_UP:
                    if(cursorI > 0)
                        --cursorI;
                    break;
                case KEY_DOWN:
                    if(cursorI < GRID_WIDTH-1)
                        ++cursorI;
                    break;
                case KEY_LEFT:
                    if(cursorJ > 0)
                        --cursorJ;
                    break;
                case KEY_RIGHT:
                    if(cursorJ < GRID_WIDTH-1)
                        ++cursorJ;
                    break;
            }
        }
        showGrid(GRID_X, GRID_Y, grid, cursorI, cursorJ);
        showCounter(COUNTER_X, COUNTER_Y, counter);
        showTime(TIME_X, TIME_Y, startTime, time(NULL));
    } while (ch != 'q');
    
    return 0;
}

