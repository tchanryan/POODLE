// Acknowledgements
// - Graph ADT upto GraphRemoveEdge was taken from GraphArrayOfEdges.c from lectures
// modified to fit subsets and other functions created

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"

struct graph {
    int nV;
    int nE;
    Vertex *vertices;
    struct adjNode **edges;
};

struct adjNode {
    Vertex vertex;
    int transmissionTime;
    struct adjNode *next;
};

static bool validVertex(Graph g, int v);

// Returns a new graph with nV vertices
Graph GraphNew(int nV) {
    Graph g = malloc(sizeof(struct graph));
    g->edges = calloc(nV, sizeof(struct adjNode *));
    g->vertices = calloc(nV, sizeof(Vertex));
    g->nV = nV;
    g->nE = 0;
    
    for (int i = 0; i < nV; i++) {
        g->vertices[i].id = i;
        g->vertices[i].securityLevel = malloc(sizeof(int));
        g->vertices[i].poodleTime = malloc(sizeof(int));
    }

    return g;
}

// Frees all memory allocated to graph
void GraphFree(Graph g) {
    for (int i = 0; i < g->nV; i++) {
		struct adjNode *curr = g->edges[i];
		while (curr != NULL) {
			struct adjNode *temp = curr;
			curr = curr->next;
			free(temp);
		}
        free(g->vertices[i].securityLevel);
        free(g->vertices[i].poodleTime);
	}

	free(g->edges);
    free(g->vertices);
	free(g);
}

// Returns the number of vertices in a graph
int GraphNumVertices(Graph g) {
	return g->nV;
}

// Returns the number of edges in a graph

int GraphNumEdges(Graph g) {
	return g->nE;
}

// Returns true if there is an edge between v and w,
// and false otherwise
bool GraphIsAdjacent(Graph g, int v, int w) {
	assert(validVertex(g, v));
	assert(validVertex(g, w));

    struct adjNode *curr = g->edges[v];
    while (curr != NULL) {
        if (curr->vertex.id == w) {
            return true;
        }
        curr = curr->next;
    }

	return false;
}

// Inserts an edge between v and w
void GraphInsertEdge(Graph g, int v, int w, int transmissionTime) {
	assert(validVertex(g, v));
	assert(validVertex(g, w));

	if (!GraphIsAdjacent(g, v, w)) {
        struct adjNode *nodeV = malloc(sizeof(struct adjNode));
        nodeV->vertex = g->vertices[w];
        nodeV->transmissionTime = transmissionTime;
        nodeV->next = g->edges[v];
        g->edges[v] = nodeV;

        struct adjNode *nodeW = malloc(sizeof(struct adjNode));
        nodeW->vertex = g->vertices[v];
        nodeW->transmissionTime = transmissionTime;
        nodeW->next = g->edges[w];
        g->edges[w] = nodeW;

		g->nE++;
	}
}

// Removes an edge between v and w
void GraphRemoveEdge(Graph g, int v, int w) {
	assert(validVertex(g, v));
	assert(validVertex(g, w));

    struct adjNode *curr = g->edges[v];
    struct adjNode *prev = NULL;

    while (curr != NULL && curr->vertex.id != w) {
        prev = curr;
        curr = curr->next;
    }
    if (curr != NULL) {
        if (prev == NULL) {
            g->edges[v] = curr->next;
        } else {
            prev->next = curr->next;
        }
        free(curr);
    }

    curr = g->edges[w];
    prev = NULL;
    while (curr != NULL && curr->vertex.id != v) {
        prev = curr;
        curr = curr->next;
    }
    if (curr != NULL) {
        if (prev == NULL) {
            g->edges[w] = curr->next;
        } else {
            prev->next = curr->next;
        }
        free(curr);
    }
    g->nE--;
}

// // Displays a graph
// void GraphShow(Graph g) {
// 	printf("Number of vertices: %d\n", g->nV);
// 	printf("Number of edges: %d\n", g->nE);
// 	printf("Edges:\n");
// 	for (int i = 0; i < g->nE; i++) {
// 		printf("(%d, %d)\n", g->edges[i].v, g->edges[i].w);
// 	}
//     printf("Vertices:\n");
// 	for (int i = 0; i < g->nV; i++) {
// 		printf("Vertex %d: Security Level: %d Poodle Time: %d\n", g->vertices[i].id, 
//         *(g->vertices[i].securityLevel), *(g->vertices[i].poodleTime));
// 	}
// 	printf("\n");
// }

// Set the security level and poodle time for a vertex
void GraphSetVertexInfo(Graph g, int v, int securityLevel, int poodleTime) {
    assert(validVertex(g, v));
    *(g->vertices[v].securityLevel) = securityLevel;
    *(g->vertices[v].poodleTime) = poodleTime;
}

// Get the security level of a vertex
int GraphGetSecurityLevel(Graph g, int v) {
    assert(validVertex(g, v));
    return *(g->vertices[v].securityLevel);
}

// Get the poodle time of a vertex
int GraphGetPoodleTime(Graph g, int v) {
    assert(validVertex(g, v));
    return *(g->vertices[v].poodleTime);
}

// Get the transmission time between vertices
int GraphGetTransmissionTime(Graph g, int v, int w) {
    assert(validVertex(g, v));
    assert(validVertex(g, w));
    
    struct adjNode *curr = g->edges[v];
    while (curr != NULL) {
        if (curr->vertex.id == w) {
            return curr->transmissionTime;
        }
        curr = curr->next;
    }

    return -1;
}

// Checks if a vertex is valid
static bool validVertex(Graph g, int v) {
	return (v >= 0 && v < g->nV);
}

// Gets the number of neighbours of a certain vertex
int GraphNeighbourCount(Graph g, int v) {
    int count = 0;
    struct adjNode *curr = g->edges[v];
    
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }

    return count;
}

// Gets the neighborus of a vertex
int *GraphNeighbours(Graph g, int v) {
    int count = GraphNeighbourCount(g, v);
    int *adjacent = malloc(count * sizeof(int));

    if (adjacent == NULL) {
        fprintf(stderr, "Error: Out of memory");
        exit(1);
    }

    int i = 0;
    struct adjNode *curr = g->edges[v];
    while (curr != NULL) {
        adjacent[i++] = curr->vertex.id;
        curr = curr->next;
    }

    return adjacent;
}
