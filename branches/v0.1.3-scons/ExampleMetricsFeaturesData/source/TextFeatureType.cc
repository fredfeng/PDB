
#ifndef TEXT_F_TYPE_C
#define TEXT_F_TYPE_C

#include "TextFeatureType.h"


TextFeatureType :: TextFeatureType (const string &forMe) {
	val = forMe;
}

void TextFeatureType :: replaceVal (const string &useMe) {
	val = useMe;
}

const string &TextFeatureType :: getVal () {
	return val;
}

const string &TextFeatureType :: getTypeName () {
	static const string result {"TextFeatureType"};
	return result;
}

string TextFeatureType :: display () {
	return val;
}

int TextFeatureType :: getCode () {
	return 34958343;
}

void *TextFeatureType :: serialize (void *toHereIn, size_t &size) {
	char *toHere = static_cast<char *> (toHereIn);
	for (size_t i = 0; i < val.length (); i++) {
		toHere[i] = val[i];
	}	
	toHere[val.length ()] = 0;
	size = val.length () + 1;
	return toHere + val.length () + 1;
}

void *TextFeatureType :: deSerialize (void *fromHereIn, size_t &size) {
	char *fromHere = static_cast<char *> (fromHereIn);
	val = fromHere;
	size = val.length () + 1;
	return fromHere + size;	
}

PDBFeatureTypePtr TextFeatureType :: getNewFeatureTypeObject () {
	return make_shared<TextFeatureType> ();
}

#endif

