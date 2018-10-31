#include "utility.h"

#include <math.h>

inline int util_getDigitCount(size_t num) {
    return (num == 0) ? 2 : (sizeof(char)*(int)log10(num))+2;
}