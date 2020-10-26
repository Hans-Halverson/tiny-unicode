#pragma once

#include <stddef.h>
#include "common.h"
#include "unicode_string.h"

int utf32_decode(String_t string, UnicodeString_t *output);
int utf32_encode(UnicodeString_t string, String_t *output);
