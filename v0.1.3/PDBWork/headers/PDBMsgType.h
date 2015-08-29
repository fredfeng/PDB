
#ifndef PDB_MESSAGE_TYPE_H
#define PDB_MESSAGE_TYPE_H

// this lists all of the identifiers for the different messages that can be received by a PDBCommunicator
// object.  See PDBCommunicator.h for more info.
enum PDBMsgType {

        QueryMsg,
	ErrMsg,
	BuildFeaturesMsg,
	InfoMsg,
        InfoReceivedMsg,
	ShutDownMsg,
	StoreDataNewFileMsg,
	NoMsg,
	CloseConnection,
        LoadedCodeForPDBMetricMsg,
        LoadedCodeForPDBQueryExecMsg,
        LoadedCodeForPDBFeatureMsg,
        LoadedCodeForPDBStoredDataMsg,
        LoadedCodeForFeatureBuilderMsg,
        CodeForPDBMetricMsg,
        CodeForPDBQueryExecMsg,
        CodeForPDBFeatureMsg,
        CodeForPDBStoredDataMsg,
        CodeForFeatureBuilderMsg,
	StoreDataNewFileDoneMsg,
        LoadDataDoneMsg,
	ShutDownDoneMsg,
        LoadDataMsg,
        FeatureBuildTaskDoneMsg,
        QueryResultMsg

};

#endif
