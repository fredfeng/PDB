
#ifndef FEATURE_BUILDER_PTR_H
#define FEATURE_BUILDER_PTR_H

#include <memory>

// create a smart pointer for PDBFetureBuilder objects
class PDBFeatureBuilder;
typedef std::shared_ptr <PDBFeatureBuilder> PDBFeatureBuilderPtr;

#endif
