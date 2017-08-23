#pragma once

#include "../component.h"

class Door : public Component
{
    void reactToMovementAttempt() override;

private:
    bool isOpen = false;
};
