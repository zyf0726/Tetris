
#ifndef SINGLEFILE
#include "options.h"
#include "feature_functions.h"
#endif

void enable_feature (int feature_i, struct options * opt) {
    if (opt->feature_enabled[feature_i] == 1) {
        return;
    }

    opt->feature_enabled[feature_i] = 1;
    opt->enabled_f_indices[opt->n_features_enabled++] = feature_i;
    opt->n_weights_enabled += features[feature_i].weights;
}
