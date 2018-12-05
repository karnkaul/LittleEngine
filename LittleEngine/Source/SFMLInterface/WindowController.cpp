#include "le_stdafx.h"
#include <string>
#include "Engine/Logger/Logger.h"
#include "WindowController.h"
#include "Utils.h"
#include "Misc/Stopwatch.h"

namespace LittleEngine {
	LayerInfo::LayerInfo(int layerID) {
		SetLayerID(layerID);
	}

	LayerInfo::LayerInfo(const LayerID& layerID) {
		SetLayerID(static_cast<int>(layerID));
	}

	int LayerInfo::GetLayerID() const {
		return layerID;
	}

	int LayerInfo::SetLayerID(int layerID) {
		this->layerID = Maths::Clamp(layerID, 0, WindowController::MAX_LAYERID);
		return this->layerID;
	}

	int LayerInfo::SetLayerID(const LayerID& layerID) {
		return SetLayerID(static_cast<int>(layerID));
	}

	void WindowController::Buffer::Push(Drawable&& drawable, int index) {
		std::vector<Drawable>& vec = buffer[index];
		vec.push_back(drawable);
	}

	void WindowController::Buffer::ForEach(std::function<void(std::vector<Drawable>&)> Callback) {
		for (int i = 0; i <= MAX_LAYERID; ++i) {
			std::vector<Drawable>& vec = buffer[i];
			if (!vec.empty()) {
				Callback(vec);
			}
		}
	}

	void WindowController::Buffer::Clear() {
		for (int i = 0; i < MAX_LAYERID; ++i) {
			buffer[i].clear();
		}
	}

	WindowController::WindowController(int screenWidth, int screenHeight, const std::string& windowTitle) {
		m_uWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(screenWidth, screenHeight), windowTitle);
		m_bFocus = true;
	}

	WindowController::~WindowController() {
		Logger::Log("WindowController and RenderWindow destroyed");
	}

	bool WindowController::IsWindowOpen() const {
		return m_uWindow->isOpen();
	}

	bool WindowController::IsWindowFocussed() const {
		return m_bFocus;
	}

	void WindowController::PollEvents() {
		sf::Event event;
		m_input.ClearRawInput();
		while (m_uWindow->isOpen() && m_uWindow->pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				CloseWindow();
				break;

			case sf::Event::LostFocus:
				m_bFocus = false;
				break;

			case sf::Event::GainedFocus:
				m_bFocus = true;
				m_input.ResetKeyStates();
				break;

			case sf::Event::KeyPressed:
				m_input.OnRawSpecialInput(event.key.code);
				m_input.OnKeyDown(event.key);
				break;

			case sf::Event::KeyReleased:
				m_input.OnKeyUp(event.key);
				break;

			case sf::Event::TextEntered:
				m_input.OnRawInput(static_cast<unsigned int>(event.text.unicode));
				break;

			default:
				break;
			}
		}
	}

	void WindowController::Push(Drawable&& drawable) {
		int index = drawable.layer.GetLayerID();
		index = Maths::Clamp(index, 0, MAX_LAYERID);
		m_buffer.Push(std::move(drawable), index);
	}

	void WindowController::Draw() {
		STOPWATCH_START("window->clear()");
		m_uWindow->clear();
		STOPWATCH_STOP();
		STOPWATCH_START("window->draw()");
		m_buffer.ForEach(
			[&w = this->m_uWindow](std::vector<Drawable>& drawables) {
				for (Drawable& drawable : drawables) {
					w->draw(drawable.GetSFMLDrawable());
				}
			}
		);
		STOPWATCH_STOP();
		
		m_buffer.Clear();
		STOPWATCH_START("window->display()");
		m_uWindow->display();
		STOPWATCH_STOP();
	}

	void WindowController::CloseWindow() {
		m_uWindow->close();
	}

	const Input& WindowController::GetInput() const {
		return m_input;
	}
}
