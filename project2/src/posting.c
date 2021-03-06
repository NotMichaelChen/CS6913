#include "posting.h"

#include <string.h>

#include "michaellib/utility.h"

// Compare the posting by term first, then by docID
int memposting_cmp(const void* a, const void* b) {
    const MemPosting *c = a, *d = b;

    int res = strcmp(string_getString(c->term), string_getString(d->term));
    if(res)
        return res;
    else {
        return (c->docID > d->docID) - (c->docID < d->docID);
    }
}