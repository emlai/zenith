#include "assert.h"
#include <stdexcept>
#include <string>
#include <sstream>

void assertionError(const char* condition, const char* file, int line)
{
    std::ostringstream s;
    s << "Assertion failed: " << condition << " (" << file << ":" << std::to_string(line) << ")";
    throw std::logic_error(s.str());
}
