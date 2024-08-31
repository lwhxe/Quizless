#include <iostream>
#include <iomanip>
#include <sstream>
#include <numeric>
#include <vector>
#include <string>
#include <random>
#include <windows.h>
#include <ranges>
#include <conio.h>
#include <functional>
#include <any>

using namespace std;

class console_part {
public:
	// Changes the color of the text or text background
	void color(int color) const {
		if (SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color) == 0) {
			// Handle error
		}
	}

	// Moves the cursor
	void gotoxy(int x, int y) const {
		COORD c{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
		if (SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c) == 0) {
			// Handle error
		}
	}
};

class effects {
private:
	// Converts all valid UPPERCASE Unicode characters into lowercase.
	string swedish_char_to_lower(const string& c) {
		if (c == "\xC3\x85") return "\xC3\xA5";  // Å to å
		if (c == "\xC3\x84") return "\xC3\xA4";  // Ä to ä
		if (c == "\xC3\x96") return "\xC3\xB6";  // Ö to ö
		return c;  // Return unchanged if not uppercase Å, Ä, or Ö
	}

	// Converts all letters into lowercase.
	string convert_swedish_to_lower(const string& input) {
		string result;
		for (size_t i = 0; i < input.length(); ++i) {
			if (i + 1 < input.length() && (unsigned char)input[i] == 0xC3) {
				result += swedish_char_to_lower(input.substr(i, 2));
				++i;
			}
			else {
				result += input[i];
			}
		}
		for (auto& x : result) {
			x = tolower(x);
		}
		return result;
	}
	string keep_swedish_chars(const string& input) {
		const string swedish_letters = "abcdefghijklmnopqrstuvwxyz\xC3\xA5\xC3\xA4\xC3\xB6";
		string result;
		copy_if(input.begin(), input.end(), back_inserter(result),
			[&swedish_letters](char c) { return swedish_letters.find(c) != string::npos; });
		return result;
	}
public:
	// Copies the valid Swedish letters of a string into another string and returns it. Used for filtering input.
	string filter(const string& input) {
		string sz_input = convert_swedish_to_lower(input);
		sz_input = keep_swedish_chars(sz_input);
		
		return sz_input;
	}

	// Measures edit distance between 2 strings
	int levenshtein_distance(const string& s1, const string& s2) {
		int len_s1 = s1.size();
		int len_s2 = s2.size();

		// Step 1: Initialize the matrix
		vector<vector<int>> levenshtein_matrix(len_s1 + 1, vector<int>(len_s2 + 1));

		// Step 2: Initialize the first row and first column
		for (int i = 0; i <= len_s1; ++i) {
			levenshtein_matrix[i][0] = i;
		}
		for (int j = 0; j <= len_s2; ++j) {
			levenshtein_matrix[0][j] = j;
		}

		// Step 3: Fill the matrix with the minimum distance
		for (int i = 1; i <= len_s1; ++i) {
			for (int j = 1; j <= len_s2; ++j) {
				int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;

				//levenshtein_matrix[i][j] = min({ levenshtein_matrix[i - 1][j] + 1, levenshtein_matrix[i][j - 1] + 1, levenshtein_matrix[i - 1][j - 1] + cost });
				// Manually compute the minimum value
				int deletion = levenshtein_matrix[i - 1][j] + 1;      // Deletion
				int insertion = levenshtein_matrix[i][j - 1] + 1;     // Insertion
				int substitution = levenshtein_matrix[i - 1][j - 1] + cost; // Substitution

				levenshtein_matrix[i][j] = deletion;
				if (insertion < levenshtein_matrix[i][j]) {
					levenshtein_matrix[i][j] = insertion;
				}
				if (substitution < levenshtein_matrix[i][j]) {
					levenshtein_matrix[i][j] = substitution;
				}
			}
		}

		// Step 4: Return the Levenshtein distance (bottom-right of the matrix)
		return levenshtein_matrix[len_s1][len_s2];
	}

	// Shuffling for random questions
	vector<int> shuffle(int end) {
		vector<int> v(end);
		random_device rd;
		iota(v.begin(), v.end(), 1);  // Fill vector with values from 1 to end
		ranges::shuffle(v, rd);       // Shuffle using C++20 ranges
		for (int i = 0; i < end; i++) {
			v[i]--;
		}
		return v;
	}
};

class questions {
private:
	effects e;
	// Question and Input handling here.
	int question_loop(const vector<string>& elements, const vector<string>& signs, int i, int n) {
		string query;
		int threshold = 23; // Just in case someone figures out a way to buffer overflow, might not destroy the program completely... Probably still will tho.
		int pointsback = 0;

		cout.flush();
		cout << "\033[H\033[J";
		cout << n << ". " << "Vad heter detta elementet? " << signs[i] << endl;
		// getline(cin >> ws, query);

		while (true) {
			getline(cin, query); // Read a line of input

			// Check if the input is empty
			if (query.empty()) {
				cout << "\033[H\033[J";
				cout << n << ". " << "Vad heter detta elementet? " << signs[i] << endl;
				continue;  // Ignore empty input and reprompt
			}

			break;  // Exit the loop if non-empty input is provided
		}

		query = e.filter(query);

		int distance = e.levenshtein_distance(query, elements[i]);
		if (distance >= 13) {
			cout << "Tar du inte detta seri\xC3\xB6st?" << endl;
			cin.get();
			return -1;
		}

		switch (distance <= 2) {
		case 1:
			if (query == elements[i]) {
				cout << "\u001b[32mYAYYY, du har r\xC3\xA4tt! Har far du 3 poang!\u001b[37m" << endl;
				pointsback = 3;
				break;
			}
			cout << "\u001b[33mOOHH, n\xC3\xA4stan... har... ta 1 gratis po\xC3\xA4ng!" << endl;
			cout << "\u001b[32mR\xC3\xA4tt svar var: " << elements[i] << "\u001b[37m" << endl;
			pointsback = 1;
			break;
		default:
			cout << "\u001b[31mOH NO, tyvarr fel... testa igen.\u001b[37m" << endl;
			cout << "R\xC3\xA4tt svar var: " << elements[i] << endl;
			break;
		}
		cout << "Tryck p\xC3\xA5 [ENTER] f\xC3\xB6r att forts\xC3\xA4tta..." << endl;

		// This is for a bug that would make the user able to submit the next query without seeing the prompt.
		cin.clear();
		cin.ignore(10000, '\n');
		cout << "\033[H\033[J" << endl;

		return pointsback;
	}
public:
	// Main Quiz functionality, calls upon question_loop inside a loop.
	int question_game_symbols(vector<string> elements, vector<string> signs) {
		vector<int> shufflist = e.shuffle(30);
		int points = 0;
		int current_point;

		cout << endl;

		for (int i = 0; i < 30; i++) {
			current_point = question_loop(elements, signs, shufflist[i], i + 1);
			if (current_point != -1) {
				points += current_point;
			}
			else {
				return 1;
			}
		}
		double percent = (static_cast<double>(points) / 90) * 100;
		cout << "Dina po\xC3\xA4ng: ";
		cout << fixed << setprecision(2) << percent << "%" << endl;

		cin.get();
		return 0;
	}
};

class menu {
private:
	// Main Menu
	vector<int> colors;
	int selected = 0;
	int key;

	vector<string> prompts;
	vector<function<any(const vector<any>&)>> functions;
public:
	console_part cp; // noice, wait what
	
	menu(vector<int> colors, int selected, vector<string> prompts, vector<function<any(const vector<any>&)>> functions) : colors(colors), selected(selected), prompts(prompts), functions(functions) {}

	int	main_menu(vector<int> colors, int selected, int key) const {
		while (true) {
			system("cls"); // Clear screen

			for (int i = 0; i < 3; i++) {
				colors[i] = (i == selected) ? 13 : 7;
				cp.gotoxy(0, i + 1);
				cp.color(colors[i]);
				switch (i) {
					case 0: cout << prompts[i]; break;
					case 1: cout << prompts[i]; break;
					case 2: cout << prompts[i]; break;
				}
			}

			key = _getch();

			if (key == 72 && selected > 0) selected--; // Up arrow
			if (key == 80 && selected < 2) selected++; // Down arrow

			if (key == 13) { // Enter key
				
			}
		}
	}
};

class filesys {
private:

public:

};

int main() {
	console_part cp; // noice, wait what
	questions q;
	system("chcp 65001"); // To support Unicode characters
	// system("@echo off"); // I can't remember why, my uncle just said so...
	vector<string> elements = {"v\xC3\xA4te", "helium", "litium", "beryllium", "bor", "kol", "kv\xC3\xA4ve", "syre", "fluor", "neon", "natrium", "magnesium", "aluminium", "kisel", "fosfor", "svavel", "klor", "argon", "kalium", "kalcium", "skandium", "titan", "vanadin", "krom", "mangan", "j\xC3\xA4rn", "kobolt", "nickel", "koppar", "zink"};
	vector<string> signs = { "H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn" };
	vector<function<any(const vector<any>&)>> functions;

	// Main Menu
	int colors[] = { 7, 7, 7 };
	int selected = 0;
	int key;

	while (true) {
		system("cls"); // Clear screen
		
		for (int i = 0; i < 3; i++) {
			colors[i] = (i == selected) ? 13 : 7;
			cp.gotoxy(0, i + 1);
			cp.color(colors[i]);
			switch (i) {
			case 0: cout << "1. Quiz"; break;
			case 1: cout << "2. Kolla Element och Symboler"; break;
			case 2: cout << "3. Avsluta"; break;
			}
		}

		key = _getch();

		if (key == 72 && selected > 0) selected--; // Up arrow
		if (key == 80 && selected < 2) selected++; // Down arrow

		if (key == 13) { // Enter key
			system("cls");
			switch (selected) {
				case 0:
					q.question_game_symbols(elements, signs);
					break;
				case 1:
					for (int i = 0; i < 30; i++) {
						cout << elements[i] << " : " << signs[i] << endl;
					}
					cout << "Tryck p\xC3\xA5 [ENTER] f\xC3\xB6r att forts\xC3\xA4tta...";
					cin.get();
					break;
				case 2:
					cp.color(7);
					return 0;
			}
		}
	}
}
