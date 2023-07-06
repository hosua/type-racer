#include "type_racer.hh"
// using namespace tr;

namespace tr = TypeRacer;

void init_color_pairs();
void print_stats(WINDOW* w);
void print_debug_vars(WINDOW* w);

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
	//		  symbol		 FG			  BG
	init_pair(CP_WHITEBLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(CP_GREENBLACK, COLOR_GREEN, COLOR_BLACK);
	init_pair(CP_REDBLACK,   COLOR_RED,   COLOR_BLACK);
	init_pair(CP_BLACKGREEN, COLOR_BLACK, COLOR_GREEN);
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
	if (mid == ' ')
		wattron(w, COLOR_PAIR(CP_BLACKGREEN));
	wprintw(w, "%c", mid);
	wattron(w, COLOR_PAIR(CP_WHITEBLACK));
	wprintw(w, "%s", rhs);
}

void print_stats(WINDOW* w){
	if (tr::total_chars > 0){
		float acc = ((float)tr::hits / (float)tr::total_chars) * 100;
		mvwprintw(w, STATS_TEXT_OFFSET_Y, WIN_TEXT_OFFSET_X, "Accuracy: %0.2f%%", acc);
	} else {
		mvwprintw(w, STATS_TEXT_OFFSET_Y, WIN_TEXT_OFFSET_X, "Accuracy: %0.2f%%", 0.0);
	}

	// print total key hits/misses
	mvwprintw(w, STATS_TEXT_OFFSET_Y+1, WIN_TEXT_OFFSET_X, 
			"Hits: %i\tMisses: %i\tTotal keystrokes: %i", tr::hits, tr::total_chars-tr::hits, tr::total_chars);
	// print WPM and total words typed
	mvwprintw(w, STATS_TEXT_OFFSET_Y+2, WIN_TEXT_OFFSET_X, "WPM: %f\tWords: %i", tr::wpm, tr::total_words);
}

void clear_stats(WINDOW* w){
	for (int y = STATS_TEXT_OFFSET_Y; y < STATS_TEXT_OFFSET_Y+4; y++){
		wmove(w, y, 0); 
		wclrtoeol(w);
	}
}

void print_debug_vars(WINDOW* w){
	mvwprintw(w, DEBUG_OFFSET_Y, WIN_TEXT_OFFSET_X, "Debug variables");
	int secs = Clock::get_time_elapsed();
	// wpm = (double)total_words/((double)mins);
	mvwprintw(w, DEBUG_OFFSET_Y+1, WIN_TEXT_OFFSET_X, "secs: %i", secs);
	mvwprintw(w, DEBUG_OFFSET_Y+2, WIN_TEXT_OFFSET_X, "total_words: %i", tr::total_words);

	// mvwprintw(w, DEBUG_OFFSET_Y+1, WIN_TEXT_OFFSET_X, "dict_size: %zu", DICT_SIZE);
	// mvwprintw(w, DEBUG_OFFSET_Y+2, WIN_TEXT_OFFSET_X, "%c(%i) %zu", tr::sentence_vec[0][tr::cursor], tr::cursor, tr::sentence_vec[0].length());
	// mvwprintw(w, DEBUG_OFFSET_Y+3, WIN_TEXT_OFFSET_X, "col_pair: %i", col_pair);
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

	tr::init();

	curs_set(0); // hide the default screen cursor.
	
	// Print TUI elements for the first frame
	mvwprintw(w, TITLE_Y, TITLE_X, " Type Racer "); 
	// print_stats(w);

	ColorPair col_pair = CP_WHITEBLACK; // By default, the color pair will be black since the player is
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
		tr::gen_rand_sentence();
		tr::sentence_vec.push_back(tr::sentence);
	}


	// print the rest of the sentences normally, none need to be colored
	for (int i = 0; i < VEC_SIZE; i++){
		int y = WIN_TEXT_OFFSET_Y + i;
		char* c_str = (char*)tr::sentence_vec[i].c_str();
		mvwprintw(w, y, WIN_TEXT_OFFSET_X, "%s", c_str); 
	}

	int input;
	std::thread tick_thr(&Clock::timer_tick);
	std::thread wpm_thr(&tr::calc_wpm);
		
	// Disable blocking-mode in wgetch for window w
	nodelay(w, TRUE);
	// while (true){
	while ( (input = wgetch(w)) != KEY_ESC ) {
		tr::validate_input(input);
		switch (tr::status){
			case tr::NONE:
				col_pair = CP_WHITEBLACK;
				break;
			case tr::HIT:
				col_pair = CP_GREENBLACK;
				break;
			case tr::MISS:
				col_pair = CP_REDBLACK;
				break;
		}

		free(lhs);
		free(rhs);
		lhs = rhs = nullptr;
		print_word_colored_at_i(w, 
				WIN_TEXT_OFFSET_X, WIN_TEXT_OFFSET_Y, 
				lhs, rhs,
				tr::sentence_vec[0], tr::cursor, 
				col_pair
			);
		clear_stats(w);	
		box(w, 0, 0);
		print_stats(w);
		size_t len = tr::sentence_vec[0].length();

		// If cursor is at the end of the sentence
		if (tr::cursor == len-1){
			tr::total_words++;
			tr::replace_sentence();
			// clear sentences from the screen 
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
					tr::sentence_vec[0], tr::cursor, 
					col_pair
				);

			// print the rest of the sentences normally, none need to be colored
			for (int i = 1; i < VEC_SIZE; i++){
				int y = WIN_TEXT_OFFSET_Y + i;
				c_str = (char*)tr::sentence_vec[i].c_str();
				mvwprintw(w, y, WIN_TEXT_OFFSET_X, "%s", c_str); 
			}
		}
		Clock::print_timer(w);
		// print_debug_vars(w);
	}

	// Detach threads before exiting
	tick_thr.detach();
	wpm_thr.detach();

	delwin(w);
	endwin();
	// close dictionary file
	tr::ifs.close();
	return 0;
}


