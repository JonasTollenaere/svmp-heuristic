//
// Created by Jonas on 12/04/2022.
//

#ifndef OPTIXMESHCORE_VOLUME_MAXIMISATION_TASK_H
#define OPTIXMESHCORE_VOLUME_MAXIMISATION_TASK_H

#include "meshcore/tasks/AbstractTask.h"
#include "meshcore/core/WorldSpaceMesh.h"
#include <glm/ext/scalar_constants.hpp>

class SingleVolumeMaximisationSolution;

class VolumeMaximisationTask: public AbstractTask {

    std::string containerPath;
    std::string itemPath;

    float stepSizeReductionFactor  = 0.6f;
    float rotationStepSize = glm::pi<float>()/2.0f;
    float relativeTranslationStepSize = 0.25f;

    float minimumStepSize = 1e-4f; // µ
    float minimumDeltaScale = 1e-4f; // kappa

    std::shared_ptr<SingleVolumeMaximisationSolution> result;
    long long durationMilliseconds;

public:

    VolumeMaximisationTask(const std::string& containerPath,const std::string& itemPath): containerPath(containerPath), itemPath(itemPath) {}

    void run() override;

    float getRotationStepSize() const;
    void setRotationStepSize(float newRotationStepSize);

    float getRelativeTranslationStepSize() const;
    void setRelativeTranslationStepSize(float newRelativeTranslationStepSize);

    float getStepSizeReductionFactor() const;
    void setStepSizeReductionFactor(float stepSizeReductionFactor);;

    float getMinimumStepSize() const;
    void setMinimumStepSize(float minimumStepSize);

    float getMinimumDeltaScale() const;
    void setMinimumDeltaScale(float minimumDeltaScale);

    const std::shared_ptr<SingleVolumeMaximisationSolution>& getResult() const;
    long long getDurationMilliseconds() const;
};


#endif //OPTIXMESHCORE_VOLUME_MAXIMISATION_TASK_H
