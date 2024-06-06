#include "libmx.h"

void mx_print_unicode(wchar_t c) {
    if (c < 0x80) {
        char result[1];
        result[0] = (c >> 0 & 0x7F) | 0x00;
        write(1, result, 1);
    } else if (c < 0x0800) {
        char result[2];
        result[0] = (c >> 6 & 0x1F) | 0xC0;
        result[1] = (c >> 0 & 0x3F) | 0x80;
        write(1, result, 2);
    } else if (c < 0x010000) {
        char result[3];
        result[0] = (c >> 12 & 0x0F) | 0xE0;
        result[1] = (c >> 6 & 0x3F) | 0x80;
        result[2] = (c >> 0 & 0x3F) | 0x80;
        write(1, result, 3);
    } else {
        char result[4];
        result[0] = (c >> 18 & 0x07) | 0xF0;
        result[1] = (c >> 12 & 0x3F) | 0x80;
        result[2] = (c >> 6 & 0x3F) | 0x80;
        result[3] = (c >> 0 & 0x3F) | 0x80;
        write(1, result, 4);
    }
}
