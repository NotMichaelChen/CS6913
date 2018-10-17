#include "utility.h"

#include <math.h>

int util_getDigitCount(size_t num) {
    return (num == 0) ? 2 : (sizeof(char)*(int)log10(num))+2;
}

int util_getMempostingSize(size_t docID, size_t freq, size_t stringlen) {
    // Add 2 spaces, add 1 newline, subtract 2 from both getDigitcount
    return docID + freq + stringlen + 1;
}