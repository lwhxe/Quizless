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

using namespace std;
// Changes the color of the text or text background. Effectively \u001b[(x)m Replace x with appropriate color code in ANSI.
void color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Moves the cursor
void gotoxy(int x, int y) {
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

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
	return result;
}

/* Function to remove carriage returns from string

string remove_cr string str) {
	str.erase(remove(str.begin(), str.end(), '\r'), str.end());
	return str;
}*/

// Copies the valid Swedish letters of a string into another string and returns it. Used for filtering input.
string keep_swedish_letters(const string& input) {
	const string swedish_letters = "abcdefghijklmnopqrstuvwxyz\xC3\xA5\xC3\xA4\xC3\xB6";
	string result;
	copy_if(input.begin(), input.end(), back_inserter(result),
		[&swedish_letters](char c) { return swedish_letters.find(c) != string::npos; });
	return result;
}

// Measures edit distance between 2 strings
int levenshtein_distance(const string& s1, const string& s2) {
	int len_s1 = s1.size();
	int len_s2 = s2.size();

	// Step 1: Initialize the matrix
	vector<vector<int>> dp(len_s1 + 1, vector<int>(len_s2 + 1));

	// Step 2: Initialize the first row and first column
	for (int i = 0; i <= len_s1; ++i) {
		dp[i][0] = i;
	}
	for (int j = 0; j <= len_s2; ++j) {
		dp[0][j] = j;
	}

	// Step 3: Fill the matrix with the minimum distance
	for (int i = 1; i <= len_s1; ++i) {
		for (int j = 1; j <= len_s2; ++j) {
			int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;

			//dp[i][j] = min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
			// Manually compute the minimum value
			int deletion = dp[i - 1][j] + 1;      // Deletion
			int insertion = dp[i][j - 1] + 1;     // Insertion
			int substitution = dp[i - 1][j - 1] + cost; // Substitution

			dp[i][j] = deletion;
			if (insertion < dp[i][j]) {
				dp[i][j] = insertion;
			}
			if (substitution < dp[i][j]) {
				dp[i][j] = substitution;
			}
		}
	}

	// Step 4: Return the Levenshtein distance (bottom-right of the matrix)
	return dp[len_s1][len_s2];
}

// Shuffling for random questions
vector<int> shuffle(int end) {
	vector<int> v(end);
	random_device rd;
	iota(v.begin(), v.end(), 1);  // Fill vector with values from 1 to end
	ranges::shuffle(v, rd);       // Shuffle using C++20 ranges
	for (int i = 0; i < end; i++) {
		v[i] = v[i] - 1;  // Adjust values from 1-based to 0-based
	}
	return v;
}

// Question and Input handling here.
int question_loop(const vector<string>& elements, const vector<string>& signs, int i, int n) {
	string query;
	int threshold = 23;
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

	query = convert_swedish_to_lower(query);

	for (auto& x : query) {
		x = tolower(x);
	}

	query = keep_swedish_letters(query);
	int distance = levenshtein_distance(query, elements[i]);
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
	cin.clear();
	cin.ignore(10000, '\n');
	cout << "\033[H\033[J" << endl;

	return pointsback;
}

// Main Quiz functionality, calls upon question_loop inside a loop.
int question_game_symbols(vector<string> elements, vector<string> signs) {
	vector<int> shufflist = shuffle(30);
	int question;
	int points = 0;
	int point;

	cout << endl;

	for (int i = 0; i < 30; i++) {
		question = shufflist[i];
		point = question_loop(elements, signs, question, i + 1);
		if (point != -1) {
			points += point;
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

int main() {
	system("chcp 65001"); // To support Unicode characters
	system("@echo off");
	vector<string> filenames = { "elements.txt", "signs.txt" };
	vector<string> elements = {"v\xC3\xA4te", "helium", "litium", "beryllium", "bor", "kol", "kv\xC3\xA4ve", "syre", "fluor", "neon", "natrium", "magnesium", "aluminium", "kisel", "fosfor", "svavel", "klor", "argon", "kalium", "kalcium", "skandium", "titan", "vanadin", "krom", "mangan", "j\xC3\xA4rn", "kobolt", "nickel", "koppar", "zink"};
	vector<string> signs = { "H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn" };

	// Main Menu
	int colors[] = { 7, 7, 7 };
	int selected = 0;
	int key;

	while (true) {
		system("cls"); // Clear screen

		for (int i = 0; i < 3; i++) {
			colors[i] = (i == selected) ? 13 : 7;
			gotoxy(0, i + 1);
			color(colors[i]);
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
					question_game_symbols(elements, signs);
					break;
				case 1:
					for (int i = 0; i < 30; i++) {
						cout << elements[i] << " : " << signs[i] << endl;
					}
					cout << "Tryck p\xC3\xA5 [ENTER] f\xC3\xB6r att forts\xC3\xA4tta...";
					_getch();
					break;
				case 2:
					return 0;
			}
		}
	}
}
