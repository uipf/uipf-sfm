cmake_minimum_required(VERSION 2.8.8)
project(uipf-sfm-libply)

if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")

	# sanitize memory issues https://gcc.gnu.org/gcc-4.8/changes.html
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
endif()

message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

include_directories(${uipf-sfm-libply_SOURCE_DIR})

add_library(uipf-sfm-libply SHARED
	ply/ply.hpp
	ply/ply_parser.cpp
)

#install(FILES ply.h
#		DESTINATION include/uipf-sfm
#)
# installation targets
install(TARGETS uipf-sfm-libply
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
