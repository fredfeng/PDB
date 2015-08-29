
#ifndef SERVER_MAIN_CC
#define SERVER_MAIN_CC

#include <iostream>
#include "PDBBackEnd.h"
#include "PDBFrontEnd.h"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main (int numArgs, const char *args []) {

	if (numArgs != 9) {
		cout << "usage: ./PDBServer catalog port fileForLocalIPC serverNumThreads numDatabaseThreeads ";
		cout << "RAMperThread ";
		cout << "frontendLogFile backEndLogFile\n";
		return (0);
	}

	string catalogFile (args[1]);
	int port = atoi (args[2]);
	string IPCfile (args[3]);
	int serverNumConnections = atoi (args[4]);
	int numDatabaseThreads = atoi (args[5]);
	int serverMemoryPerConnection = atoi (args[6]);
	string frontendLogFile (args[7]);
	string backendLogFile (args[8]);
	
	// start by foking off a server frontend
	pid_t pid = fork ();
	
	if (pid < 0) {
		cout << "Error forking process to start up the frontend server.\n";
		perror ("Error was: ");
		return (0);
	}

	// see if we are in the child process (this will be the server frontend)
	if (pid == 0) {

		// start up the server frontend
		cout << "Starting up the frontend server.\n";
		PDBFrontEnd serverFrontEnd (port, serverNumConnections, IPCfile, serverMemoryPerConnection, frontendLogFile);
		return 0;
	} 
		
	pid_t frontEndPID = pid;

	// now we just keep on forking off copies of the backend server, as long as it keeps crashing
	while (true) {

		pid = fork ();
		if (pid < 0) {
			cout << "Error forking process to start up the backend server.\n";
			perror ("Error was: ");
			cout << "\n";
			return (0);
		}

		// if we are in the child, then fork off a backend server
		if (pid == 0) {
			cout << "Starting up the backend server.\n";
			unlink (args[3]);
			PDBBackEnd serverBackEnd (numDatabaseThreads, IPCfile, serverMemoryPerConnection, backendLogFile, catalogFile);
			return 0;

		// otherwise, we are in the parent, so wait on the child to finish
		} else {

			// loop until the server dies
			while (true) {

				// now, wait for the backend server to stop
				int result;
				pid_t outPID = waitpid (pid, &result, 0);

				if (outPID < 0) {
					cout << "Error waiting on child.\n";
					perror ("Error was");
					cout << "So I am done.  Goodbye.\n";
					return 0;
				}

				if (WIFEXITED (result)) {
					cout << "It appears that the server exited normally.\n";
					cout << "So I am done.  Goodbye.\n";
					return 0;
				}

				if (WIFSIGNALED (result)) {
					cout << "It appears that the server backend has died.\n";
					cout << "I will start it up again.\n";
					break;
				}
			}

			// signal the frontend that the backend has died... in this case, all of the
			// threads in the frontend that were waiting for something from the backend
			cout << "Letting the frontend server know that the backend has died.\n";
			if (kill (frontEndPID, SIGUSR1) != 0) {
			
				cout << "This is bad: I could not let the frontend server know that the backend has died.\n";	
				perror ("Error was ");
				cout << "\n";
				return 0;
			}
		}
	}
}

#endif
