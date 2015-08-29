
#ifndef WORD_FREQ_AGG_C
#define WORD_FREQ_AGG_C

#include "WordFrequencyAggregator.h"
#include "WordFrequencyPair.h"
#include "TextFileData.h"
#include <algorithm>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

int WordFrequencyAggregator :: getCount (string checkMe) {
	if (!sorted)
		getRank ("");

	if ((counter.count (checkMe)) == 0)
		return 0;
	return counter[checkMe];
}

string WordFrequencyAggregator :: getKthMostFrequent (int checkMe) {
	if (!sorted)
		getRank ("");

	if ((reverseRanks.count (checkMe)) == 0)
		return "";
	return reverseRanks[checkMe];
}

int WordFrequencyAggregator :: getRank (string checkMe) {

	// if we are not sorted, then rebuild the ranks vector
	if (!sorted) {

		vector <WordFreqPair> frequencies;
	
		// first, extract the frequencies
		for (auto const &p : counter) {
			WordFreqPair temp (p.first, p.second);
			frequencies.push_back (temp);
		}

		// now sort them
		sort (frequencies.begin (), frequencies.end ());

		// and load them up into the list of ranks
		ranks.clear ();
		reverseRanks.clear ();
		for (int i = 0; i < frequencies.size (); i++) {
			ranks[frequencies[i].getString ()] = i;
			reverseRanks[i] = frequencies[i].getString ();
		}

		// done!
		sorted = true;
	}

	if ((ranks.count (checkMe)) == 0)
		return -1;
	return ranks[checkMe];
}

int WordFrequencyAggregator :: getNumDistinctWords () {
	return counter.size ();
}

WordFrequencyAggregator :: WordFrequencyAggregator () {
	sorted = false;
}

WordFrequencyAggregator :: ~WordFrequencyAggregator () {}

PDBQueryExecutor &WordFrequencyAggregator :: operator += (PDBQueryExecutorPtr rhs) {

	// we have to re-sort
	sorted = false;

	// this guy had better be a WordFrequencyAggregator!!
	WordFrequencyAggregator *other = (WordFrequencyAggregator *) rhs.get ();
	
	// loop through all of his entries
	for (auto const &p : other->counter) {

		// look for this word in the other guy
		if ((counter.count (p.first)) == 0) {
			counter[p.first] = p.second;
		} else {
			counter[p.first] = counter[p.first] + p.second;
		}
	}
	return *this;
}

PDBQueryExecutorPtr WordFrequencyAggregator :: getEmpty () {
	return PDBQueryExecutorPtr (new WordFrequencyAggregator);
}

void WordFrequencyAggregator :: aggregate (PDBData &aggMe) {

	// make sure we are trying to aggregate a TextFileData object
	if (aggMe.getData ()->getCode () != 494853243)
		return;

	// we have to re-sort
	sorted = false;

	// now, loop through all of the lines in the text file
	TextFileData *temp = (TextFileData *) aggMe.getData ().get ();	
	for (string s : temp->getLines ()) {

		// find the first "real" character
		int posLast = s.find_first_not_of (" \t\n");
		if (posLast == string :: npos) 
			continue;

		// loop through the string
		while (true) {

			// find the next space character
			int posNext = s.find_first_of (" <>?(),-';.\"\t\n", posLast);
			if (posNext == string :: npos) {
				posNext = s.size ();
			}

			// now, extract the word
			string word = s.substr (posLast, posNext - posLast);
			transform(word.begin(), word.end(), word.begin(), ::tolower);
			
			// and count it
			if (word.length () >= 1) {
				if ((counter.count (word) == 0)) {
					counter[word] = 1;
				} else {	
					counter[word] = counter[word] + 1;
				}
			}

			// and see if we are done
			if (posNext == s.size ())
				break;
			posLast = s.find_first_not_of (" <>?(),-';.\"\t\n", posNext);
			if (posLast == string :: npos) 
				break;
		}
	}
}

bool WordFrequencyAggregator ::  wasAnError (string &error) {
	// no possible errors at this time!
	return false;
}

string WordFrequencyAggregator :: display () {
	ostringstream buffer;
	buffer << "<";
	int limit = 10;
	if (getNumDistinctWords () < limit)
		limit = getNumDistinctWords ();
	for (int i = 0; i < limit; i++) {
		string word = getKthMostFrequent (i);
		int count = getCount (word);
		buffer << word << ": " << count;
		if (i != limit - 1)
			buffer << ", ";			
	}	
	if (limit < getNumDistinctWords ())
		buffer << "...";
	buffer << ">";
	return buffer.str();
}

// these are unimplemented; will be needed for the distributed version!!
void *WordFrequencyAggregator :: serialize (void *toHere, size_t &numBytes) {
	return toHere;
}

void *WordFrequencyAggregator :: deSerialize (void *fromHere, size_t &numBytes) {
	return fromHere;
}

#endif

