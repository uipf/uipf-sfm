project(uipf-sfm-converters)

#
# Point detectors
#

add_library(SfMOpenCVMat2ImageModule SHARED OpenCVMat2SfMImage.cpp)
target_link_libraries(SfMOpenCVMat2ImageModule ${ModuleBase}) # opencv_core opencv_highgui

add_library(SfMImage2OpenCVMatModule SHARED SfMImage2OpenCVMat.cpp)
target_link_libraries(SfMImage2OpenCVMatModule ${ModuleBase}) # opencv_core opencv_highgui

add_library(SfMLoadImageModule SHARED SfMLoadImage.cpp)
target_link_libraries(SfMLoadImageModule ${ModuleBase}) # opencv_core opencv_highgui

add_library(SfMLoadImagesModule SHARED SfMLoadImages.cpp)
target_link_libraries(SfMLoadImagesModule ${ModuleBase}) # opencv_core opencv_highgui


# installation targets
install(TARGETS
		SfMOpenCVMat2ImageModule
		SfMImage2OpenCVMatModule
		SfMLoadImageModule
		SfMLoadImagesModule

		LIBRARY DESTINATION lib/uipf
		ARCHIVE DESTINATION lib/uipf
)
