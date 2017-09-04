#include "creaturecontroller.h"
#include "action.h"
#include "creature.h"
#include "game.h"
#include "engine/menu.h"
#include "engine/window.h"

CreatureController::~CreatureController() {}

Action AIController::control(Creature& creature)
{
    if (creature.isDead())
        return Wait;

    if (auto* nearestEnemy = creature.getNearestEnemy())
        return creature.tryToMoveTowardsOrAttack(*nearestEnemy);
    else
        return creature.tryToMoveOrAttack(randomDir8());
}

Action PlayerController::control(Creature& creature)
{
    game.advanceTurn();

    while (true)
    {
        game.render(game.getWindow());
        game.getWindow().updateScreen();

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
                game.stop();
                return NoAction;

#ifdef DEBUG
            case Tab:
                game.enterCommandMode(game.getWindow());
                return NoAction;
#endif

            case NoKey:
                return NoAction;
        }
    }
}
