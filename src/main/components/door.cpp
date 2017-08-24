#include "door.h"
#include "../object.h"

bool Door::reactToMovementAttempt()
{
    if (!isOpen)
    {
        isOpen = true;
        // TODO: Add getSprite() to Entity to avoid casting here.
        dynamic_cast<Object&>(getParent()).getSprite().setFrame(1);
        return true;
    }

    return false;
}

bool Door::close()
{
    if (isOpen)
    {
        isOpen = false;
        // TODO: Add getSprite() to Entity to avoid casting here.
        dynamic_cast<Object&>(getParent()).getSprite().setFrame(0);
        return true;
    }

    return false;
}
