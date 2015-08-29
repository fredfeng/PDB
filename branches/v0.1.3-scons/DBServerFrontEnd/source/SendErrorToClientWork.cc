
#ifndef SEND_ERROR_TO_CLIENT_WORK_CC
#define SEND_ERROR_TO_CLIENT_WORK_CC

#include "EmptyPDBBuzzer.h"
#include "SendErrorToClientWork.h"

PDBCommWorkPtr SendErrorToClientWork :: clone () {
	return {make_shared <SendErrorToClientWork> ()};
}

PDBBuzzerPtr SendErrorToClientWork :: getLinkedBuzzer () {
	return {make_shared <EmptyPDBBuzzer> ()};
}

void SendErrorToClientWork :: execute (PDBBuzzerPtr callerBuzzer) {
	getLogger ()->writeLn ("SendErrorToClientWork: sending " + errMsg);
	if (getCommunicator ()->sendError (errMsg)) {
		getLogger ()->writeLn ("SendErrorToClientWork: error send sending error messge " + errMsg);
		callerBuzzer->buzz (PDBAlarm :: GenericError);
	}

	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

SendErrorToClientWork :: SendErrorToClientWork (PDBCommunicatorPtr useMe, string errMsgIn) {
	setGuts (useMe);
	errMsg = errMsgIn;
}

#endif

