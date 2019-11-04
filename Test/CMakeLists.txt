project(LETest CXX)


################################################################################
# Target
################################################################################
set(SOURCES Main.cpp)
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
set(EXE_NAME ${PROJECT_NAME}-$<CONFIG>${EXE_SUFFIX})
add_le_executable(${PROJECT_NAME} ${EXE_NAME} "${SOURCES}")

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Runtime")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_PATH} CACHE STRING "" FORCE)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_PATH} CACHE STRING "" FORCE)
endforeach()
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_INSTALL_RPATH_USE_ORIGIN ON)
	set(CMAKE_INSTALL_RPATH "$ORIGIN/Lib")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(CMAKE_INSTALL_NAME_DIR @executable_path)
	set(CMAKE_INSTALL_RPATH "@executable_path/Lib")
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	LittleEngine
)

################################################################################
# Libraries
################################################################################
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} PUBLIC
		stdc++
	)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC
	LittleEngine
)
