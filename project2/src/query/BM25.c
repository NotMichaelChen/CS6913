#include "BM25.h"

#include <stdio.h>
#include <math.h>

double IDF(unsigned int totaldocs, unsigned int docscontaining) {
    double numerator = totaldocs - docscontaining + 0.5;
    double denominator = docscontaining + 0.5;
    return log10(numerator / denominator);
}

double BMS25(freq_t* freqs, size_t freqcount, uint32_t* docscontaining,
    size_t docscontaininglen, uint32_t doclength, double avgdoclength,
    uint32_t totaldocs)
{
    //free parameters
    double k = 1.2;
    double b = 0.75;

    if(freqcount != docscontaininglen) {
        fprintf(stderr, "Error, freq and docscontaining arrays mismatched in size during BM25. %zu vs %zu",
            freqcount,
            docscontaininglen
        );
        exit(1);
    }

    double score = 0;

    for(size_t i = 0; i < freqcount; i++) {
        double numerator = freqs[i] * (k + 1);
        double denominator = freqs[i] + k * (1 - b + b * (doclength / avgdoclength));
        score += IDF(totaldocs, docscontaining[i]) * (numerator/denominator);
    }

    return score;
}