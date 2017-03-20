project(uipf-sfm-modules)

#
# Filters
#

add_library(SfMFilterImageGraphModule SHARED filters/FilterImageGraphModule.cpp)
target_link_libraries(SfMFilterImageGraphModule ${ModuleBase} opencv_core opencv_highgui opencv_imgproc opencv_features2d)


#
# IO
#

add_library(SfMReadNVMModule SHARED io/ReadNVMModule.cpp)
target_link_libraries(SfMReadNVMModule ${ModuleBase})

add_library(SfMSaveNVMModule SHARED io/SaveNVMModule.cpp)
target_link_libraries(SfMSaveNVMModule ${ModuleBase})


add_library(SfMOpenCVMat2ImageModule SHARED io/SfMOpenCVMat2SfMImage.cpp)
target_link_libraries(SfMOpenCVMat2ImageModule ${ModuleBase} uipf-sfm)

add_library(SfMImage2OpenCVMatModule SHARED io/SfMImage2OpenCVMat.cpp)
target_link_libraries(SfMImage2OpenCVMatModule ${ModuleBase} uipf-sfm)

add_library(SfMLoadImageModule SHARED io/SfMLoadImage.cpp)
target_link_libraries(SfMLoadImageModule ${ModuleBase} uipf-sfm)

add_library(SfMLoadImagesModule SHARED io/SfMLoadImages.cpp)
target_link_libraries(SfMLoadImagesModule ${ModuleBase} uipf-sfm)

add_library(SfMCopyImage SHARED io/SfMCopyImage.cpp)
target_link_libraries(SfMCopyImage ${ModuleBase} uipf-sfm)


add_library(SfMLoadPointCloud SHARED io/SfMLoadPointCloud.cpp)
target_link_libraries(SfMLoadPointCloud ${ModuleBase} uipf-sfm)


#
# Keypoints
#

add_library(SfMFREAKModule SHARED keypoints/opencv/SfMFREAKModule.cpp)
target_link_libraries(SfMFREAKModule ${ModuleBase} opencv_core opencv_features2d)

add_library(SfMFASTModule SHARED keypoints/opencv/SfMFASTModule.cpp)
target_link_libraries(SfMFASTModule ${ModuleBase} opencv_core opencv_features2d)

add_library(SfMORBModule SHARED keypoints/opencv/SfMORBModule.cpp)
target_link_libraries(SfMORBModule ${ModuleBase} opencv_core opencv_features2d)

if (ENABLE_NONFREE)
	add_library(SfMSIFTModule SHARED keypoints/opencv/SfMSIFTModule.cpp)
	target_link_libraries(SfMSIFTModule ${ModuleBase} opencv_core opencv_features2d)
	add_library(SfMSURFModule SHARED keypoints/opencv/SfMSURFModule.cpp)
	target_link_libraries(SfMSURFModule ${ModuleBase} opencv_core opencv_features2d)
endif()

#
# Keypoint Matchers
#

add_library(SfMFlannBasedMatcherModule SHARED keypoints/opencv/SfMFlannBasedMatcherModule.cpp)
target_link_libraries(SfMFlannBasedMatcherModule ${ModuleBase} opencv_core opencv_features2d)

add_library(SfMRANSACFilter SHARED keypoints/opencv/SfMRANSACFilter.cpp)
target_link_libraries(SfMRANSACFilter ${ModuleBase} opencv_core opencv_features2d opencv_calib3d)



# installation targets
install(TARGETS
	# filters
	SfMFilterImageGraphModule

	# io
	SfMReadNVMModule
	SfMSaveNVMModule

	SfMOpenCVMat2ImageModule
	SfMImage2OpenCVMatModule
	SfMLoadImageModule
	SfMLoadImagesModule
	SfMCopyImage

	SfMLoadPointCloud

	# Keypoints
	SfMFREAKModule
	SfMFASTModule
	SfMORBModule

	# Keypoint Matchers
	SfMFlannBasedMatcherModule
	SfMRANSACFilter

	LIBRARY DESTINATION lib/uipf
	ARCHIVE DESTINATION lib/uipf
)

if (ENABLE_NONFREE)
	install(TARGETS
		SfMSIFTModule
		SfMSURFModule
	LIBRARY DESTINATION lib/uipf
	ARCHIVE DESTINATION lib/uipf
	)
endif()
