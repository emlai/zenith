#include "controller.h"
#include "action.h"
#include "creature.h"
#include "game.h"
#include "engine/assert.h"
#include "engine/config.h"
#include "engine/menu.h"
#include "engine/window.h"

Controller::~Controller() {}

std::unique_ptr<AIController> AIController::get(std::string_view id, Creature& creature)
{
    auto ai = AI::get(Game::creatureConfig->get<std::string>(id, "ai"), creature);
    return std::make_unique<AIController>(std::move(ai));
}

Action AIController::control(Creature& creature)
{
    if (creature.isDead())
        return Wait;

    auto action = ai->control();
    ASSERT(action != NoAction);
    return action;
}

Action PlayerController::control(Creature& creature)
{
    // TODO: Find a better place for this.
    game.gameState->turn++;

    while (true)
    {
        Event event = game.getWindow().waitForInput();

        if (event.type != Event::KeyDown)
            continue;

        auto action = getMappedAction(event.key);

        switch (action)
        {
            case MoveUp:
            case MoveUpLeft:
            case MoveLeft:
            case MoveDownLeft:
            case MoveDown:
            case MoveDownRight:
            case MoveRight:
            case MoveUpRight:
            {
                if (creature.isDead())
                    break;

                auto direction = getMovementDirection(action);

                if (auto action = creature.tryToMoveOrAttack(direction))
                    return action;

                break;
            }

            case GoUpOrDown:
                if (!creature.isDead() && creature.enter())
                    return GoUpOrDown;
                break;

            case Wait:
                return Wait;

            case PickUpItems:
                if (!creature.isDead() && creature.pickUpItem())
                    return PickUpItems;
                break;

            case EnterLookMode:
                game.lookMode();
                break;

            case OpenInventory:
                game.showInventory("Inventory", false);
                break;

            case ShowEquipmentMenu:
                if (!creature.isDead())
                    game.showEquipmentMenu();
                break;

            case UseItem:
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to use?", false, [](auto& item)
                {
                    return item.isUsable();
                });

                if (selectedItemIndex != Menu::Exit)
                    if (creature.use(*creature.getInventory()[selectedItemIndex], game))
                        return UseItem;
                break;
            }

            case EatItem:
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to eat?", false, [](auto& item)
                {
                    return item.isEdible();
                });

                if (selectedItemIndex != Menu::Exit)
                    if (creature.eat(*creature.getInventory()[selectedItemIndex]))
                        return EatItem;
                break;
            }

            case DropItem:
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to drop?", false);

                if (selectedItemIndex != Menu::Exit)
                {
                    creature.drop(*creature.getInventory()[selectedItemIndex]);
                    return DropItem;
                }

                break;
            }

            case Close:
            {
                if (creature.isDead())
                    break;

                std::optional<Dir8> direction = game.askForDirection("What do you want to close?");

                if (direction && creature.close(*direction))
                    return Close;
                break;
            }

            case ToggleRunning:
                creature.setRunning(!creature.isRunning());
                break;

            case Attack:
            case LastAction:
                ASSERT(false);
                break;

            case NoAction:
                switch (event.key)
                {
                    case Esc:
                    case 'q':
                    case NoKey:
                        game.stop();
                        return NoAction;

#ifdef DEBUG
                    case F2:
                        game.playerSeesEverything = !game.playerSeesEverything;
                        break;

                    case F3:
                    {
                        auto itemName = game.askForString("Which item do you want to spawn?");

                        if (itemName.empty())
                            break;

                        auto materialName = game.askForString("Which material do you want to use for the item?");
                        creature.getTileUnder(0).addItem(std::make_unique<Item>(itemName, materialName));
                        break;
                    }

                    case Game::commandModeKey:
                        game.enterCommandMode(game.getWindow());
                        return NoAction;
#endif
                }
                break;
        }
    }
}

static const int keyMapSize = 128;
static Action keyMap[keyMapSize];

Action getMappedAction(Key key)
{
    if (key >= 0 && key < keyMapSize)
        return keyMap[key];

    return NoAction;
}

Key getMappedKey(Action action)
{
    for (int i = 0; i < keyMapSize; ++i)
    {
        if (keyMap[i] == action)
            return i;
    }

    return NoKey;
}

void mapKey(Key key, Action action)
{
    if (key >= 0 && key < keyMapSize)
    {
        auto oldKey = getMappedKey(action);
        keyMap[oldKey] = NoAction;
        keyMap[key] = action;
    }
}

static Key getDefaultKeyForAction(Action action)
{
    switch (action)
    {
        case Wait: return '.';
        case GoUpOrDown: return Enter;
        case PickUpItems: return ',';
        case DropItem: return 'D';
        case UseItem: return 'u';
        case EatItem: return 'e';
        case Close: return 'c';
        case EnterLookMode: return 'l';
        case OpenInventory: return 'i';
        case ShowEquipmentMenu: return 'E';
        case ToggleRunning: return 'r';
        case MoveUp: return 'w';
        case MoveLeft: return 'a';
        case MoveDown: return 's';
        case MoveRight: return 'd';
        case MoveUpLeft:
        case MoveDownLeft:
        case MoveDownRight:
        case MoveUpRight:
        case Attack:
        case NoAction:
        case LastAction:
            return NoKey;
    }

    ASSERT(false);
}

void loadKeyMap(const Config* config)
{
    for (int i = NoAction + 1; i < LastAction; ++i)
    {
        auto action = static_cast<Action>(i);
        Key key = config ? config->getOptional<int>(toString(action)).value_or(NoKey) : NoKey;

        if (!key)
            key = getDefaultKeyForAction(action);

        mapKey(key, action);
    }
}

void saveKeyMap(Config& config)
{
    for (int i = NoAction + 1; i < LastAction; ++i)
    {
        auto action = static_cast<Action>(i);
        config.set(toString(action), static_cast<long long>(getMappedKey(action)));
    }
}

Dir8 getDirectionFromEvent(Event event, Vector2 origin)
{
    switch (event.type)
    {
        case Event::MouseButtonDown:
            if (Game::hoveredTile)
                if (auto direction = (Game::hoveredTile->getPosition() - origin).getDir8())
                    return direction;

            break;

        case Event::KeyDown:
            for (auto action : movementActions)
                if (event.key == getMappedKey(action))
                    return getMovementDirection(action);

            break;

        case Event::None:
            ASSERT(false);
            break;
    }

    return Dir8::NoDir;
}
