
#ifndef TEXT_FILE_C
#define TEXT_FILE_C

#include <iostream>
#include <fstream>
using namespace std;

#include "TextFileData.h"

TextFileData :: TextFileData (string fNameIn) {
	fName = fNameIn;

	// now read in the file
	string line;
	ifstream myfile (fName);
	if (myfile.is_open()) {
		while (getline (myfile, line)) {
			lines.push_back (line);
		}
		myfile.close();
	} else {
		cerr << "Warning: unable to open file '" << fNameIn << "'\n";
	}
}

string TextFileData :: getFileName () {
	return fName;	
}

TextFileData :: TextFileData () {}
TextFileData :: ~TextFileData () {}

void *TextFileData :: serialize (void *toHereIn, size_t &size) {

	// write the text file name
	char *toHere = (char *) toHereIn;

	// copy the characters over
	fName.copy (toHere, fName.size (), 0);
	toHere[fName.size ()] = 0;
	toHere += fName.size () + 1;

	// and now write the individual lines
	*((int *) toHere) = lines.size ();
	toHere += sizeof (int);
	for (string s : lines) {
		s.copy (toHere, s.size (), 0);
		toHere[s.size ()] = 0;
		toHere += s.size () + 1;
	}

	// done!!
	size = toHere - (char *) toHereIn;
	return toHere;
}

void *TextFileData :: deSerialize (void *fromHereIn, size_t &size) {

	// get the text file name
	char *fromHere = (char *) fromHereIn;
	string hisName (fromHere);
	fName = hisName;
	fromHere += fName.size () + 1;
	
	// now, read in all of the lines
	int numLines = *((int *) fromHere);
	fromHere += sizeof (int);

	lines.clear ();
	for (int i = 0; i < numLines; i++) {
		string nextLine (fromHere);
		fromHere += nextLine.size () + 1;
		lines.push_back (nextLine);
	}

	// done!!
	size = fromHere - (char *) fromHereIn;
	return fromHere;
}

int TextFileData :: getCode () {
	return 494853243;
}

string TextFileData :: getName () {
	return "TextFileData";
}

vector <string> TextFileData :: getLines () {
	vector <string> retVal = lines;
	return retVal;
}

PDBStoredDataTypePtr TextFileData :: getNewStoredDataTypeObject () {
	PDBStoredDataTypePtr returnVal (new TextFileData);
	return returnVal;
}

string TextFileData :: display () {
	if (lines.size () == 0) {
		return fName + ": <empty>";
	} else if (lines.size () == 1) {
		return fName + ": '" + lines[0] + "'";
	} else {
		return fName + ": '" + lines[0] + "...'";
	}
}

#endif
