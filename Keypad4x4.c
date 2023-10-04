#include "Keypad4x4.h"

signed char pushKey()
{
    // P0 for keypad
    char num = 0;
    for (char r = 0; r < 4; r++) {
        P0 = ~(0x10 << r);         // select row
        for (char c = 0; c < 4; c++) {
            // 0x0F = 0000 1111 -> column mask
            if ((P0 & 0x0F) == (~(1 << c) & 0x0F)) {   // check which column, filter row bits
                return r * 4 + c;                  // calculate pressed key by row and column
            }
        }

    }
    return -1;      // return -1 if none of the key is pressed
}
