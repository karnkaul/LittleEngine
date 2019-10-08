project(Engine CXX)

################################################################################
# Sources
################################################################################
set(SFMLAPI_SOURCES
	SFMLAPI/Audio/AudioPlayer.h
	SFMLAPI/Audio/AudioPlayer.cpp
	
	SFMLAPI/Input/InputDataFrame.h
	SFMLAPI/Input/InputDataFrame.cpp
	SFMLAPI/Input/InputHandler.h
	SFMLAPI/Input/InputHandler.cpp
	SFMLAPI/Input/InputMappings.h
	SFMLAPI/Input/InputMappings.cpp
	SFMLAPI/Input/InputStateMachine.h
	SFMLAPI/Input/InputStateMachine.cpp

	SFMLAPI/Rendering/Colour.h
	SFMLAPI/Rendering/Colour.cpp
	SFMLAPI/Rendering/LayerID.h
	SFMLAPI/Rendering/Primitives.h
	SFMLAPI/Rendering/RenderStats.h
	SFMLAPI/Rendering/RenderStats.cpp
	SFMLAPI/Rendering/Shader.h
	SFMLAPI/Rendering/Shader.cpp

	SFMLAPI/Rendering/Primitives/Primitive.h
	SFMLAPI/Rendering/Primitives/Primitive.cpp
	SFMLAPI/Rendering/Primitives/Quad.h
	SFMLAPI/Rendering/Primitives/Quad.cpp
	SFMLAPI/Rendering/Primitives/Quads.h
	SFMLAPI/Rendering/Primitives/Quads.cpp
	SFMLAPI/Rendering/Primitives/SFCircle.h
	SFMLAPI/Rendering/Primitives/SFCircle.cpp
	SFMLAPI/Rendering/Primitives/SFDrawable.h
	SFMLAPI/Rendering/Primitives/SFDrawable.cpp
	SFMLAPI/Rendering/Primitives/SFRect.h
	SFMLAPI/Rendering/Primitives/SFRect.cpp
	SFMLAPI/Rendering/Primitives/SFText.h
	SFMLAPI/Rendering/Primitives/SFText.cpp

	SFMLAPI/System/Assets.h
	SFMLAPI/System/Assets.cpp
	SFMLAPI/System/AssetType.h
	SFMLAPI/System/SFTypes.h

	SFMLAPI/Viewport/Viewport.h
	SFMLAPI/Viewport/Viewport.cpp
	SFMLAPI/Viewport/ViewportData.h
	SFMLAPI/Viewport/ViewportData.cpp

	SFMLAPI/Input.h
	SFMLAPI/Rendering.h
	SFMLAPI/SFMLAPI.h
	SFMLAPI/System.h
	SFMLAPI/Viewport.h
)

set(ENGINE_SOURCES
	Engine/Audio/LEAudio.h
	Engine/Audio/LEAudio.cpp

	Engine/Context/LEContext.h
	Engine/Context/LEContext.cpp

	Engine/Debug/Profiler.h
	Engine/Debug/Profiler.cpp
	Engine/Debug/Tweakable.h
	Engine/Debug/Tweakable.cpp

	Engine/Input/KeyboardInput.h
	Engine/Input/KeyboardInput.cpp
	Engine/Input/LEInput.h
	Engine/Input/LEInput.cpp

	Engine/Locale/Locale.h
	Engine/Locale/Locale.cpp

	Engine/Physics/Collider.h
	Engine/Physics/Collider.cpp
	Engine/Physics/ColliderData.h
	Engine/Physics/ColliderData.cpp
	Engine/Physics/LEPhysics.h
	Engine/Physics/LEPhysics.cpp

	Engine/Rendering/LERenderer.h
	Engine/Rendering/LERenderer.cpp
	Engine/Rendering/LEShaders.h
	Engine/Rendering/LEShaders.cpp
	Engine/Rendering/PrimitiveFactory.h
	Engine/Rendering/PrimitiveFactory.cpp

	Engine/Repository/AssetManifest.h
	Engine/Repository/AssetManifest.cpp
	Engine/Repository/LERepository.h
	Engine/Repository/LERepository.cpp
	Engine/Repository/ManifestLoader.h
	Engine/Repository/ManifestLoader.cpp

	Engine/Engine.h
	Engine/FatalEngineException.h
	Engine/GFX.h
	Engine/GFX.cpp
)

set(SOURCES
	${SFMLAPI_SOURCES}
	${ENGINE_SOURCES}
)

################################################################################
# Target
################################################################################
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_le_static_library("${SOURCES}")

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Core
)

################################################################################
# Libraries
################################################################################
target_link_directories(${PROJECT_NAME} PUBLIC "${THIRD_PARTY_BUILD_PATH}/Lib")
target_link_libraries(${PROJECT_NAME} PUBLIC
	sfml-graphics$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-window$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-audio$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-system$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>

	Core
)
set_target_platform_libraries()
