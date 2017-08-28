#pragma once

#include "entity.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>
#include <memory>

class Creature;
class Window;

class Item : public Entity
{
public:
    Item(boost::string_ref id, boost::string_ref materialId);
    bool isUsable() const;
    bool use(Creature& user, Game& game);
    virtual std::string getNamePrefix() const override;
    void render(Vector2 position) const;
    virtual void renderWielded(Vector2 position) const;
    const Sprite& getSprite() const { return sprite; }

protected:
    Item(boost::string_ref id, boost::string_ref materialId, Sprite&& sprite);

    std::string materialId;
    Sprite sprite;
};

class Corpse final : public Item
{
public:
    Corpse(std::unique_ptr<Creature> creature);
    Corpse(boost::string_ref creatureId);
    void renderWielded(Vector2 position) const override;

private:
    std::unique_ptr<Creature> creature;
};
