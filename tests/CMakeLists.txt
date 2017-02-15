

# find Boost
# http://cpprocks.com/using-cmake-to-build-a-cross-platform-project-with-a-boost-dependency/
find_package(Boost 1.55 COMPONENTS filesystem system REQUIRED)
message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include(FindPkgConfig)
pkg_search_module (GLIBMM REQUIRED glibmm-2.4)
include_directories(${GLIBMM_INCLUDE_DIRS})
link_directories(${GLIBMM_LIBRARY_DIRS})


find_library(
	libuipf uipf
	PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
message("libuipf: ${libuipf}")
find_library(
	libuipf-yaml uipf-yaml-cpp
	PATHS ${CMAKE_INSTALL_PREFIX}/lib
)
message("libuipf-yaml: ${libuipf-yaml}")

add_executable(KeyPointListTest KeyPointListTest.cpp)
target_link_libraries(KeyPointListTest ${ModuleBase} ${libuipf} ${libuipf-yaml} ${GLIBMM_LIBRARIES} ${Boost_LIBRARIES} opencv_core)
