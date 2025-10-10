#pragma once

// Formatted output
void __assert_fail(const char* expr, const char* file, unsigned int line); // NOLINT(*-reserved-identifier)
#ifdef NDEBUG
#define assert(x) (void)0
#else
#define assert(x) if (!(x)) __assert_fail(#x, __FILE__, __LINE__);
#endif