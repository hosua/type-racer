#include <string>
#include <iostream>
#include <fstream>
#include <limits>

#define WORDS_FILE "assets/words.txt"
#define WORDS_LINE_COUNT 10000

#define CURSOR_Y 3

namespace TypeRacer {
	extern bool terminated;
	extern std::ifstream ifs;

	extern std::string word;
	extern int word_i;
	extern int hits;
	extern int total;

	void open_words_file();
	void close_words_file();

	void get_rand_word();
	void validate_word(int input);
}
