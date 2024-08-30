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

void color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) {
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

std::string swedish_char_to_lower(const std::string& c) {
	if (c == "\xC3\x85") return "\xC3\xA5";  // Å to å
	if (c == "\xC3\x84") return "\xC3\xA4";  // Ä to ä
	if (c == "\xC3\x96") return "\xC3\xB6";  // Ö to ö
	return c;  // Return unchanged if not uppercase Å, Ä, or Ö
}

std::string convert_swedish_to_lower(const std::string& input) {
	std::string result;
	for (size_t i = 0; i < input.length(); ++i) {
		if (i + 1 < input.length() && (unsigned char)input[i] == 0xC3) {
			result += swedish_char_to_lower(input.substr(i, 2));
			++i;  // Skip the next byte as we've already processed it
		}
		else {
			result += input[i];
		}
	}
	return result;
}

/* Function to remove carriage returns from string

std::string remove_cr(std::string str) {
	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
	return str;
}*/

std::string keep_swedish_letters(const std::string& input) {
	const std::string swedish_letters = "abcdefghijklmnopqrstuvwxyz\xC3\xA5\xC3\xA4\xC3\xB6";
	std::string result;
	std::copy_if(input.begin(), input.end(), std::back_inserter(result),
		[&swedish_letters](char c) { return swedish_letters.find(c) != std::string::npos; });
	return result;
}

int levenshtein_distance(const std::string& s1, const std::string& s2) {
	int len_s1 = s1.size();
	int len_s2 = s2.size();

	// Step 1: Initialize the matrix
	std::vector<std::vector<int>> dp(len_s1 + 1, std::vector<int>(len_s2 + 1));

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

			//dp[i][j] = std::min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
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

std::vector<int> shuffle(int end) {
	std::vector<int> v(end);
	std::random_device rd;
	std::iota(v.begin(), v.end(), 1);  // Fill vector with values from 1 to end
	std::ranges::shuffle(v, rd);       // Shuffle using C++20 ranges
	for (int i = 0; i < end; i++) {
		v[i] = v[i] - 1;  // Adjust values from 1-based to 0-based
	}
	return v;
}

int question_loop(const std::vector<std::string>& elements, const std::vector<std::string>& signs, int i, int n) {
	std::string query;
	int threshold = 23;
	int pointsback = 0;

	std::cout.flush();
	std::cout << "\033[H\033[J";
	std::cout << n << ". " << "Vad heter detta elementet? " << signs[i] << std::endl;
	// std::getline(std::cin >> std::ws, query);

	while (true) {
		std::getline(std::cin, query); // Read a line of input

		// Check if the input is empty
		if (query.empty()) {
			std::cout << "\033[H\033[J";
			std::cout << n << ". " << "Vad heter detta elementet? " << signs[i] << std::endl;
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
		std::cout << "Tar du inte detta seri\xC3\xB6st?" << std::endl;
		std::cin.get();
		return -1;
	}

	switch (distance <= 2) {
	case 1:
		if (query == elements[i]) {  // Add this check
			std::cout << "\u001b[32mYAYYY, du har r\xC3\xA4tt! Har far du 3 poang!\u001b[37m" << std::endl;
			pointsback = 3;
			break;
		}
		std::cout << "\u001b[33mOOHH, n\xC3\xA4stan... har... ta 1 gratis po\xC3\xA4ng!" << std::endl;
		std::cout << "\u001b[32mR\xC3\xA4tt svar var: " << elements[i] << "\u001b[37m" << std::endl;
		pointsback = 1;
		break;
	default:
		std::cout << "\u001b[31mOH NO, tyvarr fel... testa igen.\u001b[37m" << std::endl;
		std::cout << "R\xC3\xA4tt svar var: " << elements[i] << std::endl;
		break;
	}
	std::cout << "Tryck p\xC3\xA5 [ENTER] f\xC3\xB6r att forts\xC3\xA4tta..." << std::endl;
	std::cin.clear();
	std::cin.ignore(10000, '\n');
	std::cout << "\033[H\033[J" << std::endl;

	return pointsback;
}

int question_game_symbols(std::vector<std::string> elements, std::vector<std::string> signs) {
	std::vector<int> shufflist = shuffle(30);
	int question;
	int points = 0;
	int point;

	std::cout << std::endl;

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
	std::cout << "Dina po\xC3\xA4ng: ";
	std::cout << std::fixed << std::setprecision(2) << percent << "%" << std::endl;

	std::cin.get();
	return 0;
}

int main() {
	system("chcp 65001");
	system("@echo off");
	std::vector<std::string> filenames = { "elements.txt", "signs.txt" };
	std::vector<std::string> elements = {"v\xC3\xA4te", "helium", "litium", "beryllium", "bor", "kol", "kv\xC3\xA4ve", "syre", "fluor", "neon", "natrium", "magnesium", "aluminium", "kisel", "fosfor", "svavel", "klor", "argon", "kalium", "kalcium", "skandium", "titan", "vanadin", "krom", "mangan", "j\xC3\xA4rn", "kobolt", "nickel", "koppar", "zink"};
	std::vector<std::string> signs = { "H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn" };

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
			case 0: std::cout << "1. Quiz"; break;
			case 1: std::cout << "2. Kolla Element och Symboler"; break;
			case 2: std::cout << "3. Avsluta"; break;
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
						std::cout << elements[i] << " : " << signs[i] << std::endl;
					}
					std::cout << "Tryck p\xC3\xA5 [ENTER] f\xC3\xB6r att forts\xC3\xA4tta...";
					_getch();
					break;
				case 2:
					return 0;
			}
		}
	}
}