
#include "PDBClient.h"
#include <iostream>

using namespace std;

int main (int numArgs, const char *args []) {

        if (numArgs != 4) {
                cout << "usage: ./buildDB internetAddressOfServer port logFile\n";
		exit (0);
        }

        string internetAddressOfServer (args[1]);
        int port = atoi (args[2]);
        string logFile (args[3]);

        PDBClient myDB;
        bool wasError;
        string errMsg;
        myDB.connectToServer (internetAddressOfServer, port, wasError, errMsg, 12 * 1024 * 1204, logFile);

	if (wasError) {
		cout << "Error connecting to the DB: " << errMsg << '\n';
		exit (1);
	}
	
	myDB.shutDownServer (wasError, errMsg);

	if (wasError) {
		cout << "Error shutting down the DB: " << errMsg << '\n';
		exit (1);
	}

	cout << "Done shutting down the server.\n";
}

