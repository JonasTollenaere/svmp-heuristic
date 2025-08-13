//
// Created by Jonas on 15/04/2022.
//

#include "RotationMove.h"

RotationMove::RotationMove(const std::shared_ptr<const SingleVolumeMaximisationSolution> &s, const Quaternion& factorRotation):factorRotation(factorRotation) {
    this->originalRotation = s->getItemWorldSpaceMesh()->getModelTransformation().getRotation();
}

void RotationMove::doMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) {
    s->getItemWorldSpaceMesh()->getModelTransformation().factorRotation(factorRotation);
}

void RotationMove::undoMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) {
    s->getItemWorldSpaceMesh()->getModelTransformation().setRotation(originalRotation);
}

std::shared_ptr<Move<SingleVolumeMaximisationSolution>> RotationMoveFactory::sample(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, const Random &random, float stepSize) const {

    auto deltaYaw = range*stepSize*(2.0f * random.nextFloat() - 1.0f);
    auto deltaPitch = range*stepSize*(2.0f * random.nextFloat() - 1.0f);
    auto deltaRoll = range*stepSize*(2.0f * random.nextFloat() - 1.0f);

    Quaternion factorRotation(deltaYaw, deltaPitch, deltaRoll);

    return std::make_shared<RotationMove>(s, factorRotation);
}

RotationMoveFactory::RotationMoveFactory(float range): range(range) {}

std::vector<std::shared_ptr<Move<SingleVolumeMaximisationSolution>>>
RotationMoveFactory::listMoves(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, float stepSize) const {

    float r = stepSize * this->range;
    std::vector<std::shared_ptr<Move<SingleVolumeMaximisationSolution>>> moves;
    moves.reserve(26);

    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, 0, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, 0, -r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, r, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, r, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, r, -r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, -r, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, -r, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(0, -r, -r)));

    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, 0, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, 0, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, 0, -r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, r, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, r, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, r, -r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, -r, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, -r, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(r, -r, -r)));

    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, 0, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, 0, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, 0, -r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, r, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, r, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, r, -r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, -r, 0)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, -r, r)));
    moves.emplace_back(std::make_shared<RotationMove>(s, Quaternion(-r, -r, -r)));

    return moves;
}