cmake_minimum_required(VERSION 3.0.0)
project(uipf-sfm)

#c++11 support
if(${CMAKE_VERSION} VERSION_LESS 3.1.0)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
else()
	set(CMAKE_CXX_STANDARD 11)
endif()

if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

	# sanitize memory issues https://gcc.gnu.org/gcc-4.8/changes.html
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
endif()

message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

# find libuipf-module
# search in normal system paths as well as installation prefix for installation as unprivileged user.
find_library(
	ModuleBase uipf-module
	PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
message("ModuleBase: ${ModuleBase}")
include_directories(${CMAKE_INSTALL_PREFIX}/include)

find_library(
	uipf-geomview uipf-geomview
	PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
message("uipf-geomview: ${uipf-geomview}")



include_directories(${PROJECT_SOURCE_DIR}/src)


add_library(uipf-sfm SHARED
	src/data/Image.cpp
)
#set_target_properties(uipf-sfm PROPERTIES
#	SOVERSION ${UIPFramework_VERSION_MAJOR}.${UIPFramework_VERSION_MINOR}
#	VERSION ${UIPFramework_VERSION}
#)
target_link_libraries(uipf-sfm ${uipf-geomview} opencv_core opencv_features2d)

# Point detectors
add_subdirectory(src/converters)
add_subdirectory(src/point-detectors)
add_subdirectory(src/point-descriptors)

add_subdirectory(tests)


install(DIRECTORY ${uipf-sfm_SOURCE_DIR}/src/data
		DESTINATION include/uipf-sfm
		FILES_MATCHING PATTERN "*.hpp"
)
# installation targets
install(TARGETS uipf-sfm
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)