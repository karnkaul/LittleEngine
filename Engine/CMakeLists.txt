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

set(MODEL_SOURCES
	Game/Model/GameConfig.cpp
	Game/Model/GameConfig.h
	Game/Model/GameKernel.cpp
	Game/Model/GameKernel.h
	Game/Model/GameManager.cpp
	Game/Model/GameManager.h
	Game/Model/GameSettings.cpp
	Game/Model/GameSettings.h
	Game/Model/Inheritable.cpp
	Game/Model/Inheritable.h
	
	Game/Model/UI/UIContext.cpp
	Game/Model/UI/UIContext.h
	Game/Model/UI/UIElement.cpp
	Game/Model/UI/UIElement.h
	Game/Model/UI/UIGameStyle.cpp
	Game/Model/UI/UIGameStyle.h
	Game/Model/UI/UIManager.cpp
	Game/Model/UI/UIManager.h
	Game/Model/UI/UIObject.cpp
	Game/Model/UI/UIObject.h
	Game/Model/UI/UIStyle.h
	Game/Model/UI/UIText.cpp
	Game/Model/UI/UIText.h
	Game/Model/UI/UITransform.cpp
	Game/Model/UI/UITransform.h
	Game/Model/UI/UIWidget.cpp
	Game/Model/UI/UIWidget.h
	Game/Model/UI/UIWidgetMatrix.cpp
	Game/Model/UI/UIWidgetMatrix.h
	
	Game/Model/World/Camera.cpp
	Game/Model/World/Camera.h
	Game/Model/World/Component.cpp
	Game/Model/World/Component.h
	Game/Model/World/ComponentTimingType.h
	Game/Model/World/Entity.cpp
	Game/Model/World/Entity.h
	Game/Model/World/GameObject.cpp
	Game/Model/World/GameObject.h
	Game/Model/World/ILoadingHUD.h
	Game/Model/World/LoadingHUD.cpp
	Game/Model/World/LoadingHUD.h
	Game/Model/World/TileMap.cpp
	Game/Model/World/TileMap.h
	Game/Model/World/TokenHandler.h
	Game/Model/World/World.cpp
	Game/Model/World/World.h
	Game/Model/World/WorldClock.cpp
	Game/Model/World/WorldClock.h
	Game/Model/World/WorldStateMachine.cpp
	Game/Model/World/WorldStateMachine.h
)

set(FRAMEWORK_SOURCES
	Game/Framework/Components/CollisionComponent.cpp
	Game/Framework/Components/CollisionComponent.h
	Game/Framework/Components/Components.h
	Game/Framework/Components/ProjectileComponent.cpp
	Game/Framework/Components/ProjectileComponent.h
	Game/Framework/Components/RenderComponent.cpp
	Game/Framework/Components/RenderComponent.h

	Game/Framework/Entities/Entities.h
	Game/Framework/Entities/EntityPool.cpp
	Game/Framework/Entities/EntityPool.h
	Game/Framework/Entities/ParticleSystem.cpp
	Game/Framework/Entities/ParticleSystem.h

	Game/Framework/UI/Containers/UIContainer.cpp
	Game/Framework/UI/Containers/UIContainer.h
	Game/Framework/UI/Contexts/UIButtonDrawer.cpp
	Game/Framework/UI/Contexts/UIButtonDrawer.h
	Game/Framework/UI/Contexts/UIDialogue.cpp
	Game/Framework/UI/Contexts/UIDialogue.h
	Game/Framework/UI/Elements/UIProgressBar.cpp
	Game/Framework/UI/Elements/UIProgressBar.h
	Game/Framework/UI/UI.h
	Game/Framework/UI/Widgets/UIButton.cpp
	Game/Framework/UI/Widgets/UIButton.h
	Game/Framework/UI/Widgets/UISelection.cpp
	Game/Framework/UI/Widgets/UISelection.h
	Game/Framework/UI/Widgets/UITextInput.cpp
	Game/Framework/UI/Widgets/UITextInput.h
	Game/Framework/UI/Widgets/UIToggle.cpp
	Game/Framework/UI/Widgets/UIToggle.h
	
	Game/Framework/Utility/Debug/Console/ConsoleInput.cpp
	Game/Framework/Utility/Debug/Console/ConsoleInput.h
	Game/Framework/Utility/Debug/Console/ConsoleRenderer.cpp
	Game/Framework/Utility/Debug/Console/ConsoleRenderer.h
	Game/Framework/Utility/Debug/Console/DebugCommands.cpp
	Game/Framework/Utility/Debug/Console/DebugCommands.h
	Game/Framework/Utility/Debug/Console/DebugConsole.cpp
	Game/Framework/Utility/Debug/Console/DebugConsole.h
	Game/Framework/Utility/Debug/Console/LogLine.cpp
	Game/Framework/Utility/Debug/Console/LogLine.h
	Game/Framework/Utility/Debug/DebugProfiler.cpp
	Game/Framework/Utility/Debug/DebugProfiler.h
	Game/Framework/Utility/Debug/RenderStatsRenderer.cpp
	Game/Framework/Utility/Debug/RenderStatsRenderer.h
	Game/Framework/Utility/ParticleSystem/PSData.cpp
	Game/Framework/Utility/ParticleSystem/PSData.h
	Game/Framework/Utility/ParticleSystem/PSEmitter.cpp
	Game/Framework/Utility/ParticleSystem/PSEmitter.h
	Game/Framework/Utility/ProgressBar.cpp
	Game/Framework/Utility/ProgressBar.h

	Game/Framework/Framework.h
)

set(GAME_SOURCES
	${MODEL_SOURCES}
	${FRAMEWORK_SOURCES}
	Game/GameLoop.h
	Game/GameLoop.cpp
)

set(SOURCES
	${SFMLAPI_SOURCES}
	${ENGINE_SOURCES}
	${MODEL_SOURCES}
	${GAME_SOURCES}
)

################################################################################
# Target
################################################################################
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_le_static_library("${SOURCES}")
output_name(LittleEngine-$<CONFIG>)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	sfml-graphics
	sfml-window
	sfml-audio
	sfml-system
	Core
)

################################################################################
# Libraries
################################################################################
target_link_directories(${PROJECT_NAME} PUBLIC "${LIBRARIES_PATH}")
target_link_libraries(${PROJECT_NAME} PUBLIC
	sfml-graphics$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-window$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-audio$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	sfml-system$<$<BOOL:${SFML_STATIC_LIBS}>:-s>$<$<CONFIG:DEBUG>:-d>
	Core
)
set_target_platform_libraries()
