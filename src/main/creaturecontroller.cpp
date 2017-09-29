#include "creaturecontroller.h"
#include "action.h"
#include "creature.h"
#include "game.h"
#include "engine/menu.h"
#include "engine/window.h"

CreatureController::~CreatureController() {}

std::unique_ptr<AIController> AIController::get(boost::string_ref id, Creature& creature)
{
    auto ai = AI::get(Game::creatureConfig->get<std::string>(id, "ai"), creature);
    return std::make_unique<AIController>(std::move(ai));
}

Action AIController::control(Creature& creature)
{
    if (creature.isDead())
        return Wait;

    auto action = ai->control();
    assert(action != NoAction);
    return action;
}

Action PlayerController::control(Creature& creature)
{
    game.advanceTurn();

    while (true)
    {
        switch (game.getWindow().waitForInput())
        {
            case RightArrow:
                if (!creature.isDead())
                    if (auto action = creature.tryToMoveOrAttack(East))
                        return action;
                break;

            case LeftArrow:
                if (!creature.isDead())
                    if (auto action = creature.tryToMoveOrAttack(West))
                        return action;
                break;

            case DownArrow:
                if (!creature.isDead())
                    if (auto action = creature.tryToMoveOrAttack(South))
                        return action;
                break;

            case UpArrow:
                if (!creature.isDead())
                    if (auto action = creature.tryToMoveOrAttack(North))
                        return action;
                break;

            case Enter:
                if (!creature.isDead() && creature.enter())
                    return Move;
                break;

            case '.':
                return Wait;

            case ',':
                if (!creature.isDead() && creature.pickUpItem())
                    return PickUpItems;
                break;

            case 'l':
                game.lookMode();
                break;

            case 'i':
                game.showInventory("Inventory", false);
                break;

            case 'e':
                if (!creature.isDead())
                    game.showEquipmentMenu();
                break;

            case 'u':
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to use?", false,
                                                           nullptr, [](auto& item)
                {
                    return item.isUsable();
                });
        
                if (selectedItemIndex != Menu::Exit)
                    if (creature.use(*creature.getInventory()[selectedItemIndex], game))
                        return UseItem;
                break;
            }

            case 'E':
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to eat?", false,
                                                           nullptr, [](auto& item)
                {
                    return item.isEdible();
                });
        
                if (selectedItemIndex != Menu::Exit)
                    if (creature.eat(*creature.getInventory()[selectedItemIndex]))
                        return EatItem;
                break;
            }

            case 'd':
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

            case 'c':
            {
                if (creature.isDead())
                    break;

                boost::optional<Dir8> direction = game.askForDirection("What do you want to close?");

                if (direction && creature.close(*direction))
                    return Close;
                break;
            }

            case 'r':
                creature.setRunning(!creature.isRunning());
                break;

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

            case Tab:
                game.enterCommandMode(game.getWindow());
                return NoAction;
#endif
        }
    }
}
