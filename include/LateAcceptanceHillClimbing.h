//
// Created by Jonas on 15/04/2022.
//

#ifndef OPTIXMESHCORE_INFLATINGLATEACCEPTANCE_H
#define OPTIXMESHCORE_INFLATINGLATEACCEPTANCE_H

#include "meshcore/optimization/SingleVolumeMaximisationSolution.h"
#include <meshcore/optimization/AbstractLocalSearch.h>

class LateAcceptanceHillClimbing: public AbstractLocalSearch<SingleVolumeMaximisationSolution> {
    const unsigned int L;
    unsigned int acceptanceIndex=0;
    std::vector<float> acceptanceValues;
    float minDeltaScale = 1e-4f;
public:
    LateAcceptanceHillClimbing(MoveFactory<SingleVolumeMaximisationSolution>& moveFactory, const ObjectiveFunction<SingleVolumeMaximisationSolution>& objectiveFunction, unsigned int maximumIterations, const Random& random, unsigned int L);

    float performIteration(unsigned int currentIteration, std::shared_ptr<SingleVolumeMaximisationSolution>& currentSolution,
                            float currentScore) override;

    void initialize(const std::shared_ptr<SingleVolumeMaximisationSolution>& initialSolution, float initialScore) override;
};


#endif //OPTIXMESHCORE_INFLATINGLATEACCEPTANCE_H
