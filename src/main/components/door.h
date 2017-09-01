#pragma once

#include "../component.h"

class Door : public Component
{
    bool reactToMovementAttempt() override;
    bool preventsMovement() override { return !isOpen; }
    bool close() override;
    bool blocksSight() const override { return !isOpen; }
    void save(SaveFile& file) const override;
    void load(const SaveFile& file) override;

private:
    void setOpen(bool open);

    bool isOpen = false;
};
