project(uipf-sfm-point-detectors)

#
# Point detectors
#

# build sift module
add_library(SiftDetectorModule SHARED SiftDetectorModule.cpp)
target_link_libraries(SiftDetectorModule ${ModuleBase})

# build foerstner module
add_library(FoerstnerModule SHARED FoerstnerModule.cpp foerstner/Foerstner.cpp)
target_link_libraries(FoerstnerModule ${ModuleBase} opencv_core opencv_highgui)

# build view point module
add_library(ViewKeyPointsModule SHARED ViewKeyPointsModule.cpp)
target_link_libraries(ViewKeyPointsModule ${ModuleBase} opencv_core opencv_highgui opencv_imgproc opencv_features2d)

add_library(ViewImageGraphModule SHARED ViewImageGraphModule.cpp)
target_link_libraries(ViewImageGraphModule ${ModuleBase} opencv_core opencv_highgui opencv_imgproc opencv_features2d)

add_library(FilterImageGraphModule SHARED FilterImageGraphModule.cpp)
target_link_libraries(FilterImageGraphModule ${ModuleBase} opencv_core opencv_highgui opencv_imgproc opencv_features2d)

# installation targets
install(TARGETS
		SiftDetectorModule
		FoerstnerModule
		ViewKeyPointsModule
		ViewImageGraphModule
		FilterImageGraphModule

		LIBRARY DESTINATION lib/uipf
		ARCHIVE DESTINATION lib/uipf
)
