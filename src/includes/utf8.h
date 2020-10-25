#pragma once

#include <stddef.h>
#include "common.h"
#include "unicode_string.h"

int utf8_decode(const char *bytes, UnicodeString_t *output);
int utf8_encode(UnicodeString_t *string, char **output);

codepoint_t utf8_decode_codepoint(const char **pointer);