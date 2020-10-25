#pragma once

#include <stddef.h>
#include "common.h"
#include "unicode_string.h"

int utf16_decode(String_t string, UnicodeString_t *output);
int utf16_encode(UnicodeString_t string, String_t *output);
