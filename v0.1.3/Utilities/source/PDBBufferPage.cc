
#ifndef PDB_BUFFER_PAGE_C
#define PDB_BUFFER_PAGE_C

#include "PDBBufferPage.h"

char *PDBBufferPage :: getRaw () {
	return rawBytes;
}

size_t PDBBufferPage :: getSize () {
	return mySize;
}

PDBBufferPage :: PDBBufferPage () {
	parent = nullptr;
	rawBytes = nullptr;
	mySize = 0;
}

PDBBufferPage :: PDBBufferPage (char *rawBytesIn, size_t numBytes, PDBBufferPool *parentIn) {
	mySize = numBytes;
	rawBytes = rawBytesIn;
	parent = parentIn;
}

PDBBufferPage :: PDBBufferPage (size_t numBytes, PDBBufferPool *parentIn) {
	mySize = numBytes;
	rawBytes = (char *) malloc (numBytes);
	parent = parentIn;
}

void PDBBufferPage :: freePage () {
	delete (rawBytes);
	rawBytes = nullptr;
}

PDBBufferPage :: ~PDBBufferPage () {
	if (rawBytes != nullptr)
		parent->returnBytes (rawBytes, mySize);
}

#endif
