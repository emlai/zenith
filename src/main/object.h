#pragma once

#include "component.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <vector>

class Object
{
public:
    Object(boost::string_ref id);
    void render(Window& window, Vector2 position) const;
    void reactToMovementAttempt();
    bool preventsMovement();
    Sprite& getSprite() { return sprite; }

private:
    std::string id;
    Sprite sprite;
    std::vector<std::unique_ptr<Component>> components;
};
