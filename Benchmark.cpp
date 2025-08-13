//
// Created by Jonas on 7/03/2023.
//

#include <fstream>
#include "VolumeMaximisationTask.h"
#include <meshcore/utility/FileParser.h>
#include <nlohmann/json.hpp>

#include <fmt/core.h>

#include "cmake-build-debug/_deps/meshcore-src/include/meshcore/optimization/SingleVolumeMaximisationSolution.h"

int main() {

    // Initialise stream to output .csv file
    std::ofstream csvStream(SVMP_RESULTS_DIR + std::string("volumeMaximisationResults.csv"));
    std::string csvDelimiter = ";";
    csvStream.imbue(std::locale(""));
    csvStream << "Container" << csvDelimiter << "Item" << csvDelimiter << "name" << csvDelimiter << "bestScale" << csvDelimiter << "time(ms)" << csvDelimiter << "yaw" << csvDelimiter << "pitch" << csvDelimiter << "roll" << csvDelimiter << "x" << csvDelimiter << "y" << csvDelimiter << "z" << csvDelimiter << "volume" << std::endl;

    // Iterate over all container - item combinations in the given folders
    for (const auto &containerEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Containers"))) {
        for (const auto &itemEntry :  std::filesystem::directory_iterator(MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Items"))) {
            VolumeMaximisationTask task(containerEntry.path().string(), itemEntry.path().string());
            task.run();

            auto solution = task.getResult();
            auto scale = solution->getItemWorldSpaceMesh()->getModelTransformation().getScale();

            auto format_number = [](double value) {
                std::string s = fmt::format("{:.3f}", value);
                std::replace(s.begin(), s.end(), '.', ',');
                return s;
            };

            csvStream << fmt::format("{};{};{};{};{};{};{};{};{};{};{}\n",
                            solution->getContainerWorldSpaceMesh()->getModelSpaceMesh()->getName(),
                            solution->getItemWorldSpaceMesh()->getModelSpaceMesh()->getName(),
                            format_number(solution->getItemWorldSpaceMesh()->getModelTransformation().getScale()),
                            format_number(static_cast<float>(task.getDurationMilliseconds())/1000.0f),
                            format_number(solution->getItemWorldSpaceMesh()->getModelTransformation().getRotation().getYaw()),
                            format_number(solution->getItemWorldSpaceMesh()->getModelTransformation().getRotation().getPitch()),
                            format_number(solution->getItemWorldSpaceMesh()->getModelTransformation().getRotation().getRoll()),
                            format_number(solution->getItemWorldSpaceMesh()->getModelTransformation().getPosition().x),
                            format_number(solution->getItemWorldSpaceMesh()->getModelTransformation().getPosition().y),
                            format_number(solution->getItemWorldSpaceMesh()->getModelTransformation().getPosition().z),
                            format_number(solution->getItemWorldSpaceMesh()->getModelSpaceMesh()->getVolume() * scale * scale * scale)
                        );
        }
    }

    csvStream.close();
}
