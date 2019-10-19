project(Game CXX)

################################################################################
# Sources
################################################################################
set(FRAMEWORK_SOURCES
	Framework/Components/CollisionComponent.cpp
	Framework/Components/CollisionComponent.h
	Framework/Components/Components.h
	Framework/Components/ControllerComponent.cpp
	Framework/Components/ControllerComponent.h
	Framework/Components/ProjectileComponent.cpp
	Framework/Components/ProjectileComponent.h
	Framework/Components/RenderComponent.cpp
	Framework/Components/RenderComponent.h

	Framework/Entities/Entities.h
	Framework/Entities/EntityPool.cpp
	Framework/Entities/EntityPool.h
	Framework/Entities/ParticleSystem.cpp
	Framework/Entities/ParticleSystem.h
	Framework/Entities/Player.cpp
	Framework/Entities/Player.h

	Framework/UI/Containers/UIContainer.cpp
	Framework/UI/Containers/UIContainer.h
	Framework/UI/Containers/UIOptions.cpp
	Framework/UI/Containers/UIOptions.h
	Framework/UI/Contexts/UIButtonDrawer.cpp
	Framework/UI/Contexts/UIButtonDrawer.h
	Framework/UI/Contexts/UIDialogue.cpp
	Framework/UI/Contexts/UIDialogue.h
	Framework/UI/Elements/UIProgressBar.cpp
	Framework/UI/Elements/UIProgressBar.h
	Framework/UI/UI.h
	Framework/UI/Widgets/UIButton.cpp
	Framework/UI/Widgets/UIButton.h
	Framework/UI/Widgets/UISelection.cpp
	Framework/UI/Widgets/UISelection.h
	Framework/UI/Widgets/UITextInput.cpp
	Framework/UI/Widgets/UITextInput.h
	Framework/UI/Widgets/UIToggle.cpp
	Framework/UI/Widgets/UIToggle.h
	
	Framework/Utility/Debug/Console/ConsoleInput.cpp
	Framework/Utility/Debug/Console/ConsoleInput.h
	Framework/Utility/Debug/Console/ConsoleRenderer.cpp
	Framework/Utility/Debug/Console/ConsoleRenderer.h
	Framework/Utility/Debug/Console/DebugCommands.cpp
	Framework/Utility/Debug/Console/DebugCommands.h
	Framework/Utility/Debug/Console/DebugConsole.cpp
	Framework/Utility/Debug/Console/DebugConsole.h
	Framework/Utility/Debug/Console/LogLine.cpp
	Framework/Utility/Debug/Console/LogLine.h
	Framework/Utility/Debug/DebugProfiler.cpp
	Framework/Utility/Debug/DebugProfiler.h
	Framework/Utility/Debug/RenderStatsRenderer.cpp
	Framework/Utility/Debug/RenderStatsRenderer.h
	Framework/Utility/ParticleSystem/PSData.cpp
	Framework/Utility/ParticleSystem/PSData.h
	Framework/Utility/ParticleSystem/PSEmitter.cpp
	Framework/Utility/ParticleSystem/PSEmitter.h
	Framework/Utility/ProgressBar.cpp
	Framework/Utility/ProgressBar.h

	Framework/Framework.h
)

set(GAMEPLAY_SOURCES
	Gameplay/GameInit.cpp
	Gameplay/GameInit.h
	Gameplay/GameLoop.cpp
	Gameplay/GameLoop.h
	
	Gameplay/Worlds/BootWorld.cpp
	Gameplay/Worlds/BootWorld.h
	Gameplay/Worlds/TestWorld.cpp
	Gameplay/Worlds/TestWorld.h
	Gameplay/Worlds.h
)

set(MODEL_SOURCES
	Model/GameConfig.cpp
	Model/GameConfig.h
	Model/GameKernel.cpp
	Model/GameKernel.h
	Model/GameManager.cpp
	Model/GameManager.h
	Model/GameSettings.cpp
	Model/GameSettings.h
	Model/Inheritable.cpp
	Model/Inheritable.h
	
	Model/UI/UIContext.cpp
	Model/UI/UIContext.h
	Model/UI/UIElement.cpp
	Model/UI/UIElement.h
	Model/UI/UIGameStyle.cpp
	Model/UI/UIGameStyle.h
	Model/UI/UIManager.cpp
	Model/UI/UIManager.h
	Model/UI/UIObject.cpp
	Model/UI/UIObject.h
	Model/UI/UIStyle.h
	Model/UI/UIText.cpp
	Model/UI/UIText.h
	Model/UI/UITransform.cpp
	Model/UI/UITransform.h
	Model/UI/UIWidget.cpp
	Model/UI/UIWidget.h
	Model/UI/UIWidgetMatrix.cpp
	Model/UI/UIWidgetMatrix.h
	
	Model/World/Camera.cpp
	Model/World/Camera.h
	Model/World/Component.cpp
	Model/World/Component.h
	Model/World/ComponentTimingType.h
	Model/World/Entity.cpp
	Model/World/Entity.h
	Model/World/GameObject.cpp
	Model/World/GameObject.h
	Model/World/ILoadingHUD.h
	Model/World/LoadingHUD.cpp
	Model/World/LoadingHUD.h
	Model/World/TileMap.cpp
	Model/World/TileMap.h
	Model/World/TokenHandler.h
	Model/World/World.cpp
	Model/World/World.h
	Model/World/WorldClock.cpp
	Model/World/WorldClock.h
	Model/World/WorldStateMachine.cpp
	Model/World/WorldStateMachine.h
)

set(SOURCES
	${FRAMEWORK_SOURCES}
	${GAMEPLAY_SOURCES}
	${MODEL_SOURCES}
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
	Engine
)

################################################################################
# Libraries
################################################################################
target_link_libraries(${PROJECT_NAME} PUBLIC
	Engine
)
