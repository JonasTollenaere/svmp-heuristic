//
// Created by Jonas on 1/06/2023.
//

#include <meshcore/optimization/SingleVolumeMaximisationSolution.h>
#include "VolumeMaximisationTask.h"
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <unordered_map>

int main(){

    std::ofstream csvStream(SVMP_RESULTS_DIR + std::string("sensitivityAnalysis.csv"));
    csvStream << "stepsizeReductionFactor;relativeTranslationStepSize;relativeRotationStepSize;averageRelativeScale;averageRelativeTime" << std::endl;

    std::vector<float> stepsizeReductionFactors = {0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};

    for (const auto &stepsizeReductionFactor: stepsizeReductionFactors){

        for (int i = 0; i < 10; ++i){
            float relativeTranslationStepSize = 2.0f/float(std::pow(2.0f,i));
            for(int j=0; j < 10; j++){
                float relativeRotationStepSize = glm::pi<float>()/float(std::pow(2.0f,j));

                // Initialise a map to store the individual results
                std::unordered_map<std::string, std::unordered_map<std::string, std::tuple<float, float>>> resultsMap;

                // Iterate over all container - item combinations in the given folders
                for (const auto &containerEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Containers"))) {
                    for (const auto &itemEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Items"))) {
                        VolumeMaximisationTask task(containerEntry.path().string(), itemEntry.path().string());
                        task.setRelativeTranslationStepSize(relativeTranslationStepSize);
                        task.setRotationStepSize(relativeRotationStepSize);
                        task.setStepSizeReductionFactor(stepsizeReductionFactor);

                        // Execute the task
                        task.run();

                        auto seconds = static_cast<float>(task.getDurationMilliseconds())/1000.0f;

                        auto containerName = containerEntry.path().filename().string();
                        auto itemName = itemEntry.path().filename().string();

                        auto scale = task.getResult()->getItemWorldSpaceMesh()->getModelTransformation().getScale();

                        resultsMap[containerName][itemName] = std::tuple(scale, seconds);

                    }
                }

                // Parse baseline csv to compute relative scale and time
                std::ifstream baselineFile(SVMP_RESULTS_DIR + std::string("baseline.csv"));
                std::string line;
                std::getline(baselineFile, line); // Skip header
                auto entries = 0;
                auto averageRelativeScale = 0.0;
                auto averageRelativeTime = 0.0;
                while (std::getline(baselineFile, line)) {
                    // split at semicolon
                    std::stringstream ss(line);
                    std::string container;
                    std::getline(ss, container, ';');
                    std::string item;
                    std::getline(ss, item, ';');
                    std::string scale;
                    std::getline(ss, scale, ';');
                    // Replace comma with dot
                    std::replace(scale.begin(), scale.end(), ',', '.');
                    float baselineScale = std::stof(scale);
                    std::string time;
                    std::getline(ss, time, ';');
                    // Replace comma with dot
                    std::replace(time.begin(), time.end(), ',', '.');
                    float baselineTime = std::stof(time);

                    float relativeScale = std::get<0>(resultsMap[container][item])/baselineScale;
                    float relativeTime = std::get<1>(resultsMap[container][item])/baselineTime;

                    std::cout << "Relative scale: " << relativeScale << " for " << item << " in " << container << std::endl;

                    entries++;
                    averageRelativeScale += relativeScale;
                    averageRelativeTime += relativeTime;
                }
                averageRelativeScale /= entries;
                averageRelativeTime /= entries;

                std::cout << "Average relative scale: " << averageRelativeScale << std::endl;
                std::cout << "Average relative time: " << averageRelativeTime << std::endl;

                csvStream << stepsizeReductionFactor << ";" << relativeTranslationStepSize << ";" << relativeRotationStepSize << ";" << averageRelativeScale << ";" << averageRelativeTime << std::endl;
            }
        }
    }

    csvStream.flush();
    csvStream.close();
}

