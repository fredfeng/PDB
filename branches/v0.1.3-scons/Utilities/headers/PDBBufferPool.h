
#ifndef PDB_BUFFER_POOLH
#define PDB_BUFFER_POOLH

#include <memory>
using namespace std;

// create a smart pointer for PDBBufferPool objects
class PDBBufferPool;
typedef shared_ptr <PDBBufferPool> PDBBufferPoolPtr;

#include "PDBBufferPage.h"
#include <pthread.h>
#include <vector>

using namespace std;

// this encapsulates a pool of bufer pages, pre-allocated and ready for use
class PDBBufferPool {

public:

	// gets a page from the pool; block if no pages are available
	PDBBufferPagePtr get ();

	// creates a pool, with the given number of pages of the given size
	PDBBufferPool (int numPagesIn, size_t memPerPageIn);

	// free all RAM
	~PDBBufferPool ();

	// used to return RAM to the pool
	void returnBytes (char *rawBytes, size_t numBytes);

private:

	// allow concurrent access to the pool
	pthread_mutex_t waitingMutex;
	pthread_cond_t waitingSignal;

	vector <PDBBufferPagePtr> myPages;
};

#endif

