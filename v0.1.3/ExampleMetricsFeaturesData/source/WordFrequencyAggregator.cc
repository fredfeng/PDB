
#ifndef WORD_FREQ_AGG_C
#define WORD_FREQ_AGG_C

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <string.h>
#include "TextFileData.h"
#include <vector>
#include "WordFrequencyAggregator.h"
#include "WordFrequencyPair.h"
using namespace std;

int WordFrequencyAggregator :: getCount (const string &checkMe) {
	sort ();
	const auto found (counter.find (checkMe));
	return found == counter.end () ? 0 : found->second;
}

const string &WordFrequencyAggregator :: getKthMostFrequent (int checkMe) {
	sort ();
	try {
		return reverseRanks[checkMe];
	} catch (const std::out_of_range &) {
		static const string empty;
		return empty;
	}
}

int WordFrequencyAggregator :: getCode () {
	return 4923423;
}

int WordFrequencyAggregator :: getRank (const string &checkMe) {
	sort ();
	const auto found (ranks.find (checkMe));
	return found == ranks.end () ? -1 : found->second;
}

int WordFrequencyAggregator :: getNumDistinctWords () {
	return counter.size ();
}

WordFrequencyAggregator :: WordFrequencyAggregator () {
	sorted = false;
}

PDBQueryExecutor &WordFrequencyAggregator :: operator += (PDBQueryExecutorPtr rhs) {

	// we have to re-sort
	sorted = false;

	// this guy had better be a WordFrequencyAggregator!!
	WordFrequencyAggregator *other = dynamic_cast<WordFrequencyAggregator *> (rhs.get ());
	
	// loop through all of his entries
	for (auto const &p : other->counter)
		counter[p.first] += p.second;

	return *this;
}

PDBQueryExecutorPtr WordFrequencyAggregator :: getEmpty () {
	return make_shared<WordFrequencyAggregator> ();
}

void WordFrequencyAggregator :: aggregate (PDBData &aggMe) {

	// make sure we are trying to aggregate a TextFileData object
	if (aggMe.getData ()->getCode () != 494853243)
		return;

	// we have to re-sort
	sorted = false;

	// now, loop through all of the lines in the text file
	TextFileData *temp = dynamic_cast<TextFileData *> (aggMe.getData ().get ());
	for (const string &s : temp->getLines ()) {

		// find the first "real" character
		size_t posLast = s.find_first_not_of (" \t\n");
		if (posLast == string :: npos) 
			continue;

		// loop through the string
		while (true) {

			// find the next space character
			size_t posNext = s.find_first_of (" <>?(),-';.\"\t\n", posLast);
			if (posNext == string :: npos) {
				posNext = s.size ();
			}

			// now, extract the word
			string word = s.substr (posLast, posNext - posLast);
			transform(word.begin(), word.end(), word.begin(), ::tolower);
			
			// and count it
			if (!word.empty ())
				++counter[word];

			// and see if we are done
			if (posNext == s.size ())
				break;
			posLast = s.find_first_not_of (" <>?(),-';.\"\t\n", posNext);
			if (posLast == string :: npos) 
				break;
		}
	}
}

bool WordFrequencyAggregator ::  wasAnError (string &) {
	// no possible errors at this time!
	return false;
}

string WordFrequencyAggregator :: display () {
	ostringstream buffer;
	buffer << '<';
	int limit = 10;
	if (getNumDistinctWords () < limit)
		limit = getNumDistinctWords ();
	for (int i = 0; i < limit; i++) {
		const string &word = getKthMostFrequent (i);
		int count = getCount (word);
		buffer << word << ": " << count;
		if (i != limit - 1)
			buffer << ", ";			
	}	
	if (limit < getNumDistinctWords ())
		buffer << "...";
	buffer << '>';
	return buffer.str();
}

// these are unimplemented; will be needed for the distributed version!!
void *WordFrequencyAggregator :: serialize (void *toHere, size_t &numBytes) {
	sort ();	
	size_t first, second, third;
	toHere = writeMap (counter, toHere, first);
	toHere = writeMap (ranks, toHere, second);
	toHere = writeVector (reverseRanks, toHere, third);
	numBytes = first + second + third;
	return toHere;
}

void *WordFrequencyAggregator :: deSerialize (void *fromHere, size_t &numBytes) {
	sorted = true;
	size_t first, second, third;
	fromHere = readMap (counter, fromHere, first);
	fromHere = readMap (ranks, fromHere, second);
	fromHere = readVector (reverseRanks, fromHere, third);
	numBytes = first + second + third;
	return fromHere;
}

void *WordFrequencyAggregator :: readMap (unordered_map <string, int> &myMap, void *fromHereIn, size_t &size) {

        // read in the size
        char *fromHere = static_cast<char *> (fromHereIn);
        int readsize = *(reinterpret_cast<int *> (fromHere));
        fromHere += sizeof (int);

        // now read the slots
        myMap.clear ();
        for (int i = 0; i < readsize; i++) {
		string first (fromHere);
		fromHere += first.size () + 1;
                double second = *(reinterpret_cast<int *> (fromHere));
                fromHere += sizeof (int);
                myMap[first] = second;
        }

        // done!
        size = fromHere - static_cast<char *> (fromHereIn);
        return fromHere;
}

void *WordFrequencyAggregator :: readVector (vector <string> &myVec, void *fromHereIn, size_t &size) {

        // read in the size
        char *fromHere = static_cast<char *> (fromHereIn);
        int readsize = *(reinterpret_cast<int *> (fromHere));
        fromHere += sizeof (int);

        // now read the slots
        myVec.clear ();
        for (int i = 0; i < readsize; i++) {
		string first (fromHere);
		fromHere += first.size () + 1;
		myVec.push_back (first);
        }

        // done!
        size = fromHere - static_cast<char *> (fromHereIn);
        return fromHere;
}

void *WordFrequencyAggregator :: writeMap (unordered_map <string, int> &myMap, void *toHereIn, size_t &size) {

        // first write the size
        char *toHere = static_cast<char *> (toHereIn);
        *(reinterpret_cast<int *> (toHere)) = myMap.size ();
        toHere += sizeof (int);

        // now write the slots
        for (const auto &elem : myMap) {
                memmove (toHere, elem.first.c_str (), elem.first.size () + 1);
                toHere += elem.first.size () + 1;
                *(reinterpret_cast<int *> (toHere)) = elem.second;
                toHere += sizeof (int);
        }

	size = toHere - static_cast<char *> (toHereIn);
	return toHere;
}

void *WordFrequencyAggregator :: writeVector (vector <string> &myVec, void *toHereIn, size_t &size) {

        // first write the size
        char *toHere = static_cast<char *> (toHereIn);
        *(reinterpret_cast<int *> (toHere)) = myVec.size ();
        toHere += sizeof (int);

        // now write the slots
        for (const auto &elem : myVec) {
                memmove (toHere, elem.c_str (), elem.size () + 1);
                toHere += elem.size () + 1;
        }

	size = toHere - static_cast<char *> (toHereIn);
	return toHere;
}


void WordFrequencyAggregator :: sort () {

	// if already sorted, nothing more needed
	if (sorted) return;

	// not sorted, so rebuild the rank map and reverse-rank vector

	// first, sort by frequency in a temporary work area
	vector <WordFreqPair> frequencies (begin (counter), end (counter));
	std::sort (frequencies.begin (), frequencies.end ());

	// now load them up into the list of ranks
	ranks.clear ();
	ranks.reserve (frequencies.size ());
	reverseRanks.clear ();
	reverseRanks.reserve (frequencies.size ());
	for (size_t rank = 0; rank < frequencies.size (); ++rank) {
		const string &word {*frequencies.at (rank).word};
		ranks.emplace (word, rank);
		reverseRanks.push_back (word);
	}

	// done!
	sorted = true;
}

#endif

