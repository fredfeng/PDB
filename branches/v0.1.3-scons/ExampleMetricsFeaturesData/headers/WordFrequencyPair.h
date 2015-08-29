
#ifndef WORD_FREQ_PAIR_H
#define WORD_FREQ_PAIR_H

#include <string>
#include <utility>


// this silly little struct is used to sort words based on frequency
struct WordFreqPair {

	const std::string *word;
	int count;

	bool operator < (const WordFreqPair &checkMe) const {
		return count > checkMe.count;
	}

	WordFreqPair (const std::pair<const std::string, int> &fromMe)
	:	word (&fromMe.first),
		count (fromMe.second) {
	}

};

#endif

