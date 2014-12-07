#include <algorithm> // max()
#include <cstdlib> // system()
#include <iostream> // cin, cout
#include <string> // string
using namespace std;

#define MAX_BOARD_SIZE 35 // Mniej więcej tyle znaków nie powoduje problemów w cmd.exe

int read_int() {
	int i = 0;
	do {
		// http://stackoverflow.com/questions/257091/how-do-i-flush-the-cin-buffer
		if (!cin) {
			cin.clear(); // Usuwamy błędy ze strumienia
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorujemy znaki z bufora, np. 'abcd', gdyż oczekujemy liczby
		}
		cin >> i;
		if (!cin) {
			cout << "Blad: nie zostala wpisana liczba calkowita" << endl;
		}
	} while (!cin);
	return i;
}

void clear() {
	system("cls");
}

bool player_won(int width, int height, int streak, char current_player, char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE]) {
	// |      |	
	// |x     |
	// | x    |
	// |  x   |
	// |   x  |
	for (int i = 0; i <= height - streak; ++i) {
		int longest_streak = 0;
		for (int i_ = i, j = 0; i_ < height && j < width; ++i_, ++j) {
			if (board[i_][j] == current_player)
				++longest_streak;
			else longest_streak = 0;
			if (longest_streak >= streak) {
				return true;
			}
		}
	}
	// |      |	
	// |     x|
	// |    x |
	// |   x  |
	// |  x   |
	for (int i = 0; i <= height - streak; ++i) {
		int longest_streak = 0;
		for (int i_ = i, j = width - 1; i_ < height && j < width; ++i_, --j) {
			if (board[i_][j] == current_player)
				++longest_streak;
			else longest_streak = 0;
			if (longest_streak >= streak) {
				return true;
			}
		}
	}

	// |  x   |	
	// |   x  |
	// |    x |
	// |     x|
	// |      |
	for (int j = 1; j <= width - streak; ++j) {
		int longest_streak = 0;
		for (int j_ = j, i = 0; j_ < width && i < height; ++j_, ++i) {
			if (board[i][j_] == current_player)
				++longest_streak;
			else longest_streak = 0;
			if (longest_streak >= streak) {
				return true;
			}
		}
	}
	// |   x  |	
	// |  x   |
	// | x    |
	// |x     |
	// |      |
	for (int j = width - 1; j >= streak - 1; --j) {
		int longest_streak = 0;
		for (int j_ = j, i = 0; j_ < width && i < height; --j_, ++i) {
			if (board[i][j_] == current_player)
				++longest_streak;
			else longest_streak = 0;
			if (longest_streak >= streak) {
				return true;
			}
		}
	}
	// |      |	
	// |      |
	// |      |
	// | xxxx |
	// |      |
	for (int i = 0; i < height; ++i) {
		int longest_streak = 0;
		for (int j = 0; j < width; ++j) {
			if (board[i][j] == current_player)
				++longest_streak;
			else longest_streak = 0;
			if (longest_streak >= streak) {
				return true;
			}
		}
	}
	// |      |	
	// | x    |
	// | x    |
	// | x    |
	// | x    |
	for (int j = 0; j < height; ++j) {
		int longest_streak = 0;
		for (int i = 0; i < width; ++i) {
			if (board[i][j] == current_player)
				++longest_streak;
			else longest_streak = 0;
			if (longest_streak >= streak) {
				return true;
			}
		}
	}
	return false;
}

// Funkcja implemtująca jedną rozgrywkę. Zwraca true, jeśli ma być rozegrana kolejna.
bool game(int width, int height, int streak) {
	char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = { { 0 } }; // '\0' - brak elementu, 'x' - krzyzyk, 'o' - kółko
	int num_elements = 0; // ile znaczków jest na planszy?
	bool gameover = false; // Czy któryś gracz wygrał, lub jest remis? (oczekujemy wtedy tylko na potwierdzenie)
	int move = 0; // Który to ruch? (indeksujemy od zera)
	string info; // Informacja dla gracza

	while(true) {
		clear();
		// Wypisujemy planszę
		cout << " ";
		for (int j = 0; j < width; ++j) {
			if (j < 9)
				cout << (char)(j + '1');
			else if (j < 10 + 'z' - 'a') cout << (char)(j - 9 + 'a');
		}
		cout << "\n";
		for (int i = 0; i < height + 2; ++i) {
			for (int j = 0; j < width + 2; ++j) {
				if (i == 0 && j == 0)
					cout << (char)0xC9; // lewy górny róg
				else if (i == 0 && j == width + 1)
					cout << (char)0xBB; // prawy górny róg
				else if (i == height + 1 && j == 0)
					cout << (char)0xC8; // lewy dolny róg
				else if (i == height + 1 && j == width + 1)
					cout << (char)0xBC; // prawy dolny róg
				else if (i == 0 || i == height + 1)
					cout << (char)0xCD; // górna/dolna krawędź
				else if (j == 0 || j == width + 1)
					cout << (char)0xBA; // prawa/lewa krawędź
				else {
					char c = board[i - 1][j - 1];
					cout << c ? c : ' '; // spacja, 'x' lub 'o'
				}
			}
			cout << endl;
		}
		cout << info << endl; // Wypisujemy informację dla użytkownika
		cout << ">> "; // Taki jakby "wiersz polecenia"
		info = "";

		// Obsługujemy wejście
		char ch = 0;
		unsigned int column = 0;
		if (cin >> column) { // Próbujemy wczytac liczbę (numer kolumny)
			if (gameover) {
				return true; // Jeśli gracz wpisał liczbę po końcu gry, oznacza to że chce grać od nowa
			}

			if (column > width) { // Numer kolumny jest większy niż szerokość planszy
				info = "Nie ma takiej kolumny";
				continue;
			}

			char current_player = 'x'; // `x` zaczyna
			if (move % 2) // Jeśli `move` ma resztę z dzielenia przez dwa, jest to ruch `o`
				current_player = 'o';

			bool no_space = false;
			for (int i = height - 1; i >= 0; --i) { // Ta pętla sprawdza, czy element się zmieści do kolumny, jeśli tak - wpisuje go do niej
				if (board[i][column - 1] == '\0') {
					board[i][column - 1] = current_player;
					++num_elements;
					break;
				}
				if (i == 0) {
					no_space = true;
				}
			}
			if (no_space) {
				info = "Nie ma miejsca w tej kolumnie";
				continue;
			}

			if (player_won(width, height, streak, current_player, board)) { // Wygrana obecnego gracza
				gameover = true;
				info = string("Wygral gracz `") + current_player + "`! Wpisz `q`, aby wyjsc lub inny znak, aby zagrac ponownie";
			} else if(num_elements == width*height) { // Cała plansza wypełniona. Remis!
				gameover = true;
				info = "Remis! Wpisz `q`, aby wyjsc lub inny znak, aby zagrac ponownie";
			}

			++move; // Przechodzimy do następnego ruchu
		} else {
			cin.clear(); // Nie udało się wczytać liczby, więc czyścimy błędy w cin
			cin >> ch; // Wczytujemy pojedyńczy znak
			if (ch == 'q') {
				return false;
			} else if (gameover) {
				return true; // Znak inny niż `q`, gracz chce grać od nowa
			} else if (ch == '?') {
				clear();
				const char *help =
					"Pomoc:\n"
					"Wpisz liczbe, aby dodac element u dolu danej kolumny.\n"
					"Wpisz `q`, aby wyjsc z gry\n"
					"Wpisz `?`, aby wyswietlic ten komunikat\n\n"
					"Nacisnij dowolny klawisz, aby kontynuowac";
				cout << help << endl;
				cin.ignore(); // Ignorujemy znak '\n', który najprawdopodobniej znajduje się właśnie w buforze
				// Inaczej, to by od razu wróciło, zadowalając się tymże znakiem, zamiast czekać na wciśnięcie dowolnego klawisza:
				cin.get();
			} else {
				info = "Niepoprawna komenda. Wpisz `?`, aby uzyskac pomoc";
			}
		}
	}
}

int main()
{
	unsigned int height, width, streak;
	do {
		clear();
		height = width = streak = 0;

		do {
			cout << "Podaj wysokosc planszy (w przedziale od 1 do " << MAX_BOARD_SIZE << ")" << endl;
			height = read_int();
		} while (height == 0 || height > MAX_BOARD_SIZE);

		do {
			cout << "Podaj szerokosc planszy (w przedziale od 1 do " << MAX_BOARD_SIZE << ")" << endl;
			width = read_int();
		} while (width == 0 || width > MAX_BOARD_SIZE);

		do {
			cout << "Podaj liczbe kolejnych elementow dajacych wygrana (w przedziale od 1 do " << max(height, width) << ")" << endl;
			streak = read_int();
		} while (streak == 0 || streak > max(height, width));

	} while (game(width, height, streak)); // Właściwa rozgrywka
	return 0;
}
