cmake_minimum_required(VERSION 2.8.8)
project(sfm-modules)

set(CMAKE_CXX_STANDARD 11)

# temporary code for finding libuipf
find_library(
	ModuleBase uipf-module
	PATHS /home/cebe/Dokumente/Uni/master/code/uipf/cmake-build-debug/prefix/lib
)
message("ModuleBase: ${ModuleBase}")
include_directories(/home/cebe/Dokumente/Uni/master/code/uipf/cmake-build-debug/prefix/include)


include_directories(${PROJECT_SOURCE_DIR}/src)


#
# Point detectors
#

# build example module
add_library(HarrisModule SHARED src/HarrisModule.cpp)
target_link_libraries(HarrisModule ${ModuleBase})

# build sift module
add_library(SiftDescriptorModule SHARED src/SiftDescriptorModule.cpp)
target_link_libraries(SiftDescriptorModule ${ModuleBase})

# build foerstner module
add_library(FoerstnerModule SHARED src/point-detectors/FoerstnerModule.cpp src/point-detectors/foerstner/Foerstner.cpp)
target_link_libraries(FoerstnerModule ${ModuleBase} opencv_core opencv_highgui)

# build foerstner module
add_library(ViewKeyPointsModule SHARED src/point-detectors/ViewKeyPointsModule.cpp)
target_link_libraries(ViewKeyPointsModule ${ModuleBase} opencv_core opencv_highgui opencv_imgproc opencv_features2d)

