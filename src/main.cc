#include "type_racer.hh"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>  
#include <ctime>    


#define WIN_TEXT_OFFSET_Y 1
#define WIN_TEXT_OFFSET_X 2
#define WIN_H 40
#define WIN_W 120

#define STATS_TEXT_OFFSET_Y WIN_TEXT_OFFSET_Y+VEC_SIZE+4

#define TITLE_X WIN_W/2-10
#define TITLE_Y 0

#define WIN_X 2
#define WIN_Y 2

typedef enum ColorPair {
	CP_BLACK = 1,
	CP_GREEN = 2,
	CP_RED = 3,
} ColorPair;

void init_color_pairs();

// prints a string to ncurses with a color at the specific index i
// at position (x,y) in window w.
// lhs and rhs pointers exist in the caller because we need to preserve 
// the memory from the c_strings outside of the function lifetime.
// Do not forget to free lhs and rhs.
void print_word_colored_at_i(WINDOW* w, 
		int x, int y, 
		const char* lhs, const char* rhs,
		std::string str, int i, 
		ColorPair col_pair
);

void init_color_pairs(){
	start_color(); // must be called before initializing colors
	init_pair(CP_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(CP_GREEN, COLOR_WHITE, COLOR_GREEN);
	init_pair(CP_RED, COLOR_WHITE, COLOR_RED);
}

// Do not forget to free lhs and rhs
void print_word_colored_at_i(WINDOW* w, 
		int x, int y, 
		const char* lhs, const char* rhs,
		std::string str, int i, 
		ColorPair col_pair
	){
	wmove(w, y, x);
	lhs = strdup(str.substr(0, i).c_str());
	char mid = str[i];
	rhs = strdup(str.substr(i+1).c_str());
	wprintw(w, "%s", lhs);
	wattron(w, COLOR_PAIR(col_pair));
	wprintw(w, "%c", mid);
	wattron(w, COLOR_PAIR(CP_BLACK));
	wprintw(w, "%s", rhs);
}

int main() {
	srand(time(0));

	WINDOW *w = nullptr;
	initscr(); // init ncurses
	init_color_pairs(); // init colors
	TypeRacer::get_words_line_count();
	int input;
				   
	w = newwin(WIN_H, WIN_W, WIN_X, WIN_Y); // create a new window
	box(w, 0, 0); // sets default borders for the window
					// now print all the menu items and highlight the first one
	wrefresh(w); // update the terminal screen
	noecho(); // disable echoing of characters on the screen
	keypad(w, TRUE); // enable keyboard input for the window.
	curs_set(0); // hide the default screen cursor.

	TypeRacer::open_words_file();
	
	mvwprintw(w, TITLE_Y, TITLE_X, "Type Racer"); 
	// mvwprintw( w, WIN_TEXT_OFFSET_Y+3, 2, "Press q to quit"); 

	TypeRacer::i = 0;
	input = 0;
	TypeRacer::status = TypeRacer::HIT;

	char *c_str, *lhs, *rhs;
	c_str = lhs = rhs = nullptr;
	
	// populate sentence vec
	for (int i = 0; i < VEC_SIZE; i++){
		TypeRacer::gen_rand_sentence();
		TypeRacer::sentence_vec.push_back(TypeRacer::sentence);
	}

	// print first sentence with colored cursor at the beginning
	// c_str = TypeRacer::sentence_vec[0].c_str();
	// mvwprintw(w, WIN_TEXT_OFFSET_Y, WIN_TEXT_OFFSET_X, "%s", c_str); 
	print_word_colored_at_i(w, WIN_TEXT_OFFSET_X, WIN_TEXT_OFFSET_Y, 
			lhs, rhs,
			TypeRacer::sentence_vec[0], TypeRacer::i, 
			CP_GREEN
		);

	// print the rest of the sentences normally, none need to be colored
	for (int i = 1; i < VEC_SIZE; i++){
		int y = WIN_TEXT_OFFSET_Y + i;
		char* c_str = (char*)TypeRacer::sentence_vec[i].c_str();
		mvwprintw(w, y, WIN_TEXT_OFFSET_X, "%s", c_str); 
	}

	// print random word

	while (true) {
		input = wgetch(w);
	
		TypeRacer::validate_input(input);
		TypeRacer::Status tr_status = TypeRacer::status;
		ColorPair col_pair = (tr_status == TypeRacer::Status::MISS) ? CP_RED : CP_GREEN;

		free(lhs);
		free(rhs);
		print_word_colored_at_i(w, 
				WIN_TEXT_OFFSET_X, WIN_TEXT_OFFSET_Y, 
				lhs, rhs,
				TypeRacer::sentence_vec[0], TypeRacer::i, 
				col_pair
			);
		
		size_t len = TypeRacer::sentence_vec[0].length();

		// If cursor is at the end of the sentence
		if (TypeRacer::i == len-1){
			TypeRacer::replace_sentence();
			// clear sentences from ncurses
			for (int y = WIN_TEXT_OFFSET_Y; y <= WIN_TEXT_OFFSET_Y+VEC_SIZE; y++){	
				wmove(w, y, 0); 
				wclrtoeol(w);
			}
			// redraw border, since this also gets cleared
			box(w, 0, 0);

			print_word_colored_at_i(w, 
					WIN_TEXT_OFFSET_X, WIN_TEXT_OFFSET_Y, 
					lhs, rhs,
					TypeRacer::sentence_vec[0], TypeRacer::i, 
					col_pair
				);

			// print the rest of the sentences normally, none need to be colored
			for (int i = 1; i < VEC_SIZE; i++){
				int y = WIN_TEXT_OFFSET_Y + i;
				c_str = (char*)TypeRacer::sentence_vec[i].c_str();
				mvwprintw(w, y, WIN_TEXT_OFFSET_X, "%s", c_str); 
			}
		}
		

		float p = ((float)TypeRacer::hits / (float)TypeRacer::total) * 100;
		mvwprintw(w, STATS_TEXT_OFFSET_Y, WIN_TEXT_OFFSET_X, "Accuracy: %0.2f%%", p);

		// Debugging 
		// mvwprintw(w, 14, WIN_TEXT_OFFSET_X, "dict_size: %zu", DICT_SIZE);
		// mvwprintw(w, 15, WIN_TEXT_OFFSET_X, "%c(%i) %zu", TypeRacer::sentence_vec[0][TypeRacer::i], TypeRacer::i, TypeRacer::sentence_vec[0].length());
		// mvwprintw(w, 16, WIN_TEXT_OFFSET_X, "col_pair: %i", col_pair);
	}
	delwin(w);
	endwin();

	TypeRacer::close_words_file();
}


