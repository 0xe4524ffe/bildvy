#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct _ConfEntry{
	uint8_t *key;
	uintptr_t value;
} ConfEntry;

