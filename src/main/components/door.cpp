#include "door.h"
#include "../object.h"

bool Door::reactToMovementAttempt()
{
    if (!isOpen)
    {
        isOpen = true;
        getParent().getSprite().setFrame(1);
        return true;
    }

    return false;
}
