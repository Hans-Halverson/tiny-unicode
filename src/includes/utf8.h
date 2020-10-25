#pragma once

#include <stddef.h>
#include "common.h"
#include "unicode_string.h"

int utf8_decode(String_t string, UnicodeString_t *output);
int utf8_encode(UnicodeString_t string, String_t *output);
