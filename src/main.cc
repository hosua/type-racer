#include "type_racer.hh"
// using namespace TypeRacer;

typedef enum ColorPair {
	CP_BLACK = 1,
	CP_GREEN = 2,
	CP_RED = 3,
} ColorPair;

void init_color_pairs();
void print_stats(WINDOW* w);

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

void print_word_colored_at_i(WINDOW* w, 
		int x, int y, 
		const char* lhs, const char* rhs, // Do not forget to free lhs and rhs
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

void print_stats(WINDOW* w){
	float p = ((float)TypeRacer::hits / (float)TypeRacer::total_chars) * 100;
	mvwprintw(w, STATS_TEXT_OFFSET_Y, WIN_TEXT_OFFSET_X, "Accuracy: %0.2f%%", p);
	// print total key hits/misses
	mvwprintw(w, STATS_TEXT_OFFSET_Y+1, WIN_TEXT_OFFSET_X, 
			"Hits: %i\tMisses: %i\tTotal keystrokes:", TypeRacer::hits, TypeRacer::total_chars-TypeRacer::hits, TypeRacer::total_chars);
	// print WPM and total words typed
	mvwprintw(w, STATS_TEXT_OFFSET_Y+2, WIN_TEXT_OFFSET_X, "WPM: %f\tWords typed: %i", TypeRacer::wpm, TypeRacer::total_words);
}

// Add ability to restart timer/game with some keybindings

int main() {
	srand(time(0)); // seed RNG
	WINDOW *w = nullptr;

	initscr(); // init ncurses
	// wtimeout(w, -1);
	// notimeout(w, TRUE);
	// nodelay(stdscr, TRUE);
	init_color_pairs(); // init colors

	w = newwin(WIN_H, WIN_W, WIN_X, WIN_Y); // create a new window
	box(w, 0, 0); // Draw a border for our window

	wrefresh(w); // update the terminal screen
	noecho(); // disable echoing of characters on the screen
	
	keypad(w, TRUE); // enable keyboard input for the window.
	// ncurses detects ESC a little slowly by default, 
	// this sets the delay to 100ms
	set_escdelay(100); 

	TypeRacer::init();

	curs_set(0); // hide the default screen cursor.
	
	// Print TUI elements for the first frame
	mvwprintw(w, TITLE_Y, TITLE_X, " Type Racer "); 
	print_stats(w);

	ColorPair col_pair = CP_BLACK; // By default, the color pair will be black since the player is
								   // not doing anything yet.
	
	// Initialize hours, mins, and seconds to 0 and set the clock's internal 
	// timer to the time this function gets called. 
	Clock::timer_start(); 
	
	// These variables are used for printing a single char in a string, a different color.
	// TODO: Instead of how we only have the current key as the colored key, we should reimplement
	// it such that the keys stay green/red after the player reaches them. 
	// The player should also be forced to backspace, if they want to correct errors that they made,
	// instead of the cursor just staying in place.
	char *c_str, *lhs, *rhs;
	c_str = lhs = rhs = nullptr;
	
	// populate sentence vector with VEC_SIZE elements
	for (int i = 0; i < VEC_SIZE; i++){
		TypeRacer::gen_rand_sentence();
		TypeRacer::sentence_vec.push_back(TypeRacer::sentence);
	}

	// print first sentence with colored cursor at the beginning
	print_word_colored_at_i(w, WIN_TEXT_OFFSET_X, WIN_TEXT_OFFSET_Y, 
			lhs, rhs,
			TypeRacer::sentence_vec[0], TypeRacer::i, 
			col_pair
		);

	// print the rest of the sentences normally, none need to be colored
	for (int i = 1; i < VEC_SIZE; i++){
		int y = WIN_TEXT_OFFSET_Y + i;
		char* c_str = (char*)TypeRacer::sentence_vec[i].c_str();
		mvwprintw(w, y, WIN_TEXT_OFFSET_X, "%s", c_str); 
	}

	// print random word

	int input;
	std::thread tick_thr(&Clock::timer_tick);
	
	// Disable blocking-mode in wgetch for window w
	nodelay(w, TRUE);
	// while (true){
	while ( (input = wgetch(w)) != KEY_ESC ) {
		TypeRacer::validate_input(input);
		switch (TypeRacer::status){
			case TypeRacer::NONE:
				col_pair = CP_BLACK;
				break;
			case TypeRacer::HIT:
				col_pair = CP_GREEN;
				break;
			case TypeRacer::MISS:
				col_pair = CP_RED;
				break;
		}

		free(lhs);
		free(rhs);
		lhs = rhs = nullptr;
		print_word_colored_at_i(w, 
				WIN_TEXT_OFFSET_X, WIN_TEXT_OFFSET_Y, 
				lhs, rhs,
				TypeRacer::sentence_vec[0], TypeRacer::i, 
				col_pair
			);
		
		size_t len = TypeRacer::sentence_vec[0].length();

		// If cursor is at the end of the sentence
		if (TypeRacer::i == len-1){
			TypeRacer::total_words++;
			TypeRacer::replace_sentence();
			// clear sentences from ncurses
			for (int y = WIN_TEXT_OFFSET_Y; y <= WIN_TEXT_OFFSET_Y+VEC_SIZE; y++){	
				wmove(w, y, 0); 
				wclrtoeol(w);
			}
			// redraw border, since this also gets cleared
			box(w, 0, 0);
			free(lhs);
			free(rhs);
			lhs = rhs = nullptr;
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
		print_stats(w);
		Clock::print_timer(w);
		TypeRacer::calc_wpm();
		// Debugging 
		// mvwprintw(w, 14, WIN_TEXT_OFFSET_X, "dict_size: %zu", DICT_SIZE);
		// mvwprintw(w, 15, WIN_TEXT_OFFSET_X, "%c(%i) %zu", TypeRacer::sentence_vec[0][TypeRacer::i], TypeRacer::i, TypeRacer::sentence_vec[0].length());
		// mvwprintw(w, 16, WIN_TEXT_OFFSET_X, "col_pair: %i", col_pair);
	}
	// Detach thread before exiting
	tick_thr.detach();
	delwin(w);
	endwin();

	TypeRacer::ifs.close();
	return 0;
}


