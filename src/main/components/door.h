#pragma once

#include "../component.h"

class Door : public Component
{
    void reactToMovementAttempt() override;
    bool preventsMovement() override { return !isOpen; }

private:
    bool isOpen = false;
};
