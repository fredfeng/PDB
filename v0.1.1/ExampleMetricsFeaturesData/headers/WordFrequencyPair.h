
#ifndef WORD_FREQ_PAIR_H
#define WORD_FREQ_PAIR_H

// this silly little struct is used to sort words based on frequency
struct WordFreqPair {

	string word;
	int count;

	bool operator < (const WordFreqPair &checkMe) const {
		return count > checkMe.count;
	}

	WordFreqPair (string wordIn, int countIn) {
		word = wordIn;
		count = countIn;
	}

	WordFreqPair (const WordFreqPair &fromMe) {
		word = fromMe.word;
		count = fromMe.count;
	}

	WordFreqPair &operator = (const WordFreqPair &fromMe) {
		word = fromMe.word;
		count = fromMe.count;
		return *this;
	}

	string getString () {
		return word;
	}

};

#endif

