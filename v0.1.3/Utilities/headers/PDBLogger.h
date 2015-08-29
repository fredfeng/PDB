
#ifndef PDB_LOGGER_H
#define PDB_LOGGER_H

#include <memory>
using namespace std;

// create a smart pointer for PDBLogger objects
class PDBLogger;
typedef shared_ptr <PDBLogger> PDBLoggerPtr;

#include <pthread.h>


// used to log client and server activity to a text file
class PDBLogger {

public:

	// opens up a logger; output is written to the specified file
	PDBLogger (string fName);

	// opens up the logger
	void open (string fName);

	// empty logger
	PDBLogger ();

	// closes the text file
	~PDBLogger ();	

	// writes a line of text to the log file
	void writeLn (string writeMe);

private:

	// prohibits two people from writing to the file at the same time
	pthread_mutex_t fileLock;

	// the location we are writing to
	FILE *outputFile;

};

#endif
