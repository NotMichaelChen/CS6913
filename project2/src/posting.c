#include "posting.h"

#include <string.h>

#include "michaellib/utility.h"

int memposting_cmp(const MemPosting* a, const MemPosting* b) {
    int res = strcmp(string_getString(a->term), string_getString(b->term));
    if(res)
        return res;
    else {
        (a->docID < b->docID) ? -1 : (a->docID > b->docID);
    }
}

size_t memposting_getSize(MemPosting* posting) {
    // add three for two spaces and one newline
    return util_getDigitCount(posting->docID) + util_getDigitCount(posting->freq) + string_getLen(posting->term) + 3;
}