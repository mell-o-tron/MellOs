#pragma once
#ifdef NDEBUG
#include "mellos/kernel/kernel.h"
#endif
#define __MELLOS_KERNEL_ASSERT__
// Formatted output
void __kassert_fail(const char* expr, const char* file, unsigned int line); // NOLINT(*-reserved-identifier)
void __kassert_fail_msg(const char* expr, const char* msg, const char* file, unsigned int line); // NOLINT(*-reserved-identifier)
#ifdef NDEBUG
#define kassert(x) kpanic("Asserts not configured");
#define kassert_msg(x, msg) kpanic("Asserts not configured");
#else
#define kassert(x) if (!(x)) __kassert_fail(#x, __FILE__, __LINE__);
#define kassert_msg(x, msg) if (!(x)) __kassert_fail_msg(#x, msg, __FILE__, __LINE__);
#endif
