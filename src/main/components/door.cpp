#include "door.h"
#include "../object.h"

void Door::reactToMovementAttempt()
{
    if (!isOpen)
    {
        isOpen = true;
        getParent().getSprite().setFrame(1);
    }
}
