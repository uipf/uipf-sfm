project(uipf-sfm-point-descriptors)

#
# Point detectors
#

# build sift module
add_library(SiftDescriptorModule SHARED SiftDescriptorModule.cpp)
target_link_libraries(SiftDescriptorModule ${ModuleBase})

# installation targets
#install(TARGETS
#		SiftDescriptorModule
#
#		LIBRARY DESTINATION lib/uipf
#		ARCHIVE DESTINATION lib/uipf
#		)
