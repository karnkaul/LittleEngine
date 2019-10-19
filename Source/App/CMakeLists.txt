if(MSVC)
	project(App CXX RC)
else()
	project(App CXX)
endif()

################################################################################
# Source groups
################################################################################
## Code
set(SOURCES
	"App/Main.cpp"
)

## Platform Resources
set(GENERATED_SOURCES "")
set(GENERATED_RESOURCES "")
if(PLATFORM STREQUAL "Win64")
	list(APPEND GENERATED_SOURCES
		App/Resources/Win64/App.rc
	)
	list(APPEND GENERATED_RESOURCES
		App/Resources/Win64/Icon.ico
	)
endif()
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)

## Generated
list(APPEND GENERATED_SOURCES
	App/Resources/Linux/.desktop
	App/Resources/OSX/Info.plist
)
copy_files_src_to_bin("${GENERATED_RESOURCES}")
configure_files_src_to_bin("${GENERATED_SOURCES}")
set(GENERATED_FILES ${GENERATED_SOURCES} ${GENERATED_RESOURCES})

################################################################################
# Target
################################################################################
list(TRANSFORM GENERATED_FILES PREPEND "${CMAKE_CURRENT_BINARY_DIR}/")
source_group(App "${CMAKE_CURRENT_BINARY_DIR}" FILES ${GENERATED_FILES})
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
set(ALL_FILES ${SOURCES} ${GENERATED_FILES})
set(EXE_NAME LEDemo$<$<NOT:$<CONFIG:Release>>:-$<CONFIG>>${EXE_SUFFIX})
add_le_executable(${EXE_NAME} "${ALL_FILES}")
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
