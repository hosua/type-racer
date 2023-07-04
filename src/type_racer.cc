#include "type_racer.hh"

namespace TypeRacer {
	int word_i = 0;
	std::string word;
	std::ifstream ifs;
	bool terminated = false;
	int total = 0;
	int hits = 0;

	void open_words_file(){
		ifs.open(WORDS_FILE);
	}

	void close_words_file(){
		ifs.close();
	}


	void get_rand_word(){
		word_i = 0;
		int n = rand() % WORDS_LINE_COUNT;
		// Move file pointer to start
		ifs.seekg(std::ios::beg);
		// Go to nth line
		for (int i = 0; i < n; i++)
			ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		// Store nth line to word
		ifs >> word;
	}

	void validate_word(int input){
		total++;
		char ch = word[word_i];
		if (input == ch){
			hits++;
			word_i++;
		} 
	}
}
