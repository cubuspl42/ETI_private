// Kodowanie pliku: UTF-8
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include <Windows.h>
#include "conio2.h"

// -------- Definicje stałych --------

#include "sudoku_grid.h" // Osobny nagłówek, żeby w tym móc używać Unicode

#define SUBSQUARE_WIDTH 3
#define GRID_WIDTH SUBSQUARE_WIDTH*SUBSQUARE_WIDTH
#define UNSET_FIELD 0
#define TIME_X 2
#define TIME_Y 1
#define GRID_X 2
#define GRID_Y 2
#define COUNTER_X 2
#define COUNTER_Y 22
#define NONCONFLICTING_NUMBERS_X 2
#define NONCONFLICTING_NUMBERS_Y 24
#define COMMENTS_X 2
#define COMMENTS_Y 23
#define HELP_X 42
#define HELP_Y 4

#define KEY_UP 0x48
#define KEY_DOWN 0x50
#define KEY_LEFT 0x4b
#define KEY_RIGHT 0x4d
#define KEY_DELETE 0x53

#define BUFFER_SIZE 256 // Bezpieczny rozmiar bufora dla tego programu

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

UndoableMove* undoMove(UndoableMove *mostRecentMove, int sudoku[GRID_WIDTH][GRID_WIDTH],
              bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], int *counter)
{
    UndoableMove *move = mostRecentMove;
    bool canUndo = move->prev != NULL;
    if(canUndo) {
        ++(*counter);
        if(move->numberBefore != -1 && move->numberAfter != -1)
			sudoku[move->i][move->j] = move->numberBefore;
        if(move->changedComment != -1) {
            comments[move->i][move->j][move->changedComment] = !comments[move->i][move->j][move->changedComment];
        }
        return move->prev;
    }
	return mostRecentMove; // Nie udało się
}

UndoableMove* redoMove(UndoableMove *mostRecentMove, int sudoku[GRID_WIDTH][GRID_WIDTH],
              bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], int *counter)
{
    UndoableMove *move = mostRecentMove;
    bool canRedo = move->next != NULL;
    if(canRedo) {
        ++(*counter);
        move = move->next;
        if(move->numberBefore != -1 && move->numberAfter != -1)
			sudoku[move->i][move->j] = move->numberAfter;
        if(move->changedComment != -1) {
            comments[move->i][move->j][move->changedComment] = !comments[move->i][move->j][move->changedComment];
        }
        return move;
    }
	return mostRecentMove; // Nie udało się
}

UndoableMove* doMove(UndoableMove *mostRecentMove, int sudoku[GRID_WIDTH][GRID_WIDTH],
            bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], int *counter,
            int i, int j, int numberBefore, int numberAfter, int changedComment)
{
    // Usunięcie "ogona" listy
    UndoableMove *move = mostRecentMove;
    while(move) {
		free(move->next);
		move->next = NULL;
		move = move->next;
    }
    
    appendUndoableMove(mostRecentMove, i, j, numberBefore, numberAfter, changedComment);
	return redoMove(mostRecentMove, sudoku, comments, counter);
}

// -------- Funkcje rysujące --------

void putLine(int x, int y, const char *line)
{
    gotoxy(x, y);
    cputs(line);
}

void showSudoku(int x, int y, const int sudoku[GRID_WIDTH][GRID_WIDTH], int cursorI, int cursorJ, int hintI, int hintJ, int hintNumber, int markedNumber)
{
    int x_ = x;
    int y_ = y;
	for (int i = 0; i < sizeof(sudoku_grid) / sizeof(sudoku_grid[0]); ++i) {
		putLine(x, y++, sudoku_grid[i]);
	}
    y = y_ + 1;
    for(int i = 0; i < GRID_WIDTH; ++i) {
        x = x_ + 2;
        for(int j = 0; j < GRID_WIDTH; ++j) {
            gotoxy(x, y);
			if (i == hintI && j == hintJ) { // Jeżeli jest to podpowiedziana liczba
				putch(hintNumber + '0');
				gotoxy(x+1, y);
				putch('?');
			}
			else { // Jeżeli jest to zwykła liczba
				int number = sudoku[i][j];
				if (number)
					putch(number + '0');
				if (i == cursorI && j == cursorJ) {
					gotoxy(x - 1, y);
					putch('>');
				}
				if (number == markedNumber) {
					gotoxy(x + 1, y);
					putch('*');
				}
			}
            x += 4;
        }
        y += 2;
    }
}

void showNonconflictingNumbers(int x, int y, const int numbers[GRID_WIDTH], int n) {
	gotoxy(x, y);
	x += cputs("Mozliwe liczby ");
	putch('(');
	x += 2;
	for (int i = 0; i < n; ++i) {
		gotoxy(x, y);
		putch(numbers[i] + '0');
		x += 2;
	}
	gotoxy(x, y);
	putch(')');
}

void showCounter(int x, int y, int counter) {
	char buffer[BUFFER_SIZE];
    sprintf(buffer, "Ruchy: %d", counter);
    gotoxy(x, y);
    cputs(buffer);
}

void showTime(int x, int y, time_t startTime, time_t currentTime) {
	char buffer[BUFFER_SIZE];
    int diff = (int)difftime(currentTime, startTime); // różnica w sekundach
    sprintf(buffer, "%d:%d", diff / 60, diff % 60);
    gotoxy(x, y);
    cputs(buffer);
}

void showComments(int x, int y, int i, int j, const bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH]) {
	gotoxy(x, y);
	x += cputs("Komentarze ");
	putch('[');
	x += 2;
	for (int k = 0; k < GRID_WIDTH; ++k) {
		if (comments[i][j][k]) {
			gotoxy(x, y);
			putch(k+'0');
			x += 2;
		}
	}
	gotoxy(x, y);
	putch(']');
}

void showHelp(int x, int y, bool editComments, bool hintNeedsApproval) {
	if (editComments) {
		putLine(x, y++, "1 do 9 - zmien stan komentarza");
		putLine(x, y++, "k - wroc do trybu zwyklego");
	} 
	else if (hintNeedsApproval) {
		putLine(x, y++, "t - zatwierdz podpowiedz");
		putLine(x, y++, "n - odrzuc podpowiedz");
	}
	else {
		putLine(x, y++, "1 do 9 - wprowadz liczbe");
		putLine(x, y++, "k - tryb komentarza");
		putLine(x, y++, "u - cofnij ruch");
		putLine(x, y++, "r - powtorz ruch");
		putLine(x, y++, "l - lista mozliwych liczb");
		putLine(x, y++, "p - podpowiedz");
		putLine(x, y++, "m - tymczasowo oznacz wybrana liczbe");
	}
}

// -------- Logika gry --------

// Wypełnia tablicę liczbami niepowodującymi konfliktu w polu (i, j)
// Zwracamy liczbę elementów w tablicy
int nonconflictingNumbers(int numbers[GRID_WIDTH], int i, int j, const int sudoku[GRID_WIDTH][GRID_WIDTH]) {
    int numberOfNonconflictingNumbers = 0;
    for(int number = 1; number <= GRID_WIDTH; ++number) {
        bool nonconflicting = true;
        // Sprawdzamy, czy powoduje konflikt w kolumnie
        for(int k = 0; nonconflicting && k < GRID_WIDTH; ++k) {
			if (sudoku[k][j] == number)
                nonconflicting = false;
        }
        // Sprawdzamy, czy powoduje konflikt w rzędzie
        for(int k = 0; nonconflicting && k < GRID_WIDTH; ++k) {
			if (sudoku[i][k] == number)
                nonconflicting = false;
        }
        // Sprawdzamy, czy powoduje konflikt w podkwadracie
        int subsquareI = i/3; // Indeks pionowy podkwadratu (0, 1, lub 2)
        int subsquareJ = j/3; // Indeks poziomy podkwadratu (0, 1, lub 2)
        for(int k = 0; k < 3; ++k) {
            for(int l = 0; nonconflicting && l < 3; ++l) {
                int numberI = subsquareI*3+k;
                int numberJ = subsquareJ*3+l;
				if (sudoku[numberI][numberJ] == number) {
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
void giveHint(int *hintI, int *hintJ, int *hintNumber, const int sudoku[GRID_WIDTH][GRID_WIDTH]) {
    for(int i = 0; i < GRID_WIDTH; ++i) {
        for(int j = 0; j < GRID_WIDTH; ++j) {
			if (sudoku[i][j] != UNSET_FIELD)
				continue;
            int numbers[GRID_WIDTH];
			int numberOfNonconflictingNumbers = nonconflictingNumbers(numbers, i, j, sudoku);
            if(numberOfNonconflictingNumbers == 1) {
                *hintI = i;
                *hintJ = j;
                *hintNumber = numbers[0];
				return;
            }
        }
    }
    *hintI = -1;
    *hintJ = -1;
    *hintNumber = -1;
}

UndoableMove* insertNumber(int sudoku[GRID_WIDTH][GRID_WIDTH], UndoableMove *mostRecentMove, int *counter, int i, int j, int number) {
    bool conflict = true;
    int numbers[GRID_WIDTH];
	int n = nonconflictingNumbers(numbers, i, j, sudoku);
    for(int k = 0; k < n; ++k) {
        if(numbers[k] == number)
            conflict = false;
    }
	if (!conflict && sudoku[i][j] != number) {
		int numberBefore = sudoku[i][j];
        int numberAfter = number;
        int changedComment = -1; // Nie zmieniamy komentarza
		return doMove(mostRecentMove, sudoku, NULL, counter, i, j, numberBefore, numberAfter, changedComment);
    }
	return mostRecentMove;
}

UndoableMove* deleteNumber(int sudoku[GRID_WIDTH][GRID_WIDTH], UndoableMove *mostRecentMove, int *counter, int i, int j) {
	if (sudoku[i][j] != UNSET_FIELD) {
		int numberBefore = sudoku[i][j];
        int numberAfter = UNSET_FIELD;
        int changedComment = -1; // Nie zmieniamy komentarza
		return doMove(mostRecentMove, sudoku, NULL, counter, i, j, numberBefore, numberAfter, changedComment);
    }
	return mostRecentMove;
}

UndoableMove* changeComment(bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH], UndoableMove *mostRecentMove, int *counter,
                   int i, int j, int number) {
    int numberBefore = -1; // Nie zmieniamy liczby na planszy
    int numberAfter = -1;
    int changedComment = number;
    return doMove(mostRecentMove, NULL, comments, counter, i, j, numberBefore, numberAfter, changedComment);
}

// -------- Odczyt/zapis --------

void scanfFilename(char filename[BUFFER_SIZE]) {
	// Trick z SetConsoleMode pozwala używać scanf
	HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(handle, &mode); // Zapisz stan konsoli
	SetConsoleMode(handle, ~ENABLE_INSERT_MODE); // Wszystkie bity poza bitem flagi ENABLE_INSERT_MODE

	clrscr();
	gotoxy(1, 1);
	printf("Podaj nazwe pliku:\n");
	scanf("%128s", filename); // Maksymalnie 128 znakowe nazwy plików
	clrscr();

	SetConsoleMode(handle, mode); // Przywróć  zapisany stan
}

void readSudokuFromFile(int grid[GRID_WIDTH][GRID_WIDTH], const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file) {
		for (int i = 0; i < GRID_WIDTH; ++i) {
			for (int j = 0; j < GRID_WIDTH; ++j) {
				fscanf(file, "%d", &grid[i][j]);
			}
		}
		fclose(file);
	}
}

void saveSudokuToFile(int grid[GRID_WIDTH][GRID_WIDTH], const char *filename) {
	FILE *file = fopen(filename, "w");
	if (file) {
		for (int i = 0; i < GRID_WIDTH; ++i) {
			for (int j = 0; j < GRID_WIDTH; ++j) {
				fprintf(file, "%d ", grid[i][j]);
			}
			fprintf(file, "\n");
		}
		fclose(file);
	}
}

// -------- Funkcja główna --------

int main()
{
	bool editComments = false; // Czy jesteśmy w trybie komentarza?

	int hintI = -1; // Współrzędne podpowiedzianej liczby
	int hintJ = -1;
	int hintNumber = -1; // Podpowiedziana liczba

    UndoableMove undoListHead; // "Głowa" listy, niewypełniony element
    undoListHead.prev = NULL; // Zawsze NULL
    undoListHead.next = NULL; // Jeśli undoListHead.next == NULL, to lista UNDO/REDO jest pusta

    UndoableMove *mostRecentMove = &undoListHead;
    int cursorI = 0;
    int cursorJ = 0;
    int counter = 0;
	int sudoku[GRID_WIDTH][GRID_WIDTH] = { { UNSET_FIELD } };
    bool comments[GRID_WIDTH][GRID_WIDTH][GRID_WIDTH] = {{{false}}};
    
	settitle("Tomasz Rogowski NR_INDEKSU");
	textbackground(BLACK);
	_setcursortype(_NOCURSOR);
    
    time_t startTime = time(NULL);
    int ch = -1;
    do {
		clrscr();

		int markedNumber = -1; // Tymczasowo oznaczona, wybrana przez użytkownika liczba
		if (hintNumber != -1) { // Podpowiedź czeka na zatwierdzenie
			if (ch == 't') {
				mostRecentMove = insertNumber(sudoku, mostRecentMove, &counter, hintI, hintJ, hintNumber);
				hintI = hintJ = hintNumber = -1;
			} else if (ch == 'n') {
				hintI = hintJ = hintNumber = -1;
			}
		} else if (editComments) { // Jesteśmy w trybie komentarza
			if (isdigit(ch) && ch != '0') {
				mostRecentMove = changeComment(comments, mostRecentMove, &counter, cursorI, cursorJ, ch - '0');
			}
			else if (ch == 'k') {
				editComments = false;
			}
		} else { // Tryb zwykły
			if (isdigit(ch) && ch != '0') {
				mostRecentMove = insertNumber(sudoku, mostRecentMove, &counter, cursorI, cursorJ, ch - '0');
			} else
            switch (ch) {
				case KEY_DELETE: // Delete
					mostRecentMove = deleteNumber(sudoku, mostRecentMove, &counter, cursorI, cursorJ);
					break;
				case 'k':
					editComments = true;
					break;
                case 'u':
					mostRecentMove = undoMove(mostRecentMove, sudoku, comments, &counter);
                    break;
                case 'r':
					mostRecentMove = redoMove(mostRecentMove, sudoku, comments, &counter);
                    break;
                case 'l': {
                    int numbers[GRID_WIDTH];
					int n = nonconflictingNumbers(numbers, cursorI, cursorJ, sudoku);
                    showNonconflictingNumbers(NONCONFLICTING_NUMBERS_X, NONCONFLICTING_NUMBERS_Y, numbers, n);
                    break;
                }
				case 'p':
					giveHint(&hintI, &hintJ, &hintNumber, sudoku);
					break;
				case 'm': {
					int number = sudoku[cursorI][cursorJ];
					if (number != UNSET_FIELD)
						markedNumber = number; 
					break;
				}
				case 'o': {
					char filename[BUFFER_SIZE];
					scanfFilename(filename);
					readSudokuFromFile(sudoku, filename);
					break;
				}
				case 's': {
					char filename[BUFFER_SIZE];
					scanfFilename(filename);
					saveSudokuToFile(sudoku, filename);
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

		showSudoku(GRID_X, GRID_Y, sudoku, cursorI, cursorJ, hintI, hintJ, hintNumber, markedNumber);
        showCounter(COUNTER_X, COUNTER_Y, counter);
		showHelp(HELP_X, HELP_Y, editComments, hintNumber != -1);
		showComments(COMMENTS_X, COMMENTS_Y, cursorI, cursorJ, comments);
        showTime(TIME_X, TIME_Y, startTime, time(NULL));

		ch = getch();
    } while (ch != 'q');
    
    return 0;
}

