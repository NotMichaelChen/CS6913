#pragma once

#include <stdlib.h>
#include <stdint.h>

#include "global.h"

double BMS25(freq_t* freqs, size_t freqcount, uint32_t* docscontaining,
    size_t docscontaininglen, uint32_t doclength, double avgdoclength,
    uint32_t totaldocs);