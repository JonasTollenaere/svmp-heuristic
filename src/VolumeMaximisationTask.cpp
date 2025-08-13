//
// Created by Jonas on 12/04/2022.
//

#include "VolumeMaximisationTask.h"

#include <chrono>
#include <nlohmann/json.hpp>
#include "meshcore/utility/FileParser.h"
#include "meshcore/utility/io.h"
#include <iostream>
#include <string>
#include <glm/gtx/component_wise.hpp>
#include "LateAcceptanceHillClimbing.h"
#include "TranslationMove.h"
#include "RotationMove.h"
#include "SteepestAscentHillClimbing.h"

void VolumeMaximisationTask::run() {

    // File loading and logging
    this->notifyObserversStatus("Initialising");
    std::shared_ptr<ModelSpaceMesh> containerModelSpaceMesh = FileParser::loadMeshFile(containerPath);
    if(!containerModelSpaceMesh) return;
    auto containerMesh = std::make_shared<WorldSpaceMesh>(containerModelSpaceMesh);
    auto itemModelSpaceMesh = FileParser::loadMeshFile(itemPath);
    if(!itemModelSpaceMesh) return;
    std::cout << "Processing: " << itemModelSpaceMesh->getName() << " in " << containerModelSpaceMesh->getName() << std::endl;

    // Item coordinate normalisation
    // 1. Normalise scale 1 to match the largest dimension of the container
    // 2. Place the center of the object in the origin of its coordinate system
    auto itemHalfBox = itemModelSpaceMesh->getBounds().getHalf();
    auto containerHalfBox = containerMesh->getModelSpaceMesh()->getBounds().getHalf() * containerMesh->getModelTransformation().getScale();
    auto maxScale = glm::length(containerHalfBox)/glm::length(itemHalfBox); // Absolute largest possible scale
    auto normalizingScaleFactor = maxScale;

    Transformation normalizingTransformation;
    normalizingTransformation.setScale(normalizingScaleFactor);
    normalizingTransformation.setPosition(-normalizingScaleFactor * itemModelSpaceMesh->getBounds().getCenter());

    std::vector<Vertex> correctedVertices;
    correctedVertices.reserve(itemModelSpaceMesh->getVertices().size());
    for (auto &vertex : itemModelSpaceMesh->getVertices()){
        correctedVertices.emplace_back(normalizingTransformation.transformVertex(vertex));
    }
    auto name = itemModelSpaceMesh->getName();
    auto originalMesh = itemModelSpaceMesh;
    itemModelSpaceMesh = std::make_shared<ModelSpaceMesh>(correctedVertices, itemModelSpaceMesh->getTriangles());
    itemModelSpaceMesh->setName(name);

    // Create the starting solution
    auto innerMesh = std::make_shared<WorldSpaceMesh>(itemModelSpaceMesh);
    auto singleModelSolution = std::make_shared<SingleVolumeMaximisationSolution>(innerMesh, containerMesh);
    innerMesh->getModelTransformation().setScale(0.001f); // Scale the item to 1/1000 of its original size
    containerMesh->getModelTransformation().setPosition(-containerModelSpaceMesh->getBounds().getCenter()); // Center the material around the origin
    this->notifyObserversSolution(singleModelSolution);

    // Local search
    auto start = std::chrono::high_resolution_clock::now();
    {
        std::vector<std::shared_ptr<MoveFactory<SingleVolumeMaximisationSolution>>> moveFactories;
        auto size = glm::length(containerMesh->getModelSpaceMesh()->getBounds().getHalf());
        moveFactories.emplace_back(std::make_shared<TranslationMoveFactory>(size * this->relativeTranslationStepSize));
        moveFactories.emplace_back(std::make_shared<RotationMoveFactory>(this->rotationStepSize));
        CompositeMoveFactory compositeMoveFactory(moveFactories);

        class: public ObjectiveFunction<SingleVolumeMaximisationSolution>{
        public:
            [[nodiscard]] float evaluate(std::shared_ptr<const SingleVolumeMaximisationSolution> s) const override{
                float scale = s->getItemWorldSpaceMesh()->getModelTransformation().getScale();
                return -scale;
            }
        } objectiveFunction;

        Random random(0);
        auto iterations = 10000;
        SteepestAscentHillClimbing algorithm(compositeMoveFactory, objectiveFunction, iterations, random);
        algorithm.setStepSizeReductionFactor(this->stepSizeReductionFactor);
        algorithm.setMinimumDeltaScale(this->minimumDeltaScale);
        algorithm.setMinimumStepSize(this->minimumStepSize);
//                InflatingHillClimbing algorithm(compositeMoveFactory, objectiveFunction, iterations, random);
//                InflatingLateAcceptance algorithm(customMoveFactory, objectiveFunction, iterations, random, 10);
//
        // rel 1.006 scale in 0.5 time;
//                auto iterations = 100000;
//                CustomMoveFactory customMoveFactory(size/4, glm::pi<float>());
//                InflatingLateAcceptance algorithm(customMoveFactory, objectiveFunction, iterations, random, 50);

        // rel 1.006 scale in 0.25 time;
//                auto iterations = 50000;
//                CustomMoveFactory customMoveFactory(size/4, glm::pi<float>());
//                InflatingLateAcceptance algorithm(customMoveFactory, objectiveFunction, iterations, random, 25);

        // rel 1.000 scale in 0.14
//                auto iterations = 25000;
//                CustomMoveFactory customMoveFactory(size/4, glm::pi<float>());
//                InflatingLateAcceptance algorithm(customMoveFactory, objectiveFunction, iterations, random, 15);

        // rel 0.999 scale in 0.12
//                auto iterations = 20000;
//                CustomMoveFactory customMoveFactory(size/4, glm::pi<float>());
//                InflatingLateAcceptance algorithm(customMoveFactory, objectiveFunction, iterations, random, 15);


        // rel 1.001 scale in 0.127
//                auto iterations = 25000;
//                CustomMoveFactory customMoveFactory(size/4, glm::pi<float>());
//                InflatingLateAcceptance algorithm(customMoveFactory, objectiveFunction, iterations, random, 12);

        std::shared_ptr<LocalSearchListener<SingleVolumeMaximisationSolution>> listener = std::make_shared<LocalSearchListener<SingleVolumeMaximisationSolution>>();
        listener->setOnNewBestSolutionFound([&](const std::shared_ptr<const SingleVolumeMaximisationSolution>& solution, float bestScore, unsigned int iteration){
            this->notifyObserversSolution(solution);
        });
        algorithm.addListener(listener);

        singleModelSolution = std::static_pointer_cast<SingleVolumeMaximisationSolution>(algorithm.executeSearch(singleModelSolution));
    }
    auto stop = std::chrono::high_resolution_clock::now();

    // Post-processing
    // Determine the final transformation for the mesh when it's not normalized and validate if the solution is correct
    {
        // Load the meshes again
        auto validationMesh = FileParser::loadMeshFile(itemPath);
        auto validationContainer = FileParser::loadMeshFile(containerPath);
        auto validationWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(validationMesh);
        auto validationContainerWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(validationContainer);

        // Determine the transformation
        Transformation correctedTransformation = singleModelSolution->getItemWorldSpaceMesh()->getModelTransformation();
        correctedTransformation.factorScale(normalizingScaleFactor);
        correctedTransformation.setPosition(singleModelSolution->getItemWorldSpaceMesh()->getModelTransformation().transformVertex(normalizingTransformation.getPosition()));
        correctedTransformation.deltaPosition(validationContainer->getBounds().getCenter());

        // Create the solution and validate its feasibility
        auto finalSolution = std::make_shared<SingleVolumeMaximisationSolution> (validationWorldSpaceMesh, validationContainerWorldSpaceMesh);
        finalSolution->getItemWorldSpaceMesh()->setModelTransformation(correctedTransformation);
        bool feasible = finalSolution->isFeasible();

        // Infeasible solutions are most likely rounding errors and are always resolved by scaling down the item to 0.9999 of its original size
        if(!feasible){

            std::cout << "Feasible: " << std::boolalpha << feasible << std::endl;
            Transformation resultTransformation = singleModelSolution->getItemWorldSpaceMesh()->getModelTransformation();
            unsigned int iterations = 0;
            while(!feasible && iterations < 100){
                iterations ++;
                resultTransformation.factorScale(1 - 1e-4f);
                correctedTransformation = resultTransformation;
                correctedTransformation.factorScale(normalizingScaleFactor);
                correctedTransformation.setPosition(resultTransformation.transformVertex(normalizingTransformation.getPosition()));
                correctedTransformation.deltaPosition(validationContainer->getBounds().getCenter());
                finalSolution->getItemWorldSpaceMesh()->setModelTransformation(correctedTransformation);
                feasible = finalSolution->isFeasible();
                std::cout << "Feasible: " << std::boolalpha << feasible << std::endl;
            }
        }
        assert(finalSolution->isFeasible());

        // Logging results
        this->result = finalSolution;
        this->durationMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    }

    this->notifyObserversSolution(singleModelSolution);
}

float VolumeMaximisationTask::getRotationStepSize() const {
    return rotationStepSize;
}

void VolumeMaximisationTask::setRotationStepSize(float newRotationStepSize) {
    rotationStepSize = newRotationStepSize;
}

float VolumeMaximisationTask::getRelativeTranslationStepSize() const {
    return relativeTranslationStepSize;
}

void VolumeMaximisationTask::setRelativeTranslationStepSize(float newRelativeTranslationStepSize) {
    relativeTranslationStepSize = newRelativeTranslationStepSize;
}

float VolumeMaximisationTask::getStepSizeReductionFactor() const {
    return stepSizeReductionFactor;
}

void VolumeMaximisationTask::setStepSizeReductionFactor(float newStepSizeReductionFactor) {
    stepSizeReductionFactor = newStepSizeReductionFactor;
}

float VolumeMaximisationTask::getMinimumStepSize() const {
    return minimumStepSize;
}

void VolumeMaximisationTask::setMinimumStepSize(float newMinimumStepSize) {
    minimumStepSize = newMinimumStepSize;
}

float VolumeMaximisationTask::getMinimumDeltaScale() const {
    return minimumDeltaScale;
}

void VolumeMaximisationTask::setMinimumDeltaScale(float newMinimumDeltaScale) {
    minimumDeltaScale = newMinimumDeltaScale;
}

const std::shared_ptr<SingleVolumeMaximisationSolution> & VolumeMaximisationTask::getResult() const {
    return result;
}

long long VolumeMaximisationTask::getDurationMilliseconds() const {
    return this->durationMilliseconds;
}


