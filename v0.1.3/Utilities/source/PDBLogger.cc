
#ifndef PDB_LOGGER_C
#define PDB_LOGGER_C

#include <iostream>
#include "LockGuard.h"
#include "PDBLogger.h"
#include <stdio.h>

using namespace std;

PDBLogger :: PDBLogger (string fName) {
	outputFile = fopen (fName.c_str (), "a");
	if (outputFile == nullptr) {
		cout << "Unable to open logging file.\n";
		perror (nullptr);
		exit (-1);
	}
	pthread_mutex_init (&fileLock, nullptr);
}

void PDBLogger :: open (string fName) {
	const LockGuard guard {fileLock};
	if (outputFile != nullptr) {
		fclose (outputFile);
	}
	outputFile = fopen (fName.c_str (), "a");
	if (outputFile == nullptr) {
		cout << "Unable to open logging file.\n";
		perror (nullptr);
		exit (-1);
	}
}

PDBLogger :: PDBLogger () {
	pthread_mutex_init (&fileLock, nullptr);
}

PDBLogger :: ~PDBLogger () {
	fclose (outputFile);
	pthread_mutex_destroy (&fileLock);
}

void PDBLogger :: writeLn (string writeMe) {
	const LockGuard guard {fileLock};
	if (writeMe[writeMe.length () - 1] != '\n') {
		fprintf (outputFile, "%s\n", writeMe.c_str ());
	} else {
		fprintf (outputFile, "%s", writeMe.c_str ());
	}
	fflush (outputFile);
}

#endif
