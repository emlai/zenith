enum Action : int;

class Controller
{
    virtual ~Controller() = 0;
    virtual Action control(Creature creature) = 0;
}

class AIController : public Controller
{
    AIController(std::unique_ptr<AI> ai) : ai(std::move(ai)) {}
    Action control(Creature creature) override;
    static std::unique_ptr<AIController> get(string id, Creature creature);

private:
    std::unique_ptr<AI> ai;
}

class PlayerController : public Controller
{
    PlayerController(Game game) : game(game) {}

private:
    Action control(Creature creature) override;

    Game game;
}

Action getMappedAction(Key key);
Key getMappedKey(Action action);
void mapKey(Key key, Action action);
// If 'config' is null, loads the default key map.
void loadKeyMap(Config config);
void saveKeyMap(Config config);
Dir8 getDirectionFromEvent(Event event, Vector2 origin);
Controller::~Controller() {}

std::unique_ptr<AIController> AIController::get(string id, Creature creature)
{
    var ai = AI::get(Game::creatureConfig.get<string>(id, "ai"), creature);
    return std::make_unique<AIController>(std::move(ai));
}

Action AIController::control(Creature creature)
{
    if (creature.isDead())
        return Wait;

    var action = ai.control();
    assert(action != NoAction);
    return action;
}

Action PlayerController::control(Creature creature)
{
    game.advanceTurn();

    while (true)
    {
        Event event = game.getWindow().waitForInput();

        if (!creature.isDead())
            if (var direction = getDirectionFromEvent(event, creature.getPosition()))
                if (var action = creature.tryToMoveOrAttack(direction))
                    return action;

        if (event.type != Event::KeyDown)
            continue;

        var action = getMappedAction(event.key);

        switch (action)
        {
            case GoUpOrDown:
                if (!creature.isDead() && creature.enter())
                    return GoUpOrDown;
                break;

            case Wait:
                return Wait;

            case PickUpItems:
                if (!creature.isDead() && creature.pickUpItem())
                    return PickUpItems;
                break;

            case EnterLookMode:
                game.lookMode();
                break;

            case OpenInventory:
                game.showInventory("Inventory", false);
                break;

            case ShowEquipmentMenu:
                if (!creature.isDead())
                    game.showEquipmentMenu();
                break;

            case UseItem:
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to use?", false,
                                                           nullptr, [](var item)
                {
                    return item.isUsable();
                });

                if (selectedItemIndex != Menu::Exit)
                    if (creature.use(*creature.getInventory()[selectedItemIndex], game))
                        return UseItem;
                break;
            }

            case EatItem:
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to eat?", false,
                                                           nullptr, [](var item)
                {
                    return item.isEdible();
                });

                if (selectedItemIndex != Menu::Exit)
                    if (creature.eat(*creature.getInventory()[selectedItemIndex]))
                        return EatItem;
                break;
            }

            case DropItem:
            {
                if (creature.isDead())
                    break;

                int selectedItemIndex = game.showInventory("What do you want to drop?", false);

                if (selectedItemIndex != Menu::Exit)
                {
                    creature.drop(*creature.getInventory()[selectedItemIndex]);
                    return DropItem;
                }

                break;
            }

            case Close:
            {
                if (creature.isDead())
                    break;

                boost::optional<Dir8> direction = game.askForDirection("What do you want to close?");

                if (direction && creature.close(*direction))
                    return Close;
                break;
            }

            case ToggleRunning:
                creature.setRunning(!creature.isRunning());
                break;

            default:
                switch (event.key)
                {
                    case Esc:
                    case 'q':
                    case NoKey:
                        game.stop();
                        return NoAction;

#ifdef DEBUG
                    case F2:
                        game.playerSeesEverything = !game.playerSeesEverything;
                        break;

                    case F3:
                    {
                        var itemName = game.askForString("Which item do you want to spawn?");

                        if (itemName.empty())
                            break;

                        var materialName = game.askForString("Which material do you want to use for the item?");
                        creature.getTileUnder(0).addItem(std::make_unique<Item>(itemName, materialName));
                        break;
                    }

                    case Game::commandModeKey:
                        game.enterCommandMode(game.getWindow());
                        return NoAction;
#endif
                }
                break;
        }
    }
}

const int keyMapSize = 128;
static Action keyMap[keyMapSize];

Action getMappedAction(Key key)
{
    if (key >= 0 && key < keyMapSize)
        return keyMap[key];

    return NoAction;
}

Key getMappedKey(Action action)
{
    for (int i = 0; i < keyMapSize; ++i)
    {
        if (keyMap[i] == action)
            return i;
    }

    return NoKey;
}

void mapKey(Key key, Action action)
{
    if (key >= 0 && key < keyMapSize)
    {
        var oldKey = getMappedKey(action);
        keyMap[oldKey] = NoAction;
        keyMap[key] = action;
    }
}

static Key getDefaultKeyForAction(Action action)
{
    switch (action)
    {
        case Wait: return '.';
        case GoUpOrDown: return Enter;
        case PickUpItems: return ',';
        case DropItem: return 'd';
        case UseItem: return 'u';
        case EatItem: return 'e';
        case Close: return 'c';
        case EnterLookMode: return 'l';
        case OpenInventory: return 'i';
        case ShowEquipmentMenu: return 'E';
        case ToggleRunning: return 'r';
        default: return NoKey;
    }
}

void loadKeyMap(Config config)
{
    for (int i = NoAction + 1; i < LastAction; ++i)
    {
        var action = static_cast<Action>(i);
        Key key = config ? config.getOptional<int>(toString(action)).get_value_or(NoKey) : NoKey;

        if (!key)
            key = getDefaultKeyForAction(action);

        mapKey(key, action);
    }
}

void saveKeyMap(Config config)
{
    for (int i = NoAction + 1; i < LastAction; ++i)
    {
        var action = static_cast<Action>(i);
        config.set(toString(action), static_cast<long long>(getMappedKey(action)));
    }
}

Dir8 getDirectionFromEvent(Event event, Vector2 origin)
{
    switch (event.type)
    {
        case Event::MouseButtonDown:
            if (Game::cursorPosition)
                if (var direction = (*Game::cursorPosition - origin).getDir8())
                    return direction;

            break;

        case Event::KeyDown:
            switch (event.key)
            {
                case RightArrow:
                    return East;

                case LeftArrow:
                    return West;

                case DownArrow:
                    return South;

                case UpArrow:
                    return North;
            }
            break;
    }

    return Dir8::NoDir;
}
