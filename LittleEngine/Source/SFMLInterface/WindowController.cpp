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

	int LayerInfo::GetLayerID() {
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

	void WindowController::Buffer::ForEach(std::function<void(std::vector<Drawable>)> Callback) const {
		for (int i = 0; i <= MAX_LAYERID; ++i) {
			std::vector<Drawable> vec = buffer[i];
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
		window = std::make_unique<sf::RenderWindow>(sf::VideoMode(screenWidth, screenHeight), windowTitle);
		_bFocus = true;
	}

	WindowController::~WindowController() {
		Logger::Log("WindowController and RenderWindow destroyed");
	}

	bool WindowController::IsWindowOpen() const {
		return window->isOpen();
	}

	bool WindowController::IsWindowFocussed() const {
		return _bFocus;
	}

	void WindowController::PollInput() {
		sf::Event event;
		input.ClearRawInput();
		while (window->isOpen() && window->pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				CloseWindow();
				break;

			case sf::Event::LostFocus:
				_bFocus = false;
				break;

			case sf::Event::GainedFocus:
				_bFocus = true;
				input.ResetKeyStates();
				break;

			case sf::Event::KeyPressed:
				input.OnRawSpecialInput(event.key.code);
				input.OnKeyDown(event.key);
				break;

			case sf::Event::KeyReleased:
				input.OnKeyUp(event.key);
				break;

			case sf::Event::TextEntered:
				input.OnRawInput(static_cast<int>(event.text.unicode));
				break;

			default:
				break;
			}
		}
	}

	void WindowController::Push(Drawable&& drawable) {
		int index = drawable.layer.GetLayerID();
		index = Maths::Clamp(index, 0, MAX_LAYERID);
		buffer.Push(std::move(drawable), index);
	}

	void WindowController::Draw() {
		STOPWATCH_START("window->clear()");
		window->clear();
		STOPWATCH_STOP();
		STOPWATCH_START("ForEach().draw()");
		buffer.ForEach(
			[&w = this->window](std::vector<Drawable> drawables) {
				for (Drawable& drawable : drawables) {
					w->draw(drawable.GetSFMLDrawable());
				}
			}
		);
		STOPWATCH_STOP();
		
		buffer.Clear();
		STOPWATCH_START("window->display()");
		window->display();
		STOPWATCH_STOP();
	}

	void WindowController::CloseWindow() {
		window->close();
	}

	const Input& WindowController::GetInput() const {
		return input;
	}
}
