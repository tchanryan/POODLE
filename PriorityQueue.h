// Acknowledgements
// - PriorityQueue ADT was taken from Pq.h from lectures
// was modified to handle structures in stage 3

#ifndef PQ_H
#define PQ_H

typedef struct pq *Pq;

Pq PqNew(void);

void PqFree(Pq pq);

void PqInsert(Pq pq, int item, int priority);

int PqSize(Pq pq);

int PqDelete(Pq pq);

void PqUpdate(Pq pq, int item, int priority);

int PqPeek(Pq pq);

#endif
