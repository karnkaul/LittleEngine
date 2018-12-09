#include "le_stdafx.h"
#include <string>
#include "Engine/Logger/Logger.h"
#include "Graphics.h"
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
		this->layerID = Maths::Clamp(layerID, 0, Graphics::MAX_LAYERID);
		return this->layerID;
	}

	int LayerInfo::SetLayerID(const LayerID& layerID) {
		return SetLayerID(static_cast<int>(layerID));
	}

	void Graphics::Buffer::Push(Drawable&& drawable, int index) {
		std::vector<Drawable>& vec = buffer[index];
		vec.push_back(drawable);
	}

	void Graphics::Buffer::ForEach(std::function<void(std::vector<Drawable>&)> Callback) {
		for (int i = 0; i <= MAX_LAYERID; ++i) {
			std::vector<Drawable>& vec = buffer[i];
			if (!vec.empty()) {
				Callback(vec);
			}
		}
	}

	void Graphics::Buffer::Clear() {
		for (int i = 0; i < MAX_LAYERID; ++i) {
			buffer[i].clear();
		}
	}

	std::unique_ptr<Graphics> Graphics::m_instance;

	Graphics & Graphics::CreateInstance(const GraphicsData& data) {
		m_instance = std::make_unique<Graphics>(data);
		return *m_instance;
	}

	bool Graphics::DestroyInstance() {
		if (m_instance) {
			m_instance = nullptr;
			return true;
		}
		return false;
	}

	sf::Vector2f Graphics::Cast(const Vector2& vector) {
		return sf::Vector2f(vector.x.ToFloat(), vector.y.ToFloat());
	}

	Vector2 Graphics::Cast(const sf::Vector2f & vector) {
		return Vector2(Fixed(vector.x), Fixed(vector.y));
	}

	sf::Color Graphics::Cast(const Colour& colour) {
		return sf::Color(
			colour.r.ToUInt(),
			colour.g.ToUInt(),
			colour.b.ToUInt(),
			colour.a.ToUInt()
		);
	}

	Colour Graphics::Cast(const sf::Color& colour) {
		return Colour(colour.r, colour.g, colour.b, colour.a);
	}

	Vector2 Graphics::GetGameViewSize() {
		return m_instance ? m_instance->m_gameViewSize : Vector2::Zero;
	}

	Rect2 Graphics::GetWorldRect() {
		return m_instance ? m_instance->m_worldBounds : Rect2();
	}

	Vector2 Graphics::NToWorld(const Vector2 & normalised, bool autoClamp) {
		Vector2 p = normalised;
		if (autoClamp) {
			p.x = Maths::Clamp_11(p.x);
			p.y = Maths::Clamp_11(p.y);
		}
		const Vector2& s = m_instance ? m_instance->m_worldBounds.upper : Vector2::Zero;
		return Vector2(p.x * s.x, p.y * s.y);
	}

	void Graphics::Submit(Drawable && drawable) {
		if (m_instance) m_instance->Push(std::move(drawable));
	}

	Graphics::Graphics(const GraphicsData& data) {
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		std::string screenSizeStr = "[" + Strings::ToString(data.screenWidth) + "x" + Strings::ToString(data.screenHeight) + "]";
		std::string desktopSizeStr = "[" + Strings::ToString(desktop.width) + "x" + Strings::ToString(desktop.height) + "]";
		if (data.screenHeight > desktop.height || data.screenWidth > desktop.width) throw GraphicsException("Invalid screen size " + screenSizeStr + " Desktop size: " + desktopSizeStr);

		Fixed worldAspect = data.viewSize.x / data.viewSize.y;
		Fixed screenAspect = Fixed(static_cast<const int>(data.screenWidth)) / Fixed(static_cast<const int>(data.screenHeight));
		if (Maths::Abs(worldAspect - screenAspect) > Fixed(1, 100)) {
			Logger::Log("World view's aspect ratio " + data.viewSize.ToString() + " is significantly different from screen's aspect ratio " 
						+ screenSizeStr + "\nExpect display to be stretched!", Logger::Severity::Warning);
		}

		m_uWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(data.screenWidth, data.screenHeight), data.windowTitle);
		m_bFocus = true;

		// TODO: handle non-identical aspect ratios
		m_gameViewSize = data.viewSize;
		m_worldBounds = Rect2(-Fixed::OneHalf * m_gameViewSize, Fixed::OneHalf * m_gameViewSize);
		gameView.setSize(Cast(data.viewSize));
		gameView.setCenter(Cast(Vector2::Zero));
		m_uWindow->setView(gameView);
	}

	Graphics::~Graphics() {
		Logger::Log("Graphics and RenderWindow destroyed");
	}

	bool Graphics::IsWindowOpen() const {
		return m_uWindow->isOpen();
	}

	bool Graphics::IsWindowFocussed() const {
		return m_bFocus;
	}

	void Graphics::PollEvents() {
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

	void Graphics::Push(Drawable&& drawable) {
		int index = drawable.layer.GetLayerID();
		index = Maths::Clamp(index, 0, MAX_LAYERID);
		m_buffer.Push(std::move(drawable), index);
	}

	void Graphics::Draw() {
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

	void Graphics::CloseWindow() {
		m_uWindow->close();
	}

	const Input& Graphics::GetInput() const {
		return m_input;
	}
}
