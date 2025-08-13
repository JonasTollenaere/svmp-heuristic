//
// Created by Jonas on 15/04/2022.
//

#ifndef OPTIXMESHCORE_TRANSLATIONMOVE_H
#define OPTIXMESHCORE_TRANSLATIONMOVE_H

#include <meshcore/optimization/AbstractLocalSearch.h>
#include "meshcore/optimization/SingleVolumeMaximisationSolution.h"

class TranslateMove: public Move<SingleVolumeMaximisationSolution> {
    const glm::vec3 translation;
    glm::vec3 originalPosition;
public:
    TranslateMove(const std::shared_ptr<const SingleVolumeMaximisationSolution> &s, glm::vec3 translation);
    void doMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) override;
    void undoMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) override;

};

class TranslationMoveFactory: public MoveFactory<SingleVolumeMaximisationSolution> {
    const float range;
public:
    explicit TranslationMoveFactory(float range);
    [[nodiscard]] std::shared_ptr<Move<SingleVolumeMaximisationSolution>> sample(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, const Random &random, float stepSize) const override;
    std::vector<std::shared_ptr<Move<SingleVolumeMaximisationSolution>>> listMoves(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, float stepSize) const override;
};


#endif //OPTIXMESHCORE_TRANSLATIONMOVE_H
