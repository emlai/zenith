#include "entity.h"

Entity::Entity(boost::string_ref id)
:   id(id)
{
}

std::string Entity::getName() const
{
    std::string name;
    name.reserve(id.size());

    for (char ch : id)
    {
        if (std::isupper(ch))
        {
            if (!name.empty())
                name += ' ';

            name += std::tolower(ch);
        }
        else
            name += ch;
    }

    return name;
}
