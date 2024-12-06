
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "poodle.h"
#include "Graph.h"
#include "Queue.h"
#include "PriorityQueue.h"

// STAGE 1 HELPER FUNCTIONS
static int connected(Graph pug, int start, bool visited[], int can_visit[], int *count);
static void CreateGraph(Graph g, int numComputers, int numConnections, struct connection connections[], struct computer computers[]);

// STAGE 2 HELPER FUNCTIONS
static void insertionSort(int array[], int n);

// STAGE 3 HELPER FUNCTIONS
static void dijkstra(Graph g, int src, int dist[], bool sptSet[], struct computer computers[]);
static int compareSteps(const void *a, const void *b);
static struct poodleResult returnResult(Graph g, int dist[], int numComputers, struct computer computers[]);
static struct computerList *createRecipientList(Graph g, int computer, int dist[], struct computer computers[]);

////////////////////////////////////////////////////////////////////////
// Task 1

struct probePathResult probePath(
	struct computer computers[], int numComputers,
	struct connection connections[], int numConnections,
	int path[], int pathLength
) {
	struct probePathResult res = {SUCCESS, 0};
	
	Graph pug = GraphNew(numComputers);
	CreateGraph(pug, numComputers, numConnections, connections, computers);

	bool *visited = calloc(numComputers, sizeof(bool));

	if (pathLength == 1) {
		res.elapsedTime += GraphGetPoodleTime(pug, path[0]);
		visited[path[0]] = true;
		GraphFree(pug);
		free(visited);

		return res;
	}

	for (int k = 0; k < pathLength - 1; k++) {
		int first = path[k];
		int second = path[k + 1];

		if (visited[first] == false) {
			res.elapsedTime += GraphGetPoodleTime(pug, first);
			visited[first] = true;
		}		

		if (GraphIsAdjacent(pug, first, second) == false) {
			res.status = NO_CONNECTION;
			GraphFree(pug);
			free(visited);
			return res;
		}

		if (GraphGetSecurityLevel(pug, first) + 1 < GraphGetSecurityLevel(pug, second)) {
			res.status = NO_PERMISSION;
			GraphFree(pug);
			free(visited);
			return res;
		}

		int transmissionTime = GraphGetTransmissionTime(pug, first, second);
		res.elapsedTime += transmissionTime;

		if (visited[second] == false) {
			res.elapsedTime += GraphGetPoodleTime(pug, second);
			visited[second] = true;
		}
	}

	GraphFree(pug);
	free(visited);

	return res;
}

////////////////////////////////////////////////////////////////////////
// Task 2

struct chooseSourceResult chooseSource(
	struct computer computers[], int numComputers,
	struct connection connections[], int numConnections
) {
	struct chooseSourceResult res = {0, 0, NULL};

	Graph pug = GraphNew(numComputers);
	CreateGraph(pug, numComputers, numConnections, connections, computers);

	int max_computers_visited = 0;
	int optimal_source = -1;
	int *optimal_computer = NULL; 

	for (int k = 0; k < numComputers; k++) {
		bool *visited = calloc(numComputers, sizeof(bool));
		int *can_visit = calloc(numComputers, sizeof(int));

		int count = 0;
		int computers_visited = connected(pug, k, visited, can_visit, &count);

		if (computers_visited > max_computers_visited) {
			max_computers_visited = computers_visited;
			optimal_source = k;

			if (optimal_computer != NULL) {
				free(optimal_computer);
			}

			optimal_computer = malloc(computers_visited * sizeof(int));
			for (int l = 0; l < computers_visited; l++) {
				optimal_computer[l] = can_visit[l];
			}

			insertionSort(optimal_computer, computers_visited);
		}

		free(visited);
		free(can_visit);
	}

	GraphFree(pug);

	res.sourceComputer = optimal_source;
	res.numComputers = max_computers_visited;
	res.computers = optimal_computer;
	
	return res;
}

////////////////////////////////////////////////////////////////////////
// Task 3

struct poodleResult poodle(
	struct computer computers[], int numComputers,
	struct connection connections[], int numConnections,
	int sourceComputer
) {
	struct poodleResult res = {0, NULL};

	Graph pug = GraphNew(numComputers);
	CreateGraph(pug, numComputers, numConnections, connections, computers);

	int *dist = malloc(numComputers * sizeof(int));
	bool *sptSet = malloc(numComputers * sizeof(bool));

	if (dist == NULL || sptSet == NULL) {
		fprintf(stderr, "Error: out of memory");
		exit(1);
	}

	dijkstra(pug, sourceComputer, dist, sptSet, computers);

	res = returnResult(pug, dist, numComputers, computers);

	free(dist);
	free(sptSet);
	GraphFree(pug);

	return res;
}

////////////////////////////////////////////////////////////////////////
// Task 4

/**
 * Describe your solution in detail here:
 *
 * TODO
 */
struct poodleResult advancedPoodle(
	struct computer computers[], int numComputers,
	struct connection connections[], int numConnections,
	int sourceComputer
) {
	struct poodleResult res = {0, NULL};

	return res;
}

///////////////////////////////////////////////////// STAGE 1 HELPER FUNCTIONS ///////////////////////////////////////////////
// a helper function that finds the most amount of (downstream) connections for a specified computer
static int connected(Graph pug, int start, bool visited[], int can_visit[], int *count) {
	Queue q = QueueNew();
	QueueEnqueue(q, start);

	visited[start] = true;
	can_visit[(*count)++] = start;

	while (QueueSize(q) > 0) {
		int v = QueueDequeue(q);
		int *adjacent = GraphNeighbours(pug, v);
		int numAdjacent = GraphNeighbourCount(pug, v);

		for (int i = 0; i < numAdjacent; i++) {
			int neighbour = adjacent[i];

			if (visited[neighbour] == false && (GraphGetSecurityLevel(pug, v) + 1 >= GraphGetSecurityLevel(pug, neighbour))) {
				QueueEnqueue(q, neighbour);
				visited[neighbour] = true;
				can_visit[(*count)++] = neighbour;
			}
		}

		free(adjacent);
	}

	QueueFree(q);

	return *count;
}

// a helper function that creates the graph
static void CreateGraph(Graph g, int numComputers, int numConnections, struct connection connections[], struct computer computers[]) {
	for (int i = 0; i < numComputers; i++) {
		GraphSetVertexInfo(g, i, computers[i].securityLevel, computers[i].poodleTime);
	}

	for (int j = 0; j < numConnections; j++) {
		GraphInsertEdge(g, connections[j].computerA, connections[j].computerB, connections[j].transmissionTime);
	}
}

//////////////////////////////////////////////// STAGE 2 HELPER FUNCTIONS /////////////////////////////////////////////////////
// a helper function that does an insertion sort
static void insertionSort(int array[], int n) {
	for (int i = 1; i < n; i++) {
		int key = array[i];
		int j = i - 1;

		while (j >= 0 && array[j] > key) {
			array[j + 1] = array[j];
			j = j - 1;
		}

		array[j + 1] = key;
	}
}

////////////////////////////////////////////// STAGE 3 HELPER FUNCTIONS //////////////////////////////////////////////////////
// a helper function that performs djsktra's alogrithm 
// initial idea from https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/ 
// original lines of code that have been modified are marked with comments (9 lines of code)
static void dijkstra(Graph g, int src, int dist[], bool sptSet[], struct computer computers[]) {
	int numVert = GraphNumVertices(g);
	Pq pq = PqNew();
	
	for (int i = 0; i < numVert; i++) { 														// Initialize all distances as INFINITE and stpSet[] as false
		dist[i] = INT_MAX;
		sptSet[i] = false;
		PqInsert(pq, i, INT_MAX);
	}

	dist[src] = computers[src].poodleTime;														// The output array. Distance of source vertex from itself is always poodleTime

	PqUpdate(pq, src, computers[src].poodleTime);																						 

	while (PqSize(pq) > 0) {																	// Find shortest path for all vertices
		int v = PqDelete(pq);																	// Pick the minimum distance vertex from the set of vertices not yet processed. v is always equal to src in the first iteration.
		
		if (sptSet[v] == true) {																// sptSet[i] will be true if vertex i is included in shortest path tree or shortest distance from src to i is finalized
			continue;
		}

		sptSet[v] = true; 																		// Mark the picked vertex as processed
		int *adjacent = GraphNeighbours(g, v);
		int numAdjacent = GraphNeighbourCount(g, v);

		for (int i = 0; i < numAdjacent; i++) {													// Update dist value of the adjacent vertices of the picked vertex.
			int u = adjacent[i];
			int transmissionTime = GraphGetTransmissionTime(g, v, u);

			if ((computers[u].securityLevel <= computers[v].securityLevel + 1)) {				
				int overflowTimeCheck = INT_MAX - transmissionTime - computers[u].poodleTime;

				if (dist[v] != INT_MAX && dist[v] <= overflowTimeCheck) {
					int distNext = dist[v] + transmissionTime + computers[u].poodleTime;

					if (distNext < dist[u]) {													// Update dist[u] only if is not in sptSet, there is an edge from u to v, 
						dist[u] = distNext;														// and total weight of path from  src to u through v is smaller than current value of dist[u]
						PqUpdate(pq, u, distNext);
					}
				} 
			}
		}

		free(adjacent);
	}
	
	PqFree(pq);
}

// a helper function that constructs poodleResult from the distance array 
static struct poodleResult returnResult(Graph g, int dist[], int numComputers, struct computer computers[]) {
	struct poodleResult res = {0, NULL};
	res.steps = malloc(numComputers * sizeof(struct step));

	if (res.steps == NULL) {
		fprintf(stderr, "Error: Out of memory");
		exit(1);
	}

	int count = 0;
	for (int i = 0; i < numComputers; i++) {

		if (dist[i] != INT_MAX) {
			res.steps[count].computer = i;
			res.steps[count].time = dist[i];
			res.steps[count].recipients = createRecipientList(g, i, dist, computers);
			count++;
		}
	}

	qsort(res.steps, count, sizeof(struct step), compareSteps);
	res.numSteps = count;

	return res;
}

// a helper function that compares the steps for qsort
static int compareSteps(const void *a, const void *b) {
	struct step *stepA = (struct step *)a;
	struct step *stepB = (struct step *)b;

	return (stepA->time - stepB->time);
}

// a helper function that creates the linked list of recipients for a given computer
static struct computerList *createRecipientList(Graph g, int computer, int dist[], struct computer computers[]) {
	struct computerList *head = NULL;
	int *adjacent = GraphNeighbours(g, computer);
	int numAdjacent = GraphNeighbourCount(g, computer);

	for (int i = 0; i < numAdjacent; i++) {
		int x = adjacent[i];
		int transmissionTime = GraphGetTransmissionTime(g, computer, x);
		int distNext = dist[computer] + transmissionTime + computers[x].poodleTime;

		if (dist[x] == distNext && computers[x].securityLevel <= computers[computer].securityLevel + 1) {
			struct computerList *newNode = malloc(sizeof(struct computerList));
			 
			if (newNode == NULL) {
				fprintf(stderr, "Error: out of memory");
				exit(1);
			}

			newNode->computer = x;
			newNode->next = NULL;
			
			if (head == NULL || head->computer > newNode->computer) {
				newNode->next = head;
				head = newNode;
			} else {
				struct computerList *current = head;

				while (current->next != NULL && current->next->computer < newNode->computer) {
					current = current->next;
				}

				newNode->next = current->next;
				current->next = newNode;
			}
		}
	}

	free(adjacent);

	return head;
}