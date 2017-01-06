cmake_minimum_required(VERSION 2.8.8)
project(uipf-sfm)

set(CMAKE_CXX_STANDARD 11)

# find libuipf-module
# search in normal system paths as well as installation prefix for installation as unprivileged user.
find_library(
	ModuleBase uipf-module
	PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
message("ModuleBase: ${ModuleBase}")
include_directories(${CMAKE_INSTALL_PREFIX}/include)

include_directories(${PROJECT_SOURCE_DIR}/src)


# Point detectors
add_subdirectory(src/point-detectors)
add_subdirectory(src/point-descriptors)


install(DIRECTORY ${uipf-sfm_SOURCE_DIR}/src/data
		DESTINATION include/uipf-sfm/data
		FILES_MATCHING PATTERN "*.hpp"
)
