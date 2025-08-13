//
// Created by Jonas on 15/04/2022.
//

#include "BasicHillClimbing.h"
#include "BinaryInflater.h"
#include "meshcore/optimization/SingleVolumeMaximisationSolution.h"

float BasicHillClimbing::performIteration(unsigned int currentIteration, std::shared_ptr<SingleVolumeMaximisationSolution>& currentSolution, float currentScore) {

    auto& item = currentSolution->getItemWorldSpaceMesh();
    const auto initialScale = item->getModelTransformation().getScale();

    float stepSize = std::pow(64,-static_cast<float>(currentIteration)/maximumIterations);

    // Sample a move
    auto move = this->moveFactory.sample(currentSolution, this->random,stepSize);
    if(move==nullptr){
        return currentScore;
    }

#if true
    // Apply the move
    move->doMove(currentSolution);

    // Check if it's possible to improve on the initial scale
    if(!currentSolution->isFeasible()){
        move->undoMove(currentSolution);
        item->getModelTransformation().setScale(initialScale);
        return currentScore;
    }

    // Perform inflation
    BinaryInflater::inflate(currentSolution);

    if(item->getModelTransformation().getScale() < initialScale){
        move->undoMove(currentSolution);
        item->getModelTransformation().setScale(initialScale);
        return currentScore;
    }

    assert(item->getModelTransformation().getScale() >= initialScale);
    return objectiveFunction.evaluate(currentSolution);
#else

    // Naive implementation
    move->doMove(currentSolution);
    inflater.inflate(std::dynamic_pointer_cast<AABBSingleItemSolution>(currentSolution));
    if(item.getModelTransformation().getScale() < initialScale){
        move->undoMove(currentSolution);
        item.getModelTransformation().setScale(initialScale);
        return currentScore;
    }

#if !NDEBUG
    std::cout << "returning with scale: " << item.getModelTransformation().getScale() << std::endl;
    std::cout << "currentScore: " << currentScore << std::endl;
    std::cout << "objectiveFunction.evaluate(currentSolution): " << objectiveFunction.evaluate(currentSolution) << std::endl;
    assert(objectiveFunction.evaluate(currentSolution) <= currentScore);
#endif // !NDEBUG
    return objectiveFunction.evaluate(currentSolution);
#endif //

}

void
BasicHillClimbing::initialize(const std::shared_ptr<SingleVolumeMaximisationSolution>& initialSolution, float initialScore) {
    // Do nothing
}
