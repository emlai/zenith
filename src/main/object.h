#pragma once

#include "component.h"
#include "entity.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <vector>

class Object : public Entity
{
public:
    Object(boost::string_ref id);
    void render(Window& window, Vector2 position) const;
    /// Returns true if the object did react to the movement attempt.
    bool reactToMovementAttempt();
    bool preventsMovement();
    Sprite& getSprite() { return sprite; }

private:
    Sprite sprite;
    std::vector<std::unique_ptr<Component>> components;
};
