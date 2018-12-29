#include "error.h"
#include <cassert>
#include <cstdio>

void warn(std::string_view message)
{
    fprintf(stderr, "%.*s\n", int(message.size()), message.data());
    assert(false);
}
