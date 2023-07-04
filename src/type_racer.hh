#ifndef TYPE_RACER_HH
#define TYPE_RACER_HH

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <queue>

#define DICT_FILE "assets/dictionary.txt"
extern size_t DICT_SIZE; // Total # of lines/words in DICT_FILE

#define VEC_SIZE 10

#define CURSOR_Y 3

namespace TypeRacer {
	typedef enum Status {
		NONE,
		HIT,
		MISS,
	} Status;

	extern bool terminated;
	extern std::ifstream ifs;

	extern std::vector<std::string> sentence_vec;
	extern std::string sentence;
	extern std::string word;
	extern int i;

	extern int total;
	extern int hits;

	extern Status status;
	
	int get_words_line_count();
	void open_words_file();
	void close_words_file();

	// void gen_rand_word();
	void gen_rand_sentence();
	void replace_sentence(); // in sentence_vec
	void validate_input(int input);

}

#endif // TYPE_RACER_HH
