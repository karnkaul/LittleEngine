if(MSVC)
	project(App CXX RC)
else()
	project(App CXX)
endif()

################################################################################
# Source groups
################################################################################
## Code
file(GLOB_RECURSE ALL_FILES
	"*.h"
	"*.cpp"
)

## Platform Resources
set(GENERATED_SOURCES "")
if(PLATFORM STREQUAL "Win64")
	list(APPEND GENERATED_SOURCES
		App/Resources/Win64/App.rc
	)
	list(APPEND ALL_FILES
		App/Resources/Win64/Icon.ico
	)
	configure_files_copyonly("App/Resources/Win64/Icon.ico")
endif()
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

## Generated
set(GENERATED_FILES
	${GENERATED_SOURCES}
	App/Resources/Linux/.desktop
	App/Resources/OSX/Info.plist
)
configure_files_src_to_bin("${GENERATED_FILES}")

list(APPEND ALL_FILES ${GENERATED_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ALL_FILES})

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

set(EXE_NAME LEDemo$<$<NOT:$<CONFIG:Ship>>:-$<CONFIG>>${EXE_SUFFIX})
output_name(${EXE_NAME})
output_directory("${BUILD_PATH}/Exe")
recursive_include("${CMAKE_CURRENT_SOURCE_DIR}")
add_target_compile_definitions()
set_target_compile_options()
set_target_link_options()
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../Runtime")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Game
)

################################################################################
# Libraries
################################################################################
target_link_libraries(${PROJECT_NAME} PUBLIC
	Game
)

################################################################################
# Install
################################################################################
install(TARGETS ${PROJECT_NAME}
	CONFIGURATIONS ${INSTALL_CONFIGS}
	RUNTIME DESTINATION "${RUNTIME_PATH}"
)
install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/App/Resources/Icon.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/App/Resources/OSX/Icon.icns"
	"${CMAKE_CURRENT_BINARY_DIR}/App/Resources/OSX/Info.plist"
	"${CMAKE_CURRENT_BINARY_DIR}/App/Resources/Linux/.desktop"
	DESTINATION "${RUNTIME_PATH}/Resources"
)
install_runtime(${EXE_NAME})
