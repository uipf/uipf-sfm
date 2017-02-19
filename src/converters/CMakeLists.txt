project(uipf-sfm-converters)

#
# Point detectors
#

add_library(SfMOpenCVMat2ImageModule SHARED SfMOpenCVMat2SfMImage.cpp)
target_link_libraries(SfMOpenCVMat2ImageModule ${ModuleBase} uipf-sfm) # opencv_core opencv_highgui

add_library(SfMImage2OpenCVMatModule SHARED SfMImage2OpenCVMat.cpp)
target_link_libraries(SfMImage2OpenCVMatModule ${ModuleBase} uipf-sfm) # opencv_core opencv_highgui

add_library(SfMLoadImageModule SHARED SfMLoadImage.cpp)
target_link_libraries(SfMLoadImageModule ${ModuleBase} uipf-sfm) # opencv_core opencv_highgui

add_library(SfMLoadImagesModule SHARED SfMLoadImages.cpp)
target_link_libraries(SfMLoadImagesModule ${ModuleBase} uipf-sfm) # opencv_core opencv_highgui

add_library(SfMCopyImage SHARED SfMCopyImage.cpp)
target_link_libraries(SfMCopyImage ${ModuleBase} uipf-sfm) # opencv_core opencv_highgui

add_library(SfMLoadPointCloud SHARED SfMLoadPointCloud.cpp)
target_link_libraries(SfMLoadPointCloud ${ModuleBase} uipf-sfm) # opencv_core opencv_highgui


# installation targets
install(TARGETS
		SfMOpenCVMat2ImageModule
		SfMImage2OpenCVMatModule
		SfMLoadImageModule
		SfMLoadImagesModule
		SfMCopyImage
		SfMLoadPointCloud

		LIBRARY DESTINATION lib/uipf
		ARCHIVE DESTINATION lib/uipf
)
