#include "le_stdafx.h"
#include "RenderComponent.h"
#include "Transform.h"
#include "Entities/Actor.h"
#include "Levels/Level.h"
#include "Engine/World.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/Rendering/SpriteRenderer.h"
#include "SFMLInterface/Rendering/TextRenderer.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine {
	RenderComponent::RenderComponent(Actor& actor) : Component(actor, "RenderComponent") {
	}

	RenderComponent::RenderComponent(Actor& owner, const RenderComponent & prototype) : Component(owner, prototype) {
		renderer = prototype.renderer->UClone();
	}

	LayerInfo RenderComponent::GetLayer() const {
		return renderer->layer;
	}

	void RenderComponent::SetLayer(const LayerInfo& layer) {
		renderer->layer = layer;
	}

	void RenderComponent::SetRenderer(std::unique_ptr<Renderer> renderer) {
		this->renderer = std::move(renderer);
	}

	Rect2 RenderComponent::GetBounds() const {
		return renderer == nullptr ? Rect2(Vector2::Zero, Vector2::Zero) : renderer->GetBounds();
	}

	CircleRenderer & RenderComponent::SetCircleRenderer(const ShapeData & shapeData) {
		Fixed radius = shapeData.size.x;
		std::unique_ptr<CircleRenderer> renderer = std::make_unique<CircleRenderer>(radius, shapeData.colour);
		CircleRenderer* c = renderer.get();
		SetRenderer(std::move(renderer));
		return *c;
	}

	RectangleRenderer & RenderComponent::SetRectangleRenderer(const ShapeData & shapeData) {
		std::unique_ptr<RectangleRenderer> renderer = std::make_unique<RectangleRenderer>(shapeData.size, shapeData.colour);
		RectangleRenderer* r = renderer.get();
		SetRenderer(std::move(renderer));
		return *r;
	}
	
	SpriteRenderer& RenderComponent::SetSpriteRenderer(const std::string & texturePath) {
		TextureAsset* texture = GetActor().GetActiveLevel().GetAssetManager().Load<TextureAsset>(texturePath);
		SpriteData spriteData(*texture);
		SetRenderer(std::make_unique<SpriteRenderer>(spriteData));
		return *dynamic_cast<SpriteRenderer*>(renderer.get());
	}

	SpriteRenderer & RenderComponent::SetSpriteRenderer(TextureAsset & texture) {
		SpriteData spriteData(texture);
		SetRenderer(std::make_unique<SpriteRenderer>(spriteData));
		return dynamic_cast<SpriteRenderer&>(*renderer);
	}

	TextRenderer& RenderComponent::SetTextRenderer(const std::string & text) {
		FontAsset* font = GetActor().GetActiveLevel().GetAssetManager().GetDefaultFont();
		TextData textData(*font, text);
		SetRenderer(std::make_unique<TextRenderer>(textData));
		return *dynamic_cast<TextRenderer*>(renderer.get());
	}

	void RenderComponent::Render(RenderParams params) {
		renderer->Render(params);
	}
	std::shared_ptr<Component> RenderComponent::SClone(Actor& owner) const {
		return std::make_shared<RenderComponent>(owner, *this);
	}
}
