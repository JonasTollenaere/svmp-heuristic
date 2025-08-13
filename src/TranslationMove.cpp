//
// Created by Jonas on 15/04/2022.
//

#include "TranslationMove.h"

std::shared_ptr<Move<SingleVolumeMaximisationSolution>> TranslationMoveFactory::sample(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, const Random &random, float stepSize) const {

    // Sample random translation (equal-area projection)
    auto z = 2 * random.nextFloat() - 1;
    auto theta = random.nextFloat() * 2 * glm::pi<float>();
    glm::vec3 unitTranslation(glm::sqrt(1-z*z) * glm::cos(theta), glm::sqrt(1-z*z) * glm::sin(theta), z);
    auto translation = random.nextFloat() * unitTranslation * range * stepSize;

    return std::make_shared<TranslateMove>(s, translation);
}

TranslationMoveFactory::TranslationMoveFactory(float range): range(range) {}

std::vector<std::shared_ptr<Move<SingleVolumeMaximisationSolution>>>
TranslationMoveFactory::listMoves(const std::shared_ptr<const SingleVolumeMaximisationSolution>& s, float stepSize) const {

    float r = stepSize * this->range;
    std::vector<std::shared_ptr<Move<SingleVolumeMaximisationSolution>>> moves;
    moves.reserve(26);

    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, 0, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, 0, -r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, r, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, r, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, r, -r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, -r, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, -r, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(0, -r, -r)));

    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, 0, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, 0, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, 0, -r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, r, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, r, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, r, -r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, -r, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, -r, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(r, -r, -r)));

    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, 0, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, 0, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, 0, -r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, r, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, r, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, r, -r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, -r, 0)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, -r, r)));
    moves.emplace_back(std::make_shared<TranslateMove>(s, glm::dvec3(-r, -r, -r)));

    return moves;
}

void TranslateMove::doMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) {
    s->getItemWorldSpaceMesh()->getModelTransformation().deltaPosition(translation);
}

void TranslateMove::undoMove(std::shared_ptr<SingleVolumeMaximisationSolution> s) {
    s->getItemWorldSpaceMesh()->getModelTransformation().setPosition(originalPosition);
}

TranslateMove::TranslateMove(const std::shared_ptr<const SingleVolumeMaximisationSolution> &s, glm::vec3 translation): translation(translation), originalPosition() {
    originalPosition = s->getItemWorldSpaceMesh()->getModelTransformation().getPosition();
}