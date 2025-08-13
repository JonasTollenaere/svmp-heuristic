//
// Created by Jonas on 15/04/2022.
//

#ifndef OPTIXMESHCORE_INFLATINGHILLCLIMBING_H
#define OPTIXMESHCORE_INFLATINGHILLCLIMBING_H

#include <meshcore/optimization/AbstractLocalSearch.h>
#include "meshcore/optimization/SingleVolumeMaximisationSolution.h"

class BasicHillClimbing: public AbstractLocalSearch<SingleVolumeMaximisationSolution> {

public:
    using AbstractLocalSearch::AbstractLocalSearch;
    float performIteration(unsigned int currentIteration, std::shared_ptr<SingleVolumeMaximisationSolution>& currentSolution, float currentScore) override;
    void initialize(const std::shared_ptr<SingleVolumeMaximisationSolution>& initialSolution, float initialScore) override;
};


#endif //OPTIXMESHCORE_INFLATINGHILLCLIMBING_H
