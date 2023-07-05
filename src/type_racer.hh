#ifndef TYPE_RACER_HH
#define TYPE_RACER_HH

#include <algorithm>
#include <chrono>
#include <cstdlib>  
#include <ctime>    
#include <fstream>
#include <iostream>
#include <limits>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <queue>

// TODO: Implement a way to select difficulty

// #define DICT_FILE "assets/dictionary_easy.txt"
// #define DICT_FILE "assets/dictionary_med.txt"
#define DICT_FILE "assets/dictionary_hard.txt"
extern size_t DICT_SIZE; // Total # of lines/words in DICT_FILE
						 
#define VEC_SIZE 10 // Number of lines that show inlcuding the current sentence

// Window size
#define WIN_H 40
#define WIN_W 100
// Window position
#define WIN_X 2
#define WIN_Y 2
// Text positional offset
#define WIN_TEXT_OFFSET_Y 1
#define WIN_TEXT_OFFSET_X 2
// Statistics tracker offset
#define STATS_TEXT_OFFSET_Y WIN_TEXT_OFFSET_Y+VEC_SIZE+4
// Clock/Timer offset
#define CLOCK_TEXT_OFFSET_Y STATS_TEXT_OFFSET_Y+1
// Title position
#define TITLE_X WIN_W/2-10
#define TITLE_Y 0

#define KEY_ESC 27

namespace Clock {
	extern int hh, mm, ss;
	void timer_start();
	void timer_end();
	void timer_restart();
	void timer_tick();

	void print_timer(WINDOW* w);
	void print_clock(WINDOW* w);
}

namespace TypeRacer {
	typedef enum Status {
		NONE, // Default state
		HIT, // Player correctly hit a key
		MISS, // Player incorrectly hit a key
	} Status;

	extern std::ifstream ifs;

	extern std::vector<std::string> sentence_vec;
	extern std::string sentence;
	extern std::string word;
	extern int i;
	
	// count characters
	extern int total_chars;
	extern int hits;
	
	// count # words
	extern int total_words;
	// words per min
	extern int wpm;
	
	extern Status status;
	
	void init();
	int get_words_line_count();
	void open_words_file();
	void close_words_file();

	// void gen_rand_word();
	void gen_rand_sentence();
	void replace_sentence(); // in sentence_vec
	void validate_input(int input);

}

#endif // TYPE_RACER_HH
