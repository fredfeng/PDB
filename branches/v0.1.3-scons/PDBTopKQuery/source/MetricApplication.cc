#include "MetricApplication.h"
#include <cstdlib>
#include <functional>
#include <iostream>


namespace {
	template <typename Chaff> static void destroy(Chaff *chaff, std::function<void (void)> descend) {
		if (chaff != nullptr) {
			descend ();
			free (chaff);
		}
	}
}


void free_MetricParam(MetricParam *chaff) {
	destroy (chaff, [chaff](){
			free (chaff->paramName);
		});
}


void free_MetricParamList(MetricParamList *chaff) {
	destroy (chaff, [chaff](){
			free_MetricParamList (chaff->next);
			free_MetricParam (chaff->param);
		});
}


void free_MetricApplication(MetricApplication *chaff) {
	destroy (chaff, [chaff](){
			free_MetricApplication (chaff->next);
			free_MetricParamList (chaff->paramList);
			free (chaff->metricName);
		});
}


void free_FinalQuery(FinalQuery *chaff) {
	destroy (chaff, [chaff](){
			free_MetricApplication (chaff->metricApplication);
		});
}
