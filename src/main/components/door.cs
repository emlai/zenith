class Door : public Component
{
    bool reactToMovementAttempt() override;
    bool preventsMovement() override { return !isOpen; }
    bool close() override;
    bool blocksSight() override { return !isOpen; }
    void save(SaveFile file) override;
    void load(SaveFile file) override;

private:
    void setOpen(bool open);

    bool isOpen = false;
}
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
    var sprite = dynamic_cast<Object>(getParent()).getSprite();
    sprite.setFrame(isOpen ? 1 : 0);
    sprite.setAsciiGlyph(isOpen ? '_' : '#');
}

void Door::save(SaveFile file)
{
    file.write(isOpen);
}

void Door::load(SaveFile file)
{
    setOpen(file.readBool());
}
