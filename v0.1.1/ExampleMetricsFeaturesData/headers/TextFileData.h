
#ifndef TEXT_FILE_H
#define TEXT_FILE_H

#include "PDBStoredDataType.h"
#include <vector>
#include <string>
using namespace std;

class TextFileData : public PDBStoredDataType {

private:

	string fName;
	vector <string> lines;

public:

	// create a TextFileData object by reading a file
	TextFileData (string fName);

	// access the vector of lines in the file
	vector <string> getLines ();

	// access the name of the file
	string getFileName ();

	// do-nothing constructor, and destructor
	TextFileData ();
	~TextFileData ();

	// from PDBStoredDataType
	int getCode ();
	string display ();
	string getName ();
	void *serialize (void *toHereIn, size_t &size);
	void *deSerialize (void *toHereIn, size_t &size);
	PDBStoredDataTypePtr getNewStoredDataTypeObject ();
};

#endif
