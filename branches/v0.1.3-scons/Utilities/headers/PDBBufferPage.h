
#ifndef PDB_BUFFER_PAGEH
#define PDB_BUFFER_PAGEH

class PDBBufferPool;

#include <memory>
using namespace std;

// create a smart pointer for PDBBufferPagePtr objects
class PDBBufferPage;
typedef shared_ptr <PDBBufferPage> PDBBufferPagePtr;

#include "PDBBufferPool.h"
#include <stdlib.h>

// encapsulates a single buffer page... for now, these is used just for sending/receiving data,
// but eventually it'll be used to actually buffer data in RAM
class PDBBufferPage {

public:

	// gets access to the RAW bytes available here
	char *getRaw ();

	// gets the size of the page
	size_t getSize ();

	// constructs an empty page
	PDBBufferPage ();

	// constructor: allocates the page
	PDBBufferPage (size_t numBytes, PDBBufferPool *parentIn);

	// create using already-created RAM
	PDBBufferPage (char *rawBytes, size_t numBytes, PDBBufferPool *parentIn);

	// destructor: returns the RAM to the parent
	~PDBBufferPage ();

	// free the pages memory forever
	void freePage ();

private:

	// the page's parent, where it goes to return itself for reuse
	PDBBufferPool *parent;
	
	// these are the page's raw bytes
	char *rawBytes;

	// the size
	size_t mySize;
};

#endif

