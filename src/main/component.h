#pragma once

#include <boost/utility/string_ref.hpp>
#include <cassert>
#include <memory>

class Creature;
class Entity;
class Game;
class Item;
class SaveFile;

class Component
{
public:
    virtual ~Component() = 0;
    static std::unique_ptr<Component> get(boost::string_ref name, Entity& parent);
    Entity& getParent() const { return *parent; }

    /// Returns true if the component did react to the movement attempt.
    virtual bool reactToMovementAttempt() { return false; }
    virtual bool preventsMovement() { return false; }
    virtual bool close() { return false; }
    virtual bool blocksSight() const { return false; }
    virtual bool isUsable() const { return false; }
    virtual bool use(Creature&, Item&, Game&) { return false; }
    virtual void save(SaveFile& file) const = 0;
    virtual void load(const SaveFile& file) = 0;

private:
    Entity* parent;
};
