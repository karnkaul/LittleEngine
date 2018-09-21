#include "stdafx.h"
#include <string>
#include "Engine/Logger/Logger.h"
#include "WindowController.h"
#include "Utils/Utils.h"

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
		_focus = true;
	}

	WindowController::~WindowController() {
		Logger::Log("WindowController and RenderWindow destroyed");
	}

	bool WindowController::IsWindowOpen() const {
		return window->isOpen();
	}

	bool WindowController::IsWindowFocussed() const {
		return _focus;
	}

	void HandleRawInput(RawTextInput& rawTextInput, int unicode) {
		if (unicode < 5 || unicode >= 128) {
			return;
		}

		switch (unicode) {
		case 8:
			rawTextInput.special = RawTextInputType::Backspace;
			return;

		case 9:
			rawTextInput.special = RawTextInputType::Tab;
			return;

		case 13:
			rawTextInput.special = RawTextInputType::Enter;
			return;

		case 27:
			rawTextInput.special = RawTextInputType::Escape;
			return;
		}

		rawTextInput.text += static_cast<char>(unicode);
	}

	void WindowController::PollInput() {
		sf::Event event;
		rawTextInput.text.clear();
		rawTextInput.special = RawTextInputType::None;
		while (window->isOpen() && window->pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				CloseWindow();
				break;

			case sf::Event::LostFocus:
				_focus = false;
				break;

			case sf::Event::GainedFocus:
				_focus = true;
				input.ResetKeyStates();
				break;

			case sf::Event::KeyPressed:
				input.OnKeyDown(event.key);
				break;

			case sf::Event::KeyReleased:
				input.OnKeyUp(event.key);
				break;

			case sf::Event::TextEntered:
				HandleRawInput(rawTextInput, static_cast<int>(event.text.unicode));
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
		window->clear();
		buffer.ForEach(
			[&w = this->window](std::vector<Drawable> drawables) {
				for (Drawable& drawable : drawables) {
					w->draw(drawable.GetSFMLDrawable());
				}
			}
		);
		buffer.Clear();
		window->display();
	}

	void WindowController::CloseWindow() {
		window->close();
	}

	const Input& WindowController::GetInput() const {
		return input;
	}

	const RawTextInput& WindowController::GetRawSFMLInput() const {
		return rawTextInput;
	}
}
