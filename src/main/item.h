#pragma once

#include "entity.h"
#include "engine/geometry.h"
#include "engine/sprite.h"
#include <boost/utility/string_ref.hpp>
#include <memory>

class Creature;
class SaveFile;
class Window;
enum EquipmentSlot : int;

class Item : public Entity
{
public:
    Item(boost::string_ref id, boost::string_ref materialId);
    static std::unique_ptr<Item> load(const SaveFile& file);
    virtual void save(SaveFile& file) const;
    virtual void exist() {}
    bool isUsable() const;
    bool use(Creature& user, Game& game);
    bool isEdible() const;
    EquipmentSlot getEquipmentSlot() const;
    virtual std::string getNameAdjective() const override;
    void render(Window& window, Vector2 position) const;
    virtual void renderEquipped(Window& window, Vector2 position) const;
    const Sprite& getSprite() const { return sprite; }

protected:
    Item(boost::string_ref id, boost::string_ref materialId, Sprite sprite);

    std::string materialId;
    Sprite sprite;
};

std::string getRandomMaterialId(boost::string_ref itemId);

class Corpse final : public Item
{
public:
    Corpse(std::unique_ptr<Creature> creature);
    Corpse(boost::string_ref creatureId);
    void exist() override;
    void renderEquipped(Window& window, Vector2 position) const override;
    void save(SaveFile& file) const override;

private:
    static const int corpseFrame = 2;
    static const char corpseGlyph = ',';

    std::unique_ptr<Creature> creature;
};
