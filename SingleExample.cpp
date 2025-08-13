//
// Created by Jonas on 6/06/2023.
//

#include "VolumeMaximisationTask.h"
#include "meshcore/rendering/ApplicationWindow.h"

int main(int argc, char *argv[]) {

    auto containerPath = MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Containers/rock_008k.obj");
    auto itemPath = MESHCORE_DATA_DIR + std::string("Tollenaere, J. et al/Items/star.stl");

    VolumeMaximisationTask singleTask(containerPath, itemPath);

    QApplication app(argc, argv);
    ApplicationWindow window;
    window.show();
    RenderWidget *renderWidget = window.getRenderWidget();
    renderWidget->observeTask(&singleTask);
    singleTask.start();
    int returnCode = QApplication::exec();
    singleTask.stop();
    singleTask.join();
    return returnCode;
}

