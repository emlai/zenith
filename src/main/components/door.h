#pragma once

#include "../component.h"

class Door : public Component
{
    bool reactToMovementAttempt() override;
    bool preventsMovement() override { return !isOpen; }
    bool close() override;

private:
    bool isOpen = false;
};
