// Acknowledgements
// - PriorityQueue ADT was taken from PqArray.c from lectures
// was modified to handle structures in stage 3

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "PriorityQueue.h"

#define INITIAL_CAPACITY 8

struct pq {
	struct pqItem *items;
    int *positions;
	int numItems;
	int capacity;
};

struct pqItem {
	int item;
	int priority;
};

static void resize(Pq pq);
static void heapifyUp(Pq pq, int index);
static void heapifyDown(Pq pq, int index);
static void swap(Pq pq, int i, int j);

Pq PqNew(void) {
	Pq pq = malloc(sizeof(struct pq));
	if (pq == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

	pq->numItems = 0;
	pq->capacity = INITIAL_CAPACITY;
	pq->items = malloc(pq->capacity * sizeof(struct pqItem));
    pq->positions = malloc(pq->capacity * sizeof(int));

    if (pq->items == NULL || pq->positions == NULL) {
        fprintf(stderr, "Error: Out of memory");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < pq->capacity; i++) {
        pq->positions[i] = -1;
    }

	return pq;
}

void PqFree(Pq pq) {
	free(pq->items);
    free(pq->positions);
	free(pq);
}

void PqInsert(Pq pq, int item, int priority) {
	if (pq->numItems == pq->capacity) {
		resize(pq);
	}

	pq->items[pq->numItems].item = item;
	pq->items[pq->numItems].priority = priority;
    pq->positions[item] = pq->numItems;

    heapifyUp(pq, pq->numItems);

	pq->numItems++;
}

int PqSize(Pq pq) {
    return pq->numItems;
}

int PqDelete(Pq pq) {
	if (pq->numItems == 0) {
		fprintf(stderr, "error: pq is empty\n");
		exit(EXIT_FAILURE);
	}

    int item = pq->items[0].item;

    pq->numItems--;
    pq->items[0] = pq->items[pq->numItems];
    pq->positions[pq->items[0].item] = 0;

    heapifyDown(pq, 0);

	return item;
}

void PqUpdate(Pq pq, int item, int priority) {
    int index = pq->positions[item];
    if (index == -1) {
        fprintf(stderr, "Error: item not found in priority queue");
        exit(EXIT_FAILURE);
    }

    pq->items[index].priority = priority;
    heapifyUp(pq, index);
}

int PqPeek(Pq pq) {
	if (pq->numItems == 0) {
		fprintf(stderr, "error: pq is empty\n");
		exit(EXIT_FAILURE);
	}

	return pq->items[0].item;
}

//////////////////////////////////////////////////////////

// helper function to resize the priority queue
static void resize(Pq pq) {
    int oldCapacity = pq->capacity;
	pq->capacity *= 2;
	pq->items = realloc(pq->items, pq->capacity * sizeof(struct pqItem));
    pq->positions = realloc(pq->positions, pq->capacity * sizeof(int));
	
    if (pq->items == NULL || pq->positions == NULL) {
		fprintf(stderr, "error: out of memory\n");
		exit(EXIT_FAILURE);
	}

    for (int i = oldCapacity; i < pq->capacity; i++) {
        pq->positions[i] = -1;
    }
}

// helper function to maintain the heap property by moving an item up
static void heapifyUp(Pq pq, int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (pq->items[index].priority >= pq->items[parentIndex].priority) {
            break;
        }

        swap(pq, index, parentIndex);
        index = parentIndex;
    }
}

// helper function to maintain the heap property by moving an item down
static void heapifyDown(Pq pq, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < pq->numItems && pq->items[left].priority < pq->items[smallest].priority) {
        smallest = left;
    }
    if (right < pq->numItems && pq->items[right].priority < pq->items[smallest].priority) {
        smallest = right;
    }
    if (smallest != index) {
        swap(pq, index, smallest);
        heapifyDown(pq, smallest);
    }
}

// helper function to swap two items in the heap
static void swap(Pq pq, int i, int j) {
    struct pqItem temp = pq->items[i];
    pq->items[i] = pq->items[j];
    pq->items[j] = temp;

    pq->positions[pq->items[i].item] = i;
    pq->positions[pq->items[j].item] = j;
}