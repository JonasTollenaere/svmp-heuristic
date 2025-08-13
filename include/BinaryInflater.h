//
// Created by Jonas on 11/04/2022.
//

#ifndef OPTIXMESHCORE_BINARYINFLATER_H
#define OPTIXMESHCORE_BINARYINFLATER_H

#include "meshcore/optimization/SingleVolumeMaximisationSolution.h"

class BinaryInflater {
public:

    static void inflate(const std::shared_ptr<SingleVolumeMaximisationSolution>& solution){
        inflate(solution, 0.0f);
    }

    static void inflate(const std::shared_ptr<SingleVolumeMaximisationSolution>& solution, float knownFeasibleScale){

        auto& item = solution->getItemWorldSpaceMesh();
        auto maxScale = 1.0f; // Assumes item dimensions is normalized to container dimensions
        auto minDeltaScale = maxScale * 1e-5f;

        // Test if the initial position is valid
        if(knownFeasibleScale<=0.0f){
            item->getModelTransformation().setScale(minDeltaScale);
            if(!solution->isFeasible()){
                item->getModelTransformation().setScale(0.0f);
                return;
            }
        }

        // The knownFeasibleScale often doesn't reduce the search interval significantly
        // The speed-up is most likely caused by a quick fail of the feasible check

        // Binary search
        auto lastFeasibleScale = knownFeasibleScale;
        item->getModelTransformation().setScale(knownFeasibleScale);
        auto currentStep = maxScale - knownFeasibleScale;
        while(currentStep >= minDeltaScale){
            currentStep /= 2.0f;
            item->getModelTransformation().deltaScale(currentStep);
            if(!solution->isFeasible()){
                item->getModelTransformation().setScale(lastFeasibleScale); // This is equivalent to taking the step back, but not prone to rounding errors
            }
            else{
                lastFeasibleScale = item->getModelTransformation().getScale();
            }
        }

// Add some checks in debug mode
#if !NDEBUG

        // Scale should be positive
        assert(solution->getItemWorldSpaceMesh()->getModelTransformation().getScale() >= 0.0f && "The item scale should be positive");

        // Scale at least as large as the known feasible scale
        assert(solution->getItemWorldSpaceMesh()->getModelTransformation().getScale() >= knownFeasibleScale  && "The item is smaller than the known feasible scale");

        // The final solution should be feasible
        assert(solution->isFeasible() && "Inflater: Final solution should be feasible but is not");

        // Adding minDeltaScale should result in an infeasible solution
        const auto finalScale = item->getModelTransformation().getScale();
        item->getModelTransformation().deltaScale(minDeltaScale);
        assert(!solution->isFeasible() && "Inflater: Adding minDeltaScale should result in an infeasible solution");

        item->getModelTransformation().setScale(finalScale);
        assert(finalScale == item->getModelTransformation().getScale() && "Inflater: Debug check should not change the scale");
        assert(lastFeasibleScale == item->getModelTransformation().getScale() && "Inflater: Debug check should not change the scale");
#endif

    }
};

#endif //OPTIXMESHCORE_BINARYINFLATER_H