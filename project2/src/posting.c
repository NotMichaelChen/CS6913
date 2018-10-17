#include "posting.h"

#include <string.h>

#include "michaellib/utility.h"

int memposting_cmp(const void* a, const void* b) {
    const MemPosting *c = a, *d = b;

    int res = strcmp(string_getString(c->term), string_getString(d->term));
    if(res)
        return res;
    else {
        return (c->docID < d->docID) ? -1 : (c->docID > d->docID);
    }
}