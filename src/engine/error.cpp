#include "error.h"
#include "assert.h"
#include <cstdio>

void warn(std::string_view message)
{
    fprintf(stderr, "%.*s\n", int(message.size()), message.data());
    ASSERT(false);
}
