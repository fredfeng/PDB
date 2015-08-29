
#ifndef PDBFEATURE_BUILDER_DESER_C
#define PDBFEATURE_BUILDER_DESER_C

#include <iostream>
#include "FlatFeatureBuilder.h"
#include "PDBFeatureBuilderDeserializationMachine.h"
using namespace std;

PDBFeatureBuilderPtr PDBFeatureBuilderDeserializationMachine :: deSerialize (void *fromHereIn, 
	size_t &numBytes, bool &success) {

	// we are not successful reading in a zero-byte message
	if (numBytes == 0) {
		success = false;
		return nullptr;
	}

        // so we can do pointer arithmatic
        char *fromHere = static_cast<char *> (fromHereIn);

        // read in the type key
        int *code = reinterpret_cast<int *> (fromHere);

        fromHere += sizeof (int);

        // run through all of the registered types, and look for one that matches
        for (PDBFeatureBuilderPtr p : allRegisteredTypes) {

                // we found a correct type
                if (p->getCode () == *code) {
                        PDBFeatureBuilderPtr newObject = p->getEmptyBuilder ();
                        size_t bytesRead;
                        newObject->deSerialize (fromHere, bytesRead);
                        numBytes = bytesRead + sizeof (int);
                        success = true;
                        return newObject;
                }
        }

        // if we got here, deserialization failed because we could not find a type that matched
        success = false;
        return {};
}

PDBFeatureBuilderPtr PDBFeatureBuilderDeserializationMachine :: deSerialize (void *fromHereIn, size_t &numBytes) {

        bool success;
        PDBFeatureBuilderPtr result = deSerialize (fromHereIn, numBytes, success);
        if (success == false) {
                result = make_shared <FlatFeatureBuilder> ();
                result->deSerialize (fromHereIn, numBytes);
        }

        return result;
}

PDBFeatureBuilderDeserializationMachine :: PDBFeatureBuilderDeserializationMachine (vector <PDBFeatureBuilderPtr> &allFeatureBuilders) {
	allRegisteredTypes = allFeatureBuilders;
}

void PDBFeatureBuilderDeserializationMachine :: registerNewFeatureBuilders (PDBFeatureBuilderPtr regMe) {
	allRegisteredTypes.push_back (regMe);
}

#endif
