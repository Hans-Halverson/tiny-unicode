#pragma once

#include <stddef.h>
#include "common.h"
#include "unicode_string.h"

int utf16_decode(const char *bytes, UnicodeString_t *output);
int utf16_encode(UnicodeString_t *string, char **output);
