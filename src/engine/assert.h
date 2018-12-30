#pragma once

[[noreturn]]
void assertionError(const char* condition, const char* file, int line);

#ifdef DEBUG
// TODO: Allow passing message as second parameter.
#define ASSERT(condition) ((condition) || (assertionError(#condition, __FILE__, __LINE__), 0))
#else
#define ASSERT(condition) ((void) 0)
#endif
