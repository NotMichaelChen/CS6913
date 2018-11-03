#include "minheap.h"

struct MinHeap {
    double* buf;
    size_t len;
};

MinHeap* minheap_new(size_t count) {
    MinHeap* heap = malloc(sizeof (MinHeap));
    heap->len = count;

    heap->buf = calloc(count, sizeof (double));

    return heap;
}

void minheap_attemptInsert(MinHeap* heap, double num) {
    if(num > heap->buf[0]) {
        //Swap with end to remove min element
        heap->buf[0] = heap->buf[heap->len - 1];

        // trickle-down
        size_t index = 0;
        while(index*2+2 < heap->len) {
            size_t swapindex = heap->buf[index*2+1] > heap->buf[index*2+2] ?
                index*2+2 : index*2+1;
            
            if(heap->buf[index] > heap->buf[swapindex]) {
                double tmp = heap->buf[index];
                heap->buf[index] = heap->buf[swapindex];
                heap->buf[swapindex] = tmp;

                index = swapindex;
            }
            else
                break;
        }

        //Place new num at end
        heap->buf[heap->len - 1] = num;

        // trickle-up
        index = heap->len - 1;
        while(index != 0 && heap->buf[(index - 1) / 2] > heap->buf[index]) {
            double tmp = heap->buf[index];
            heap->buf[index] = heap->buf[(index - 1) / 2];
            heap->buf[(index - 1) / 2] = tmp;

            index = (index - 1) / 2;
        }
    }
}

size_t minheap_len(MinHeap* heap) {
    return heap->len;
}

double* minheap_getArr(MinHeap* heap) {
    return heap->buf;
}

void minheap_free(MinHeap* heap) {
    free(heap->buf);
    free(heap);
}