#pragma once

#include "../component.h"

class Door : public Component
{
    bool reactToMovementAttempt() override;
    bool preventsMovement() override { return !isOpen; }

private:
    bool isOpen = false;
};
