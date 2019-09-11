project(Core CXX)

################################################################################
# Generated sources
################################################################################
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/Core/BuildVersion.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Core/BuildVersion.h"
)

set(EXTRA_FILES Core/BuildVersion.h)
set_source_files_properties(${EXTRA_FILES} PROPERTIES GENERATED TRUE)

################################################################################
# Target
################################################################################
add_little_engine_static_library(${EXTRA_FILES})
recursive_include("${CMAKE_SOURCE_DIR}/ThirdParty/Include")

# Include build directory for configured/generated files
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Core")

################################################################################
# Libraries
################################################################################
target_link_directories(${PROJECT_NAME} PUBLIC "${BUILD_THIRD_PARTY_PATH}/Lib")
target_link_libraries(${PROJECT_NAME} PUBLIC
	physfs$<$<CONFIG:Debug>:-d>
)
set_target_platform_libraries()
