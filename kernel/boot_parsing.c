
#include "mellos/kernel/kernel.h"
#include "mem.h"
#include "string.h"

char value[128];

const char* cmdline_get(const char* key, const char* fallback) {
	const size_t key_len = strlen(key);

	const char* p = boot_cmdline;
	int key_start_offset = 0;
	const char* value_start = NULL;

	while (p++) {
		if (value_start != NULL) {
			int i = 0;
			for (i = 0; value_start[i] != ' ' && value_start[i] != '\n' && value_start[i] != '\0'; i++) {
				value[i] = value_start[i];
			}
			value[i] = '\0';
			return value;
		}

		if (*p == '=') {
			if (memcmp(boot_cmdline + key_start_offset, key, key_len) == 0) {
				value_start = p + 1;
			}
		} else if (*p == ' ') {
			key_start_offset = p - boot_cmdline;
		}
	}

	return fallback;
}
