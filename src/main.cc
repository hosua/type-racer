#include <ncurses.h>
#include "type_racer.hh"
#include <cstdlib>  //required for rand(), srand()
#include <ctime>    //required for time()

int main() {
	srand(time(0));

	WINDOW *w;
	int ch, i = 0, width = 7;
	initscr(); // initialize Ncurses
	w = newwin( 6, 30, 1, 1 ); // create a new window
	box( w, 0, 0 ); // sets default borders for the window
					// now print all the menu items and highlight the first one
	wrefresh( w ); // update the terminal screen
	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad( w, TRUE ); // enable keyboard input for the window.
	curs_set( 0 ); // hide the default screen cursor.
				   // get the input

	TypeRacer::open_words_file();

	mvwprintw( w, i+1, 2, "Type Racer"); 
	// mvwprintw( w, i+3, 2, "Press q to quit"); 

	TypeRacer::word_i = 0;
	const char* c_str = "\0";
	char input = 0;
	TypeRacer::get_rand_word();
	c_str = TypeRacer::word.c_str();
	// print random word
	mvwprintw( w, i+2, 2, "%s", c_str);

	while (true) {
		input = wgetch(w);
		size_t len = TypeRacer::word.length();
	
		TypeRacer::validate_word(input);
		// print cursor under word
		wmove(w, i+2, 0); 
		if (TypeRacer::word_i){
			// clear line
			wmove(w, i+3, 0); 
			wclrtoeol(w);
			// redraw border
			box(w, 0, 0);
			mvwprintw(w, i+3, TypeRacer::word_i+1, "^");
		}
		// std::cout << TypeRacer::word << " " << TypeRacer::word_i << "\n";
		if (TypeRacer::word_i == len){
			// clear previous word
			wmove(w, i+2, 0); 
			wclrtoeol(w);
			// clear carrot (^)
			wmove(w, i+3, 0); 
			wclrtoeol(w);
			// redraw border
			box(w, 0, 0);
			TypeRacer::get_rand_word();
			c_str = TypeRacer::word.c_str();
			// print random word
			mvwprintw(w, i+2, 2, "%s", c_str);
		}
		
		float p = ((float)TypeRacer::hits / (float)TypeRacer::total) * 100;
		mvwprintw(w, i+4, 2, "Accuracy: %0.2f%%", p);
	}
	delwin( w );
	endwin();

	TypeRacer::close_words_file();
}


