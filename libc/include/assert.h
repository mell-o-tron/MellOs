#pragma once

// Formatted output
void __assert_fail(const char* expr, const char* file, unsigned int line); // NOLINT(*-reserved-identifier)
void __assert_fail_msg(const char* expr, const char* msg, const char* file, unsigned int line); // NOLINT(*-reserved-identifier)
#ifdef NDEBUG
#define assert(x) asm("hlt");
#else
#define assert(x) if (!(x)) __assert_fail(#x, __FILE__, __LINE__);
#define assert_msg(x, msg) if (!(x)) __assert_fail_msg(#x, msg, __FILE__, __LINE__);
#endif