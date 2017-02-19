cmake_minimum_required(VERSION 3.0.0)
project(uipf-sfm-ply)

if(UNIX)
	#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

	# sanitize memory issues https://gcc.gnu.org/gcc-4.8/changes.html
	SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
endif()

message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")

include_directories(${uipf-sfm-ply_SOURCE_DIR})

add_library(uipf-sfm-ply SHARED
	ply.h
	plyfile.c
)

install(FILES ply.h
		DESTINATION include/uipf-sfm
)
# installation targets
install(TARGETS uipf-sfm-ply
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
