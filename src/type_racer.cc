#include "type_racer.hh"
#include <chrono>

#define SENTENCE_LEN 8

size_t DICT_SIZE = 0;

namespace Clock {
	using namespace std::chrono;

	std::chrono::time_point<std::chrono::system_clock> start_time;
	std::chrono::time_point<std::chrono::system_clock> end_time;
	bool terminated = false;

	int hh, mm, ss, ms;
	
	// returns minutes elapsed since timer started
	double get_time_elapsed(){ 
		end_time = std::chrono::system_clock::now();
		std::chrono::duration<long, std::ratio<1, 1000>> delta = 
			std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time);
		return duration_cast<minutes>(delta).count();
	}

	void timer_start(){
		terminated = hh = mm = ss = ms = 0;
		// Initialize the start time so we have a frame of reference
		start_time = std::chrono::system_clock::now();
	}

	void timer_stop(){
		terminated = true;
		end_time = std::chrono::system_clock::now();
	}

	void timer_restart(){
		timer_stop();
		timer_start();
	}

	void timer_tick(){
		while (!terminated){
			end_time = std::chrono::system_clock::now();
			std::chrono::duration<long, std::ratio<1, 1000>> delta = 
				std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time);

			hh = duration_cast<hours>(delta).count();
			mm = duration_cast<minutes>(delta).count() - (hh*60);
			ss = duration_cast<seconds>(delta).count() - (hh*60*60) - (mm*60);
			ms = duration_cast<milliseconds>(delta).count() - (hh*60*60*1000) - (mm*60*1000) - (ss*1000);

			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}
	}

	void print_timer(WINDOW* w){
		mvwprintw(w, CLOCK_TEXT_OFFSET_Y+2, WIN_TEXT_OFFSET_X, "Timer: %02i:%02i:%02i:%03i", Clock::hh, Clock::mm, Clock::ss, Clock::ms);
	}

	// TODO: implement clock
	void print_clock(WINDOW* w){
	}
}

namespace TypeRacer {
	int i = 0;

	std::vector<std::string> sentence_vec;
	std::string sentence;
	std::string word;

	std::ifstream ifs;

	bool terminated = false;

	int total_chars = 0;
	int hits = 0;

	int total_words = 0;
	double wpm = 0;

	Status status = NONE;

	void init(){
		// count # of words in our dictionary_*.txt file,
		// and store it in the TypeRacer namespace
		TypeRacer::get_words_line_count(); 
		TypeRacer::ifs.open(DICT_FILE); // Open our dictionary text file to generate random words
	
		TypeRacer::i = 0;
		TypeRacer::status = TypeRacer::NONE; // TypeRacer should initially be in a neutral state, 
										 	 // before the user hits any keys.
	}

	double calc_wpm(){
		double total_mins = Clock::get_time_elapsed();
		wpm = (double)total_words/total_mins;
		return wpm;
	}

	int get_words_line_count(){
		int line_count = 0;
		ifs.open(DICT_FILE);
		std::string line;
		while(getline(ifs, line))
			line_count++;
		ifs.close();
		DICT_SIZE = line_count-1;
		return DICT_SIZE;
	}

	// Helper for gen_rand_word()
	void gen_rand_word(){
		i = 0;
		int n = rand() % DICT_SIZE;
		// Move file pointer to start
		ifs.seekg(std::ios::beg);
		// Go to nth line
		for (int i = 0; i < n; i++)
			ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		// Store nth line to word
		ifs >> word;
	}

	void gen_rand_sentence(){
		sentence.clear();
		for (int ii = 0; ii < SENTENCE_LEN; ii++){
			gen_rand_word();
			sentence += word + " ";
		}
		i = 0;
	}

	void validate_input(int input){
		if (input == ERR)
			return;
		total_chars++;
		char ch = sentence_vec[0][i];
		if (input == ch){
			if (isspace(ch))
				TypeRacer::total_words += 1;
			hits++;
			i++;
			status = HIT;
		} else {
			status = MISS;
		}
	}

	void replace_sentence(){
		// dequeue old sentence
		TypeRacer::sentence_vec.erase(TypeRacer::sentence_vec.begin());
		// generate new sentence
		TypeRacer::gen_rand_sentence();
		// enqueue new sentence
		TypeRacer::sentence_vec.push_back(TypeRacer::sentence);
	}
}
