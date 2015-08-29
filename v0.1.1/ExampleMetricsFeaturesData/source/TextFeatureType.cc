
#ifndef TEXT_F_TYPE_C
#define TEXT_F_TYPE_C

#include "TextFeatureType.h"

TextFeatureType :: TextFeatureType () {
	val = "";
}

TextFeatureType :: TextFeatureType (string forMe) {
	val = forMe;
}

TextFeatureType :: ~TextFeatureType () {}

void TextFeatureType :: replaceVal (string useMe) {
	val = useMe;
}

string TextFeatureType :: getVal () {
	return val;
}

string TextFeatureType :: getTypeName () {
	return "TextFeatureType";
}

string TextFeatureType :: display () {
	return val;
}

int TextFeatureType :: getCode () {
	return 34958343;
}

void *TextFeatureType :: serialize (void *toHereIn, size_t &size) {
	char *toHere = (char *) toHereIn;
	for (int i = 0; i < val.length (); i++) {
		toHere[i] = val[i];
	}	
	toHere[val.length ()] = 0;
	size = val.length () + 1;
	return toHere + val.length () + 1;
}

void *TextFeatureType :: deSerialize (void *fromHereIn, size_t &size) {
	char *fromHere = (char *) fromHereIn;
	string temp (fromHere);
	val = temp;
	size = val.length () + 1;
	return fromHere + size;	
}

PDBFeatureTypePtr TextFeatureType :: getNewFeatureTypeObject () {
	PDBFeatureTypePtr returnVal (new TextFeatureType);
	return returnVal;	
}

#endif

