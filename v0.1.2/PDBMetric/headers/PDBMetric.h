
#ifndef PDBMETRIC_H_
#define PDBMETRIC_H_

// this is the virtual base class from which all distance/similarity metrics are derived

#include "PDBFeatureType.h"
#include <memory>
using namespace std;

// create a smart pointer type for PDBMetric objects
class PDBMetric;
typedef shared_ptr <PDBMetric> PDBMetricPtr;

class PDBMetric {
    public:

	// returns the name of this metric
        virtual const string &getName () = 0;

	// tell hom many parameters this metric takes
        virtual int getNumParams () = 0;

	// accept the param for slot whichParam
        virtual void takeParam (int whichParam, PDBFeatureTypePtr param) = 0;

	// returns the name of the PDBFeatureType associated with the given parameter
	virtual const string &getParamType (int whichParam) = 0;

	// reset this PDBMetric object, so that it forgets all of the parameters it has been given
	// this needs to be called each time the metric is re-parameterized
	virtual void reset () = 0;

	// apply this metric to the various parameters it has been given... if there is an 
	// error (such as a type mismatch in the parameter) wasError is set to true, and and
	// error message is returned
	virtual double apply (bool &wasError, string &errorMessage) = 0;

	// get an unparameterized copy of this metric
	virtual PDBMetricPtr getCopy () = 0;

	// destructor
	virtual ~PDBMetric () = default;
};

#endif /* PDBMETRIC_H_ */

