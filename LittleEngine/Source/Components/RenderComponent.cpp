#include "le_stdafx.h"
#include "RenderComponent.h"
#include "Transform.h"
#include "Entities/Actor.h"
#include "Levels/Level.h"
#include "Engine/World.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Rendering/ShapeRenderable.h"
#include "SFMLInterface/Rendering/SpriteRenderable.h"
#include "SFMLInterface/Rendering/TextRenderable.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine {
	RenderComponent::RenderComponent(Actor& actor) : Component(actor, "RenderComponent") {
	}

	RenderComponent::RenderComponent(Actor& owner, const RenderComponent & prototype) : Component(owner, prototype) {
		uRenderable = prototype.uRenderable->UClone();
	}

	LayerInfo RenderComponent::GetLayer() const {
		return uRenderable->layer;
	}

	void RenderComponent::SetLayer(const LayerInfo& layer) {
		uRenderable->layer = layer;
	}

	void RenderComponent::SetRenderable(std::unique_ptr<Renderable> renderable) {
		this->uRenderable = std::move(renderable);
	}

	Rect2 RenderComponent::GetBounds() const {
		return uRenderable == nullptr ? Rect2(Vector2::Zero, Vector2::Zero) : uRenderable->GetBounds();
	}

	CircleRenderable & RenderComponent::SetCircleRenderable(const ShapeData & shapeData) {
		Fixed radius = shapeData.size.x;
		std::unique_ptr<CircleRenderable> renderable = std::make_unique<CircleRenderable>(radius, shapeData.colour);
		CircleRenderable* c = renderable.get();
		SetRenderable(std::move(renderable));
		return *c;
	}

	RectangleRenderable & RenderComponent::SetRectangleRenderable(const ShapeData & shapeData) {
		std::unique_ptr<RectangleRenderable> renderable = std::make_unique<RectangleRenderable>(shapeData.size, shapeData.colour);
		RectangleRenderable* r = renderable.get();
		SetRenderable(std::move(renderable));
		return *r;
	}
	
	SpriteRenderable& RenderComponent::SetSpriteRenderable(const std::string & texturePath) {
		TextureAsset* texture = GetActor().GetActiveLevel().GetAssetManager().Load<TextureAsset>(texturePath);
		SpriteData spriteData(*texture);
		SetRenderable(std::make_unique<SpriteRenderable>(spriteData));
		return *dynamic_cast<SpriteRenderable*>(uRenderable.get());
	}

	SpriteRenderable & RenderComponent::SetSpriteRenderable(TextureAsset & texture) {
		SpriteData spriteData(texture);
		SetRenderable(std::make_unique<SpriteRenderable>(spriteData));
		return dynamic_cast<SpriteRenderable&>(*uRenderable);
	}

	TextRenderable& RenderComponent::SetTextRenderable(const std::string & text) {
		FontAsset* font = GetActor().GetActiveLevel().GetAssetManager().GetDefaultFont();
		TextData textData(*font, text);
		SetRenderable(std::make_unique<TextRenderable>(textData));
		return *dynamic_cast<TextRenderable*>(uRenderable.get());
	}

	void RenderComponent::Render(RenderParams& params) {
		uRenderable->Render(params);
	}

	Component::Ptr RenderComponent::UClone(Actor& owner) const {
		return std::make_unique<RenderComponent>(owner, *this);
	}
}
