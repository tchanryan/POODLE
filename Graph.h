// Acknowledgements
// - Graph ADT upto GraphRemoveEdge was taken from GraphArrayOfEdges.c from lectures
// modified to fit subsets and other functions created

#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

typedef struct graph *Graph;

typedef struct Vertex {
    int id;
    int *securityLevel;
    int *poodleTime;
} Vertex;

// Returns a new graph with nV vertices
Graph GraphNew(int nV);

// Frees all memory allocated to a graph
void GraphFree(Graph g);

// Returns the number of vertices in a graph
int GraphNumVertices(Graph g);

// Returns the number of edges in a graph
int GraphNumEdges(Graph g);

// Returns true if there is a edge between two vertices and false if not
bool GraphIsAdjacent(Graph g, int v, int w);

// Adds an edge beteeen v and w
void GraphInsertEdge(Graph g, int v, int w, int transmissionTime);

// Removes an edge between v and w
void GraphRemoveEdge(Graph g, int v, int w);

// Displays a graph
// void GraphShow(Graph g);

// Set the security level and poodle time for a vertex
void GraphSetVertexInfo(Graph g, int v, int securityLevel, int poodleTime);

// Gets the security level of a vertex
int GraphGetSecurityLevel(Graph g, int v);

// Gets the poodle time of a vertex
int GraphGetPoodleTime(Graph g, int v);

// Gets the transmission time
int GraphGetTransmissionTime(Graph g, int v, int w);

// Gets the number of neighbours of a certain vertex
int GraphNeighbourCount(Graph g, int v);

// Gets the neighborus of a vertex
int *GraphNeighbours(Graph g, int v);

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif