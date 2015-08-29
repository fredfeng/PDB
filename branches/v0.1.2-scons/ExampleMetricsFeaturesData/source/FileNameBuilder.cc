
#ifndef FILE_NAME_BUILDER_C
#define FILE_NAME_BUILDER_C

#include "FileNameBuilder.h"
#include "TextFeatureType.h"
#include "TextFileData.h"

FileNameBuilder :: FileNameBuilder (const string &myNameIn) {
	myName = myNameIn;
}

PDBFeatureBuilder &FileNameBuilder :: operator = (PDBFeatureBuilderPtr copyMeIn) {
	FileNameBuilder *copyMe = dynamic_cast<FileNameBuilder *> (copyMeIn.get ());
	myName = copyMe->myName;
	return *this;
}

bool FileNameBuilder :: runsOnThisPDBData (PDBData &checkMe) {
	if (checkMe.getData ()->getCode () == 494853243)
		return true;
	else
		return false;
}

PDBFeatureTypePtr FileNameBuilder :: buildFeature (PDBData &fromMe, bool &wasError, string &) {

	wasError = false;
	TextFileData *temp = dynamic_cast<TextFileData *> (fromMe.getData ().get ());
	return make_shared<TextFeatureType> (temp->getFileName ());
}

const string &FileNameBuilder :: getFeatureName () {
	return myName;
}

// these do not do anything... they will only be called in a distributed database
// implementation in order to ship serialized objects around the cluster
void *FileNameBuilder :: serialize(void *toHere, size_t &) {
	return toHere;
}

void *FileNameBuilder :: deSerialize(void *fromHere, size_t &) {
	return fromHere;
}

#endif

