cmake_minimum_required(VERSION 3.31)
project(sample-meshcore-through-cmake-fetchcontent)

set(CMAKE_CXX_STANDARD 20)

### fmt ###
find_package(fmt REQUIRED)
link_libraries(fmt::fmt)

### MeshCore ###
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
set(MESHCORE_BUILD_DEMOS OFF)
include(FetchContent)
set(CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
FetchContent_Declare(MeshCore
        GIT_REPOSITORY https://github.com/JonasTollenaere/MeshCore.git
        GIT_TAG        706be0c0eb507652f0e8f87495dcb6960f1d4569
)
FetchContent_MakeAvailable(MeshCore)

# Create a library for the shared code in this project, VMPBase
file(GLOB SOURCES src/*.cpp)
include_directories(include)
add_library(VMPBase STATIC ${SOURCES})
target_link_libraries(VMPBase PUBLIC MeshCoreBase fmt::fmt)
target_compile_definitions(VMPBase PUBLIC SVMP_RESULTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/results/")

add_executable(Benchmark Benchmark.cpp)
target_link_libraries(Benchmark PUBLIC VMPBase)

add_executable(SingleExample SingleExample.cpp)
target_link_libraries(SingleExample PUBLIC VMPBase MeshCore)

add_executable(NormalisationExample NormalisationExample.cpp)
target_link_libraries(NormalisationExample PUBLIC VMPBase MeshCore)

add_executable(SensitivityAnalysis SensitivityAnalysis.cpp)
target_link_libraries(SensitivityAnalysis PUBLIC VMPBase MeshCore)

add_executable(SensitivityAnalysis2 SensitivityAnalysis2.cpp)
target_link_libraries(SensitivityAnalysis2 PUBLIC VMPBase MeshCore)