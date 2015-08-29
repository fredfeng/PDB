
#ifndef _PDB_DATA_COMPAR_H
#define _PDB_DATA_COMPAR_H

#include "PDBQueryResult.h"

// this little struct is used to power the priority queues used by top-K query objects
struct PDBDataComparison {
        bool operator () (const PDBQueryResult &left, const PDBQueryResult &right) {
                return left.getDistanceToQuery () < right.getDistanceToQuery ();
        }
};

#endif
