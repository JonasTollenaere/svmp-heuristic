//
// Created by Jonas on 15/04/2022.
//

#ifndef OPTIXMESHCORE_ROTATIONMOVE_H
#define OPTIXMESHCORE_ROTATIONMOVE_H

#include <meshcore/optimization/AbstractLocalSearch.h>
#include "meshcore/optimization/SingleVolumeMaximisationSolution.h"

class RotationMove: public Move<SingleVolumeMaximisationSolution> {
    const Quaternion factorRotation;
    Quaternion originalRotation;

public:
    RotationMove(const std::shared_ptr<const SingleVolumeMaximisationSolution> &s, const Quaternion& factorRotation);

    void doMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) override;
    void undoMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) override;
};

class RotationMoveFactory: public MoveFactory<SingleVolumeMaximisationSolution> {
    float range;
public:
    explicit RotationMoveFactory(float range);
    [[nodiscard]] std::shared_ptr<Move<SingleVolumeMaximisationSolution>>sample(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, const Random &random, float stepSize) const override;
    [[nodiscard]] std::vector<std::shared_ptr<Move<SingleVolumeMaximisationSolution>>> listMoves(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, float stepSize) const override;
};


#endif //OPTIXMESHCORE_ROTATIONMOVE_H
