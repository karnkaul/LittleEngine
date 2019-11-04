project(Core CXX)

################################################################################
# Submodule dependencies
################################################################################
set(DEEP_SUBMODULE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Core/Game/Asserts/SysDialog/SysDialog.cmake")
if(NOT EXISTS "${DEEP_SUBMODULE_FILE}")
	message(FATAL_ERROR "Missing required files! Please ensure git submodules are up to date!\n[${DEEP_SUBMODULE_FILE}]")
endif()

################################################################################
# Sources
################################################################################
set(BUILD_VERSION_H "${CMAKE_CURRENT_BINARY_DIR}/Core/BuildVersion.h")
configure_file("${CMAKE_CURRENT_LIST_DIR}/Core/BuildVersion.h.in" "${BUILD_VERSION_H}")
set(GENERATED_SOURCES "${BUILD_VERSION_H}")
set(SOURCES
	Core/CoreTypes.h
	Core/Game/ArchiveReader.h
	Core/Game/ArchiveReader.cpp
	Core/Game/Deferred.h
	Core/Game/FileRW.h
	Core/Game/FileRW.cpp
	Core/Game/GData.h
	Core/Game/GData.cpp
	Core/Game/Jobs.h
	Core/Game/Jobs.cpp
	Core/Game/Jobs/JobCatalog.h
	Core/Game/Jobs/JobCatalog.cpp
	Core/Game/Jobs/JobHandle.h
	Core/Game/Jobs/JobHandle.cpp
	Core/Game/Jobs/JobWorker.h
	Core/Game/Jobs/JobWorker.cpp
	Core/Game/Jobs/JobManager.h
	Core/Game/Jobs/JobManager.cpp
	Core/Game/LEMacros.h
	Core/Game/NoCopy.h
	Core/Game/OS.h
	Core/Game/OS.cpp
	Core/Game/Property.h
	Core/Game/Property.cpp
	Core/Game/RingBuffer.h
	Core/Game/TRange.h
)
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
include("${CMAKE_CURRENT_LIST_DIR}/Core/Game/Asserts/Asserts.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Core/Game/Delegate/Delegate.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Core/Game/GameTypes/GameTypes.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Core/Game/Logger/Logger.cmake")
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

################################################################################
# Target
################################################################################
set(ALL_SOURCES ${SOURCES} ${GENERATED_SOURCES})
source_group(Core "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GENERATED_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_le_static_library(${PROJECT_NAME} "${ALL_SOURCES}")
# Add ThirdParty interface root to include path
recursive_include(${PROJECT_NAME} "${THIRD_PARTY_INCLUDE_PATHS}")
# Add binary directory (generated files) to include path
recursive_include(${PROJECT_NAME} "${CMAKE_CURRENT_BINARY_DIR}")

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	physfs-static
)

################################################################################
# Libraries
################################################################################
target_link_directories(${PROJECT_NAME} PUBLIC "${LIBRARIES_PATH}")
target_link_libraries(${PROJECT_NAME} PUBLIC
	physfs$<$<CONFIG:Debug>:-d>
)
set_target_platform_libraries(${PROJECT_NAME})
