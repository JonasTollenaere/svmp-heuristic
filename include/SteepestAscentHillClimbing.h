//
// Created by Jonas on 6/05/2022.
//

#ifndef OPTIXMESHCORE_INFLATINGSTEEPESTDESCENT_H
#define OPTIXMESHCORE_INFLATINGSTEEPESTDESCENT_H

#include "meshcore/optimization/SingleVolumeMaximisationSolution.h"
#include <meshcore/optimization/AbstractLocalSearch.h>

class SteepestAscentHillClimbing: public AbstractLocalSearch<SingleVolumeMaximisationSolution> {

    float stepSize=1.0f;
    float stepSizeReductionFactor=0.5f;
    float minimumDeltaScale = 1e-4f;
    float minimumStepSize = 1e-4f;

public:
    using AbstractLocalSearch::AbstractLocalSearch;
    float performIteration(unsigned int currentIteration, std::shared_ptr<SingleVolumeMaximisationSolution>& currentSolution, float currentScore) override;
    void initialize(const std::shared_ptr<SingleVolumeMaximisationSolution>& initialSolution, float initialScore) override;

    float getStepSizeReductionFactor() const;
    void setStepSizeReductionFactor(float stepSizeReductionFactor);

    float getMinimumDeltaScale() const;
    void setMinimumDeltaScale(float minimumDeltaScale);

    float getMinimumStepSize() const;
    void setMinimumStepSize(float minimumStepSize);
};


#endif //OPTIXMESHCORE_INFLATINGSTEEPESTDESCENT_H
