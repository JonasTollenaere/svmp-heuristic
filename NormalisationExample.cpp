//
// Created by Jonas on 4/01/2024.
//

#include <QtWidgets>
#include "meshcore/rendering/ApplicationWindow.h"
#include "meshcore/utility/FileParser.h"

void run(RenderWidget* renderWidget);

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    ApplicationWindow window;
    window.show();

    std::thread thread(run, window.getRenderWidget());
    int returnCode = QApplication::exec();
    thread.join();
    return returnCode;
}

void run(RenderWidget* renderWidget){

    auto itemPath = MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Items/dagger.stl");
    auto innerModelMesh = FileParser::loadMeshFile(itemPath);

    // Item coordinate normalisation
    // 1. Place the center of the object in the origin of its coordinate system
    Transformation normalizingTransformation;
    normalizingTransformation.setPosition(-innerModelMesh->getBounds().getCenter());

    std::vector<Vertex> correctedVertices;
    correctedVertices.reserve(innerModelMesh->getVertices().size());
    for (auto &vertex : innerModelMesh->getVertices()){
        correctedVertices.emplace_back(normalizingTransformation.transformVertex(vertex));
    }
    auto name = innerModelMesh->getName();
    auto originalMesh = innerModelMesh;
    auto normalizedModelMesh = std::make_shared<ModelSpaceMesh>(correctedVertices, innerModelMesh->getTriangles());
    normalizedModelMesh->setName(name);

    // Render both the original and the normalized mesh at different scales
    for (int i = 0; i < 4; ++i){
        auto color = (1.0f - 0.33f * i) * Color(215.0f/255.0f,0,0,1) + 0.33f * float(i) * Color::Locust();

        auto scale = 0.8f* std::pow(2,i);
        auto originalWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(originalMesh);
        originalWorldSpaceMesh->getModelTransformation().setScale(scale);
        originalWorldSpaceMesh->getModelTransformation().setPosition(glm::vec3(0,100 + 100*i,0));
        renderWidget->renderWorldSpaceMesh("Original", originalWorldSpaceMesh,  Color(color.x, color.y, color.z, 1));

        auto normalizedWorldSpaceMesh = std::make_shared<WorldSpaceMesh>(normalizedModelMesh);
        normalizedWorldSpaceMesh->getModelTransformation().setScale(scale);
        normalizedWorldSpaceMesh->getModelTransformation().setPosition(glm::vec3(0,100 + 100*i,0));
        renderWidget->renderWorldSpaceMesh("Normalized", normalizedWorldSpaceMesh, Color(color.x, color.y, color.z, 1));
    }

}