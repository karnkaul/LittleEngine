project(Engine CXX)

################################################################################
# Target
################################################################################
add_little_engine_static_library("")

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Core
)

################################################################################
# Libraries
################################################################################
target_link_directories(${PROJECT_NAME} PUBLIC "${BUILD_THIRD_PARTY_PATH}/Lib")
target_link_libraries(${PROJECT_NAME} PUBLIC
	sfml-graphics$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-window$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-audio$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-system$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>

	Core
)
set_target_platform_libraries()
