#include "creaturecontroller.h"
#include "creature.h"
#include "game.h"
#include "engine/menu.h"
#include "engine/window.h"

CreatureController::~CreatureController() {}

void AIController::control(Creature& creature)
{
    if (creature.isDead())
        return;

    if (auto* nearestEnemy = creature.getNearestEnemy())
        creature.tryToMoveTowardsOrAttack(*nearestEnemy);
    else
        creature.tryToMoveOrAttack(randomDir8());
}

void PlayerController::control(Creature& creature)
{
    game.advanceTurn();

    while (true)
    {
        game.render(game.getWindow());
        game.getWindow().updateScreen();

        switch (game.getWindow().waitForInput())
        {
            case RightArrow:
                if (!creature.isDead() && creature.tryToMoveOrAttack(East))
                    return;
                break;

            case LeftArrow:
                if (!creature.isDead() && creature.tryToMoveOrAttack(West))
                    return;
                break;

            case DownArrow:
                if (!creature.isDead() && creature.tryToMoveOrAttack(South))
                    return;
                break;

            case UpArrow:
                if (!creature.isDead() && creature.tryToMoveOrAttack(North))
                    return;
                break;

            case Enter:
                if (!creature.isDead() && creature.enter())
                    return;
                break;

            case '.':
                return;

            case ',':
                if (!creature.isDead() && creature.pickUpItem())
                    return;
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
                        return;
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
                    return;
                }
        
                break;
            }

            case 'c':
            {
                if (creature.isDead())
                    break;

                boost::optional<Dir8> direction = game.askForDirection("What do you want to close?");

                if (direction && creature.close(*direction))
                    return;
                break;
            }

            case Esc:
            case 'q':
                game.stop();
                return;

#ifdef DEBUG
            case Tab:
                game.enterCommandMode(game.getWindow());
                return;
#endif

            case NoKey:
                return;
        }
    }
}
