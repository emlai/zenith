#include "door.h"
#include "../object.h"
#include "engine/savefile.h"

bool Door::reactToMovementAttempt()
{
    if (!isOpen)
    {
        setOpen(true);
        return true;
    }

    return false;
}

bool Door::close()
{
    if (isOpen)
    {
        setOpen(false);
        return true;
    }

    return false;
}

void Door::setOpen(bool open)
{
    isOpen = open;

    // TODO: Add getSprite() to Entity to avoid casting here.
    auto& sprite = dynamic_cast<Object&>(getParent()).getSprite();
    sprite.setFrame(isOpen ? 1 : 0);
}

void Door::save(SaveFile& file) const
{
    file.write(isOpen);
}

void Door::load(const SaveFile& file)
{
    setOpen(file.readBool());
}
