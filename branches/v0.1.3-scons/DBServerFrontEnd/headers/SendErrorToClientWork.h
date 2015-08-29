
#ifndef SEND_ERROR_TO_CLIENT_WORK_H
#define SEND_ERROR_TO_CLIENT_WORK_H

#include "PDBCommWork.h"

class SendErrorToClientWork : public PDBCommWork {

public:

	// send an error message to a client
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// constructor; accept the communicator that is pointing to the client, plus the error message
	SendErrorToClientWork (PDBCommunicatorPtr useMe, string errMsg);

	// from the interface; used when the server needs to build one of these
        PDBCommWorkPtr clone () override;

	// from the interface; return a buzzer linked to this object
        PDBBuzzerPtr getLinkedBuzzer () override;

	// default constructor
	SendErrorToClientWork () = default;

private:

	// the error message to deliver
	string errMsg;
};

#endif

