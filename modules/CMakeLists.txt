project(uipf-sfm-modules)

#
# IO
#

add_library(ReadNVMModule SHARED io/ReadNVMModule.cpp)
target_link_libraries(ReadNVMModule ${ModuleBase})

add_library(SaveNVMModule SHARED io/SaveNVMModule.cpp)
target_link_libraries(SaveNVMModule ${ModuleBase})

# installation targets
install(TARGETS
	ReadNVMModule
	SaveNVMModule

	LIBRARY DESTINATION lib/uipf
	ARCHIVE DESTINATION lib/uipf
)
