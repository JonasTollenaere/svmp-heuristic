//
// Created by Jonas on 6/05/2022.
//

#include "SteepestAscentHillClimbing.h"
#include "BinaryInflater.h"

float SteepestAscentHillClimbing::performIteration(unsigned int currentIteration, std::shared_ptr<SingleVolumeMaximisationSolution>& currentSolution, float currentScore) {

    auto& item = currentSolution->getItemWorldSpaceMesh();
    const auto initialScale = item->getModelTransformation().getScale();

    // Keep track of best move and scale
    std::shared_ptr<Move<SingleVolumeMaximisationSolution>> bestMove = nullptr;
    auto bestScale = initialScale;

    // Iterate over all possible moves
    const auto moves = this->moveFactory.listMoves(currentSolution, this->stepSize);
    for (const auto &move: moves){

        move->doMove(currentSolution);

#define USE_VERSION_3
#define ACCEPT_FIRST false

#ifdef USE_VERSION_1
        // Naive version, inflate every time
        BinaryInflater::inflate(currentSolution);
        if(item->getModelTransformation().getScale() > bestScale + minimumDeltaScale){
            bestScale = item->getModelTransformation().getScale();
            bestMove = move;
        }
#endif
#ifdef USE_VERSION_2
        // Test if we can improve upon the best scale first
        float targetScale = bestScale + minimumDeltaScale;
        item->getModelTransformation().setScale(targetScale);
        if(currentSolution->isFeasible()){
            BinaryInflater::inflate(currentSolution);
            if(item->getModelTransformation().getScale() < targetScale){ // This is actually necessary to function correctly and avoid loops
                item->getModelTransformation().setScale(targetScale);
            }
            assert(item->getModelTransformation().getScale() >= bestScale);
            bestScale = item->getModelTransformation().getScale();
            bestMove = move;
        }
#endif
#ifdef USE_VERSION_3
        // Test if we can improve upon the best scale first
        float targetScale = bestScale + minimumDeltaScale;
        item->getModelTransformation().setScale(targetScale);
        if(currentSolution->isFeasible()){
            BinaryInflater::inflate(currentSolution, targetScale); // Pass on the knowledge that (bestScale + 1e-4f) is feasible to the binary inflater, speed up inflation
            if(item->getModelTransformation().getScale() < targetScale){
                item->getModelTransformation().setScale(targetScale);
            }
            assert(item->getModelTransformation().getScale() >= bestScale);
            bestScale = item->getModelTransformation().getScale();
            bestMove = move;
        }
#endif

        // Go back to the initial state
        move->undoMove(currentSolution);
        item->getModelTransformation().setScale(initialScale);

#if ACCEPT_FIRST
        if(bestMove != nullptr){
            break;
        }
#endif
    }

    // Perform the best move
    if(bestMove != nullptr){
        bestMove->doMove(currentSolution);
        item->getModelTransformation().setScale(bestScale);
        return objectiveFunction.evaluate(currentSolution);
    }
    else{
        assert(objectiveFunction.evaluate(currentSolution) == currentScore);

        // Stop if the stepSize is too small
        if(this->stepSize < this->minimumStepSize){
            this->stopped = true;
        }

        // Decrease the stepSize
        assert(stepSizeReductionFactor < 1.0);
        this->stepSize *= stepSizeReductionFactor;

        return currentScore;
    }
}

void SteepestAscentHillClimbing::initialize(const std::shared_ptr<SingleVolumeMaximisationSolution>& initialSolution, float initialScore) {
    this->stepSize = 1.0f;
}

float SteepestAscentHillClimbing::getStepSizeReductionFactor() const {
    return stepSizeReductionFactor;
}

void SteepestAscentHillClimbing::setStepSizeReductionFactor(float newStepSizeReductionFactor) {
    stepSizeReductionFactor = newStepSizeReductionFactor;
}

float SteepestAscentHillClimbing::getMinimumDeltaScale() const {
    return minimumDeltaScale;
}

void SteepestAscentHillClimbing::setMinimumDeltaScale(float newMinimumDeltaScale) {
    minimumDeltaScale = newMinimumDeltaScale;
}

float SteepestAscentHillClimbing::getMinimumStepSize() const {
    return minimumStepSize;
}

void SteepestAscentHillClimbing::setMinimumStepSize(float newMinimumStepSize) {
    minimumStepSize = newMinimumStepSize;
}
