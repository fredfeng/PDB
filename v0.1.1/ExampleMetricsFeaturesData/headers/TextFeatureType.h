
#ifndef TEXT_F_TYPE_H
#define TEXT_F_TYPE_H

#include <string>
#include "PDBFeatureType.h"
using namespace std;

// stores a vector in a sparse format
class TextFeatureType : public PDBFeatureType {

public:

	// constructors/destructor
	TextFeatureType ();
	TextFeatureType (string forMe);
	~TextFeatureType ();

	// replace the existing string with a new one
	void replaceVal (string useMe);
	
	// get the string
	string getVal ();
	
	// from the interface
	string getTypeName ();
	int getCode ();
	void *serialize (void *toHere, size_t &size);
	void *deSerialize (void *fromHere, size_t &size);
	PDBFeatureTypePtr getNewFeatureTypeObject ();
	string display ();
	
private:
	
	string val;
};

#endif


