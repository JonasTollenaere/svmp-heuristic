//
// Created by Jonas on 15/04/2022.
//

#include "LateAcceptanceHillClimbing.h"
#include "BinaryInflater.h"

LateAcceptanceHillClimbing::LateAcceptanceHillClimbing(MoveFactory<SingleVolumeMaximisationSolution>& moveFactory,
                                                 const ObjectiveFunction<SingleVolumeMaximisationSolution>& objectiveFunction,
                                                 unsigned int maximumIterations, const Random& random, unsigned int L):
        AbstractLocalSearch(moveFactory, objectiveFunction, maximumIterations, random), L(L) {}

float LateAcceptanceHillClimbing::performIteration(unsigned int currentIteration,
                                                 std::shared_ptr<SingleVolumeMaximisationSolution>& currentSolution,
                                                 float currentScore) {

    auto& item = currentSolution->getItemWorldSpaceMesh();
    const auto initialScale = item->getModelTransformation().getScale();

    float stepSize = std::pow(128,-static_cast<float>(currentIteration)/maximumIterations);

    // Sample a move
    auto move = this->moveFactory.sample(currentSolution, this->random,stepSize);
//    auto moves = this->moveFactory.sampleMoves(currentSolution, this->random,stepSize); // somehow this behaves very different in terms of solution finding
//    auto move = moves[random.nextInteger(0,moves.size()-1)];
    if(move==nullptr){
        return currentScore;
    }

    // Apply the move
    move->doMove(currentSolution);

#define USE_VERSION_3

#ifdef USE_VERSION_1
    // First inflate
    BinaryInflater::inflate(currentSolution);
    // Test if the resulting scale is good enough to accept the move
    const float targetScale = float(acceptanceValues.at(acceptanceIndex)) + minDeltaScale;
    if(item->getModelTransformation().getScale() < targetScale) {
        item->getModelTransformation().setScale(targetScale);
        assert(!currentSolution->isFeasible());
        move->undoMove(currentSolution);
        item->getModelTransformation().setScale(initialScale);
        return currentScore;
    }
#endif

#ifdef USE_VERSION_2
    // Test if we can improve upon the best scale first
    const float targetScale = float(acceptanceValues.at(acceptanceIndex)) + minDeltaScale;
    item->getModelTransformation().setScale(targetScale);
    if(!currentSolution->isFeasible()){
        move->undoMove(currentSolution);
        item->getModelTransformation().setScale(initialScale);
        acceptanceIndex++;
        if(acceptanceIndex==L){
            acceptanceIndex = 0;
        }
        return currentScore;
    }
    BinaryInflater::inflate(currentSolution);
    if(item->getModelTransformation().getScale() < targetScale){
        item->getModelTransformation().setScale(targetScale);
    }
    assert(item->getModelTransformation().getScale() >= targetScale);

#endif
#ifdef USE_VERSION_3
    // Test if we can improve upon the best scale first
    const float targetScale = acceptanceValues.at(acceptanceIndex) + minDeltaScale;
    item->getModelTransformation().setScale(targetScale);
    if(!currentSolution->isFeasible()){
        move->undoMove(currentSolution);
        item->getModelTransformation().setScale(initialScale);
//        acceptanceValues[acceptanceIndex] = item->getModelTransformation().getScale();
        acceptanceIndex++;
        if(acceptanceIndex==L){
            acceptanceIndex = 0;
        }
        return currentScore;
    }
    BinaryInflater::inflate(currentSolution, targetScale); // Pass on the knowledge that (bestScale + 1e-4f) is feasible to the binary inflater, speed up inflation
    if(item->getModelTransformation().getScale() < targetScale){
        item->getModelTransformation().setScale(targetScale);
    }
    assert(item->getModelTransformation().getScale() >= targetScale);
#endif

#ifdef USE_VERSION_4
    // Test if we can improve upon the best scale first
    const float targetScale = float(acceptanceValues.at(acceptanceIndex)) + minDeltaScale;
    item->getModelTransformation().setScale(targetScale);
    if(!currentSolution->isFeasible()){
        move->undoMove(currentSolution);
        item->getModelTransformation().setScale(initialScale);
        acceptanceIndex++;
        if(acceptanceIndex==L){
            acceptanceIndex = 0;
        }
        return currentScore;
    }
    DecomposingInflater::inflate(std::dynamic_pointer_cast<AABBSingleItemSolution>(currentSolution), targetScale); // Pass on the knowledge that (bestScale + 1e-4f) is feasible to the binary inflater, speed up inflation
    if(item->getModelTransformation().getScale() < targetScale){
        item->getModelTransformation().setScale(targetScale);
    }
    assert(item->getModelTransformation().getScale() >= targetScale);
#endif


    // Update acceptance array
    assert(item->getModelTransformation().getScale() >= acceptanceValues[acceptanceIndex]);
    acceptanceValues[acceptanceIndex] = item->getModelTransformation().getScale();
    acceptanceIndex++;
    if(acceptanceIndex==L){
        acceptanceIndex = 0;
    }

    assert(item->getModelTransformation().getScale() >= targetScale);
    return objectiveFunction.evaluate(currentSolution);
}

void LateAcceptanceHillClimbing::initialize(const std::shared_ptr<SingleVolumeMaximisationSolution>& initialSolution, float initialScore) {
    auto& item = initialSolution->getItemWorldSpaceMesh();
    const auto initialScale = item->getModelTransformation().getScale();
    for(int i = 0; i < L; i++){
        this->acceptanceValues.emplace_back(initialScale);
    }
    this->acceptanceIndex = 0;
}

