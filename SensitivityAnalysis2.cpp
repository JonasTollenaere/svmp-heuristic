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

void sensitivityKappa(){
    std::ofstream csvStream(SVMP_RESULTS_DIR + std::string("sensitivityAnalysisKappa.csv"));
    csvStream << "kappa;averageRelativeScale;averageRelativeTime" << std::endl;

    std::vector<float> kappas = {1e0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8};

    for(const auto &kappa: kappas) {

        // Initialise a map to store the individual results
        std::unordered_map<std::string, std::unordered_map<std::string, std::tuple<float, float>>> resultsMap;

        // Iterate over all container - item combinations in the given folders
        for (const auto &containerEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Containers"))) {
            for (const auto &itemEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Items"))) {
                VolumeMaximisationTask task(containerEntry.path().string(), itemEntry.path().string());

                task.setMinimumDeltaScale(kappa);

                // Execute the task
                task.run();

                auto seconds = static_cast<float>(task.getDurationMilliseconds())/1000.0f;

                auto containerName = containerEntry.path().filename().string();
                auto itemName = itemEntry.path().filename().string();

                auto scale = task.getResult()->getItemWorldSpaceMesh()->getModelTransformation().getScale();

                resultsMap[containerName][itemName] = std::tuple(scale, seconds);

            }
        }

        // Parse baseline csv
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

            float relativeScale = std::get<0>(resultsMap[container][item]) / baselineScale;
            float relativeTime = std::get<1>(resultsMap[container][item]) / baselineTime;

            std::cout << "Relative scale: " << relativeScale << " for " << item << " in " << container << std::endl;

            if (relativeScale > 2.5 || relativeScale == 0.0) {
                std::cin.get();
            }

            entries++;
            averageRelativeScale += relativeScale;
            averageRelativeTime += relativeTime;
        }
        averageRelativeScale /= entries;
        averageRelativeTime /= entries;

        std::cout << "Average relative scale: " << averageRelativeScale << std::endl;
        std::cout << "Average relative time: " << averageRelativeTime << std::endl;

        csvStream << kappa << ";" << averageRelativeScale << ";" << averageRelativeTime << std::endl;
    }

    csvStream.flush();
    csvStream.close();
}

void sensitivityMu(){
    std::ofstream csvStream(SVMP_RESULTS_DIR + std::string("sensitivityAnalysisMu.csv"));
    csvStream << "mu;averageRelativeScale;averageRelativeTime" << std::endl;

    std::vector<float> mus = {1e0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8};

    for(const auto &mu: mus) {

        // Initialise a map to store the individual results
        std::unordered_map<std::string, std::unordered_map<std::string, std::tuple<float, float>>> resultsMap;

        // Iterate over all container - item combinations in the given folders
        for (const auto &containerEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Containers"))) {
            for (const auto &itemEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Items"))) {
                VolumeMaximisationTask task(containerEntry.path().string(), itemEntry.path().string());
                task.setMinimumStepSize(mu);

                // Execute the task
                task.run();

                auto seconds = static_cast<float>(task.getDurationMilliseconds())/1000.0f;

                auto containerName = containerEntry.path().filename().string();
                auto itemName = itemEntry.path().filename().string();

                auto scale = task.getResult()->getItemWorldSpaceMesh()->getModelTransformation().getScale();

                resultsMap[containerName][itemName] = std::tuple(scale, seconds);

            }
        }

        // Parse baseline csv
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

            float relativeScale = std::get<0>(resultsMap[container][item]) / baselineScale;
            float relativeTime = std::get<1>(resultsMap[container][item]) / baselineTime;

            std::cout << "Relative scale: " << relativeScale << " for " << item << " in " << container << std::endl;

            if (relativeScale > 2.5 || relativeScale == 0.0) {
                std::cin.get();
            }

            entries++;
            averageRelativeScale += relativeScale;
            averageRelativeTime += relativeTime;
        }
        averageRelativeScale /= entries;
        averageRelativeTime /= entries;

        std::cout << "Average relative scale: " << averageRelativeScale << std::endl;
        std::cout << "Average relative time: " << averageRelativeTime << std::endl;

        csvStream << mu << ";" << averageRelativeScale << ";" << averageRelativeTime << std::endl;
    }

    csvStream.flush();
    csvStream.close();
}

void sensitivityEpsilon(){
//    std::ofstream csvStream("../../volumemaximisation/results/sensitivityAnalysisEpsilon.csv");
//    csvStream << "epsilon;averageRelativeScale;averageRelativeTime" << std::endl;
//
//    std::vector<float> epsilons = {1e0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8};
//
//    for(const auto &epsilon: epsilons) {
//
        // Initialise a map to store the individual results
        std::unordered_map<std::string, std::unordered_map<std::string, std::tuple<float, float>>> resultsMap;

        // Iterate over all container - item combinations in the given folders
        for (const auto &containerEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Containers"))) {
            for (const auto &itemEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Items"))) {
                VolumeMaximisationTask task(containerEntry.path().string(), itemEntry.path().string());

                //        task.setInflatingPrecision(epsilon);

                // Execute the task
                task.run();

                auto seconds = static_cast<float>(task.getDurationMilliseconds())/1000.0f;

                auto containerName = containerEntry.path().filename().string();
                auto itemName = itemEntry.path().filename().string();

                auto scale = task.getResult()->getItemWorldSpaceMesh()->getModelTransformation().getScale();

                resultsMap[containerName][itemName] = std::tuple(scale, seconds);

            }
        }

        // Parse baseline csv
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

            float relativeScale = std::get<0>(resultsMap[container][item]) / baselineScale;
            float relativeTime = std::get<1>(resultsMap[container][item]) / baselineTime;

            std::cout << "Relative scale: " << relativeScale << " for " << item << " in " << container << std::endl;

            if (relativeScale > 2.5 || relativeScale == 0.0) {
                std::cin.get();
            }

            entries++;
            averageRelativeScale += relativeScale;
            averageRelativeTime += relativeTime;
        }
        averageRelativeScale /= entries;
        averageRelativeTime /= entries;

        std::cout << "Average relative scale: " << averageRelativeScale << std::endl;
        std::cout << "Average relative time: " << averageRelativeTime << std::endl;
//
//        csvStream << epsilon << ";" << averageRelativeScale << ";" << averageRelativeTime << std::endl;
//    }
//
//    csvStream.flush();
//    csvStream.close();
}

int main(){

    sensitivityKappa();
    sensitivityMu();
    sensitivityEpsilon();

    return 0;

}

