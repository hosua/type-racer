#include "type_racer.hh"

#define SENTENCE_LEN 8

size_t DICT_SIZE = 0;

namespace TypeRacer {
	int i = 0;

	std::vector<std::string> sentence_vec;
	std::string sentence;
	std::string word;

	std::ifstream ifs;

	bool terminated = false;

	int total = 0;
	int hits = 0;

	Status status = NONE;

	void open_words_file(){
		ifs.open(DICT_FILE);
	}

	void close_words_file(){
		ifs.close();
	}

	int get_words_line_count(){
		int line_count = 0;
		ifs.open(DICT_FILE);
		std::string line;
		while(getline(ifs, line)){
			line_count++;
		}
		ifs.close();
		DICT_SIZE = line_count-1;
		return DICT_SIZE;
	}

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
		total++;
		char ch = sentence_vec[0][i];
		if (input == ch){
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
