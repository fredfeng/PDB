
#ifndef FILE_NAME_BUILDER_C
#define FILE_NAME_BUILDER_C

#include "FileNameBuilder.h"
#include "TextFileData.h"
#include "TextFeatureType.h"

FileNameBuilder :: FileNameBuilder (string myNameIn) {
	myName = myNameIn;
}

PDBFeatureBuilder &FileNameBuilder :: operator = (PDBFeatureBuilderPtr copyMeIn) {
	FileNameBuilder *copyMe = (FileNameBuilder *) copyMeIn.get ();
	myName = copyMe->myName;
	return *this;
}

bool FileNameBuilder :: runsOnThisPDBData (PDBData &checkMe) {
	if (checkMe.getData ()->getCode () == 494853243)
		return true;
	else
		return false;
}

PDBFeatureTypePtr FileNameBuilder :: buildFeature (PDBData &fromMe) {

	TextFileData *temp = (TextFileData *) fromMe.getData ().get ();
	PDBFeatureTypePtr retVal (new TextFeatureType (temp->getFileName ()));
	return retVal;
}

string FileNameBuilder :: getFeatureName () {
	return myName;
}

// these do not do anything... they will only be called in a distributed database
// implementation in order to ship serialized objects around the cluster
void *FileNameBuilder :: serialize(void *toHere, size_t &size) {
	return toHere;
}

void *FileNameBuilder :: deSerialize(void *fromHere, size_t &size) {
	return fromHere;
}

#endif

