if(MSVC)
	project(App CXX RC)
else()
	project(App CXX)
endif()

################################################################################
# Source groups
################################################################################
set(ALL_CODE
	App/Main.cpp
)

if(PLATFORM STREQUAL "Win64")
	set(IDE_RESOURCES_SOURCES
		App/Resources/MSVC/App.rc
	)
	set(IDE_RESOURCES_FILES
		App/Resources/MSVC/resource.h
		App/Resources/MSVC/Icon.ico)
	set(IDE_RESOURCES
		${IDE_RESOURCES_SOURCES}
		${IDE_RESOURCES_FILES}
	)
endif()

set(APP_RESOURCES
	App/Resources/Icon.png
)

set(ALL_FILES
	${ALL_CODE}
	${APP_RESOURCES}
	${IDE_RESOURCES}
)

set_source_files_properties(${IDE_RESOURCES} PROPERTIES GENERATED TRUE)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ALL_FILES})

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

set(RESOURCE_ROOT App/Resources)
set(RESOURCE_FILES
	MSVC/App.rc
	Linux/.desktop
	OSX/Info.plist
)

list(TRANSFORM RESOURCE_FILES PREPEND "${RESOURCE_ROOT}/")
configure_files_src_to_bin("${RESOURCE_FILES};${IDE_RESOURCES_SOURCES}")
configure_files_copyonly("${IDE_RESOURCES_FILES}")

################################################################################
# Executable configuration
################################################################################
output_name(LEDemo$<$<NOT:$<CONFIG:Ship>>:-$<CONFIG>>${EXE_SUFFIX})
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
install_runtime()
