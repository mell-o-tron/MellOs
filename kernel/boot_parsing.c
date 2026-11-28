
#include "kernel_stdio.h"
#include "mellos/kernel/boot_params.h"
#include "stdbool.h"
#include "stddef.h"
#include "string.h"
#include "stdlib.h"

char value[BOOT_PARAMS_LENGTH];

char* cmdline_get(const char* key, char* fallback) {
	const size_t key_len = kstrlen(key);

	char* p = boot_cmdline;
	int key_start_offset = 0;
	char* value_start = NULL;

	do {
		if (value_start != NULL) {
			int i = 0;
			for (i = 0; value_start[i] != ' ' && value_start[i] != '\n' && value_start[i] != '\0'; i++) {
				value[i] = value_start[i];
			}
			value[i] = '\0';
			return value;
		}

		if (*p == '=') {
			if (kmemcmp(boot_cmdline + key_start_offset, key, key_len) == 0) {
				value_start = p + 1;
			}
		} else if (*p == ' ') {
			key_start_offset = p - boot_cmdline;
		}
		p++;
	} while(*p != '\0');

	return fallback;
}

size_t boot_params_storage_iterator = 0;

int cache_boot_params() {
	uint32_t i = 0;
	boot_parameter_t* p;
	size_t str_length;

	do {
		p = &(boot_params[i]);

		const char* retval = cmdline_get(p->keyname, p->default_value);
		switch (p->format) {
			case PARAM_TYPE_STRING:
			str_length = kstrlen(retval);
			kmemmove(&(boot_params_storage[boot_params_storage_iterator]), retval, str_length);
			p->value = &(boot_params_storage[boot_params_storage_iterator]);
			boot_params_storage_iterator += str_length;
		break;
			case PARAM_TYPE_BOOL:
			if (kstrcmp(retval, "true") == 0) {
				boot_params_storage[boot_params_storage_iterator] = true;
				p->value = &(boot_params_storage[boot_params_storage_iterator]);
				boot_params_storage_iterator++;
			} else if (kstrcmp(retval, "false") == 0) {
				boot_params_storage[boot_params_storage_iterator] = false;
				p->value = &(boot_params_storage[boot_params_storage_iterator]);
				boot_params_storage_iterator++;
			} else {
				kprintf("Invalid boolean for value %s", retval);
			}
		break;
			case PARAM_TYPE_INT32:
			boot_params_storage[boot_params_storage_iterator] = atoi((char*)retval);
			p->value = &(boot_params_storage[boot_params_storage_iterator]);
			boot_params_storage_iterator += sizeof(int32_t);
		break;
		}


		i++;
	} while(p->last == false);

	return 0;
}

void* param_get_address(const char* key) {
	boot_parameter_t* p;
	int i = 0;
	do {
		p = &(boot_params[i]);
		if (kstrcmp(p->keyname, key) == 0) {
			if (p->value == NULL) {
				return p->default_value;
			} else {
				return p->value;
			}
		}
		i++;
	} while (!p->last);
	return NULL;
}