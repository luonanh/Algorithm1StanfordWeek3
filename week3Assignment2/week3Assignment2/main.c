//
//  main.c
//  week3Assignment2
//
//  Created by Anh Luong on 7/31/16.
//  Copyright © 2016 Anh Luong. All rights reserved.
//
/*
 The file contains the adjacency list representation of a simple undirected graph. There are 200 vertices labeled 1 to 200. The first column in the file represents the vertex label, and the particular row (other entries except the first column) tells all the vertices that the vertex is adjacent to. So for example, the 6th row looks like : "6	155	56	52	120	......". This just means that the vertex with label 6 is adjacent to (i.e., shares an edge with) the vertices with labels 155,56,52,120,......,etc
 
 Your task is to code up and run the randomized contraction algorithm for the min cut problem and use it on the above graph to compute the min cut. (HINT: Note that you'll have to figure out an implementation of edge contractions. Initially, you might want to do this naively, creating a new graph from the old every time there's an edge contraction. But you should also think about more efficient implementations.) (WARNING: As per the video lectures, please make sure to run the algorithm many times with different random seeds, and remember the smallest cut that you ever find.) Write your numeric answer in the space provided. So e.g., if your answer is 5, just type 5 in the space provided.
 */

#include <stdio.h>
#include <stdlib.h> // for malloc() and free()
#include <string.h> // for strtok()
#include <assert.h>
#include <time.h>   // for time()
#include <unistd.h> // for sleep(seconds)

#define MAX_VERTEX      200
#define REPETITION      100


#ifdef DEBUG
#undef DEBUG
//#define DEBUG 0
#endif

#ifdef DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT
#endif

typedef enum {FALSE, TRUE} BOOLEAN;

typedef struct _node {
    int     label;
    int     *neighbors;
    int     numNeighbors;
} node;

typedef struct _edge {
    int     n1; // label of node 1
    int     n2; // label of node 2
} edge;

typedef struct _graph {
    int     numNode;
    node    *nodeList;
    int     *existedNode;
} graph;

void printGraph(graph *g);
void sortGraphNodeNeighbors(graph *g);
int getFirstParallelEdge(graph *g, int node);
void printParallelEdge(graph *g);
node *getNodeFromLabel(graph *g, int label);
void removeNode(graph *g, int label);
void printNeighbor(graph *g, int label);
int myRand(int limit);
void myPrng(void);
int randomContract(graph *g);
void cleanupGraph(graph *g);

// Assignment 2
int partition(int *A, int left, int right, int pivot);
void swap(int *A, int src, int dest);
void myqsort(int *A, int left, int right);
void printArray(int *A, int left, int right);
BOOLEAN isIntArraySortedAscending(int *A, int left, int right);


int choosePivotFirst(int *A, int left, int right)
{
    return left;
}

int choosePivotLast(int *A, int left, int right)
{
    return right;
}

int choosePivotMedian(int *A, int left, int right)
{
    int median = (left + right) / 2;
    int m, l, r;
    
    m = *(A + median);
    l = *(A + left);
    r = *(A + right);
    
    if ((l <= m && m <= r) || (r <= m && m <= l))
        return median;
    else if ((m <= l && l <= r) || (r <= l && l <= m))
        return left;
    else    // if ((m <= r && r<= l) || (l <= r && r <= m))
        return right;
}

int partition(int *A, int left, int right, int pivot)
{
    int p, i, j;    // p is pivot element, i is position of the 1st element in the partitioned part that is not smaller than element at pivot, j is the start of the unpartitioned part
    
    p = *(A + pivot);
    swap(A, left, pivot);
    DEBUG_PRINT("=========\n");
    DEBUG_PRINT("Start partition function, pivot is %d, p is %d, left is %d, right is %d\n", pivot, p, left, right);
    printArray(A, left, right);
    for (i = j = left + 1; j <= right; j++) {
        if (*(A + j) < p) {
            DEBUG_PRINT("%dth %d is smaller than %d\n", j, *(A + j), p);
            swap(A, i++, j);
        }
    }
    printArray(A, left, right);
    DEBUG_PRINT("i is %d\n", i);
    swap(A, left, i - 1);
    printArray(A, left, right);
    DEBUG_PRINT("End partition function\n");
    DEBUG_PRINT("=========\n");
    return i - 1;   // return the position of the pivot after partitioning.
}

void swap(int *A, int src, int dest)
{
    int temp;
    
    DEBUG_PRINT("Swap %d with %d\n", src, dest);
    temp = *(A + src);
    *(A + src) = *(A + dest);
    *(A + dest) = temp;
}

void myqsort(int *A, int left, int right)
{
    int p;
    
    //if (left >= right || (right - left) == 1)
    if (left >= right)
        return;
    
    DEBUG_PRINT("-------> qsort with left %d right %d\n", left, right);
    printArray(A, left, right);
    p = choosePivotFirst(A, left, right);
    DEBUG_PRINT("choosePivotFirst() p = %d\n", p);
    p = partition(A, left, right, p);
    DEBUG_PRINT("After partition() p = %d\n", p);
    printArray(A, left, right);
    //countCompare += (left < p - 1) ? p - 1 - left : 0;
    //DEBUG_PRINT("countCompare 1st half is %d, left is %d, right is %d\n", countCompare, left, p - 1);
    myqsort(A, left, p - 1);
    
    //countCompare += (p + 1 < right) ? right - p - 1 : 0;
    //DEBUG_PRINT("countCompare 2nd half is %d, left is %d, right is %d\n", countCompare, p + 1, right);
    myqsort(A, p + 1, right);
    
    printArray(A, left, right);
}

void printArray(int *A, int left, int right)
{
    /*
     int i;
     
     printf("Array ");
     for (i = left; i <= right; i++) {
     printf("%dth is %d | ", i, *(A + i));
     }
     printf("\n");
     */
}

BOOLEAN isIntArraySortedAscending(int *A, int left, int right)
{
    int i;
    for (i = left; i < right; i++)
        if (A[i] > A[i+1])
            return FALSE;
    
    return TRUE;
}

int main(int argc, const char * argv[]) {
    FILE *fp;
    //node nodeList[MAX_VERTEX];
    node *nodeList = NULL;
    int lineCounter, wordCounter = 0;
    //int neighbor[MAX_VERTEX];
    int *neighbor = NULL;
    char line[3 * MAX_VERTEX + 1];
    //char *line = NULL;
    int i, j, average, min, max, sum, temp;
    char *token;
    const char s[] = " \t";
    graph g;
    
    argv++;
    sum = average = min = max = 0;
    for (j = 0; j < REPETITION; j++) {
        if ((fp = fopen(*argv, "r")) == NULL) {
            printf("read failed\n");
            return 1;
        }
        //delay(1000);
        myPrng();
        
        nodeList = (node *)malloc(MAX_VERTEX * sizeof(node));
        neighbor = (int *)malloc(MAX_VERTEX * sizeof(int));
        //line = (char *)malloc((3 * MAX_VERTEX + 1) * sizeof(char));

    //for (i = 0; i < sizeof(neighbor); i++)
        for (i = 0; i < MAX_VERTEX; i++)
            neighbor[i] = -1;
    
        //DEBUG_PRINT("Sizeof line is %d\n", sizeof(line));
        for (i = 0; i < sizeof(line); i++)
        //for (i = 0; i < (3 * MAX_VERTEX + 1); i++)
            *(line + i) = 0x20;
        
        //for (i = 0; i < sizeof(nodeList); i++) {
        for (i = 0; i < MAX_VERTEX; i++) {
            nodeList[i].label = 0;
            nodeList[i].numNeighbors = 0;
            nodeList[i].neighbors = NULL;
        }
    
    // read graph input file
        
    //for (lineCounter = 0; fgets(line, sizeof(line) - 1, fp) != NULL; lineCounter++) {
        for (lineCounter = 0; fgets(line, 3 * MAX_VERTEX, fp) != NULL; lineCounter++) {
        
        DEBUG_PRINT("line %s\n", line);
        token = strtok(line, s);
        wordCounter = 0;
        while (token != NULL) {
            sscanf(token, "%d", &neighbor[wordCounter]);
            DEBUG_PRINT("token %d ", neighbor[wordCounter]);
            wordCounter++;  // already counted 1 more than the real number of words
            token = strtok(NULL, s);
        }
        DEBUG_PRINT(" | wordCounter %d\n", wordCounter);
        //nodeList[lineCounter].label = neighbor[0];
        nodeList[lineCounter].label = *neighbor;
        nodeList[lineCounter].numNeighbors = wordCounter - 1;
        nodeList[lineCounter].neighbors = (int *)malloc((wordCounter - 1) * sizeof(int));
        DEBUG_PRINT("nodeList[lineCounter].numNeighbors %d\n", nodeList[lineCounter].numNeighbors);
        for (i = 0; i < nodeList[lineCounter].numNeighbors; i++) {
            nodeList[lineCounter].neighbors[i] = neighbor[i+1];
            //nodeList[lineCounter].neighbors[i] = *neighbor(i+1);
            DEBUG_PRINT("%dth Neighbor is %d", i, nodeList[lineCounter].neighbors[i]);
        }
        
        
        
        //sscanf(line, "%d", &graph[lineCounter].label);
        
        /* Why the below read line and parse integer do NOT work ???
        wordCounter = 0;
        while (sscanf(line, "%d", &neighbor[wordCounter]) == 1) {
            printf("neighbor[%d] is %d\n", wordCounter, neighbor[wordCounter]);
            wordCounter++;
        }
        
        graph[lineCounter].numNeighbors = wordCounter;
        graph[lineCounter].neighbors = (int *)malloc(wordCounter*sizeof(int));
        for (i = 0; i <= wordCounter; i++) {
            graph[lineCounter].neighbors[i] = neighbor[i];
        }
        for (i = 0; i < sizeof(line); i++)
            *(line + i) = 0x20;
         */

    }
    g.numNode = lineCounter;
    g.nodeList = nodeList;
    g.existedNode = (int *)malloc(g.numNode * sizeof(int));
    for (i = 0; i < g.numNode; i++)
        g.existedNode[i] = i + 1;
    

    DEBUG_PRINT("----------------------------------------------------------\n");
    //printGraph(&g);
    DEBUG_PRINT("----------------------------------------------------------\n");
    //sortGraphNodeNeighbors(&g);
    DEBUG_PRINT("----------------------------------------------------------\n");
    printGraph(&g);
    DEBUG_PRINT("----------------------------------------------------------\n");
    //printParallelEdge(&g);
    /*
    for (i = 0; i < MAX_VERTEX; i++) {
        printf("%d ", graph[i].label);
        for (j = 0; j <= graph[i].numNeighbors; j++) {
            printf ("%d ", graph[i].neighbors[j]);
        }
        printf("\n");
    }
     */
    DEBUG_PRINT("----------------------------------------------------------\n");
        temp = randomContract(&g);
        if (j == 0) {
            min = temp;
            max = temp;
        } else {
            if (temp < min)
                min = temp;
            if (temp > max)
                max = temp;
        }
        sum += temp;
        average = sum / (j+1);
        printf("Loop %d, min %d\n", j + 1, min);
        cleanupGraph(&g);
        free(neighbor);
        //free(line);
        /*
        if (nodeList != NULL)
            free(nodeList);
         */
        fclose(fp);
        sleep(1);
    }
    printf("Statistics:");
    printf("Min: %d | Max: %d | Average: %d\n", min, max, average);
    
    return 0;
}

void cleanupGraph(graph *g)
{
    if (g == NULL)
        return;
    if (g->nodeList != NULL) {
        int i, label;
        for (i = 0; i < g->numNode; i++) {
            label = g->existedNode[i];
            if (g->nodeList[label - 1].neighbors != NULL)
                free(g->nodeList[label - 1].neighbors);
        }
        free(g->nodeList);
    }
    if (g->existedNode != NULL)
        free(g->existedNode);
    
    g->numNode = 0;
}

void printGraph(graph *g)
{
    int i, j, label;

    if (g == NULL)
        return;
    
    DEBUG_PRINT("Print Graph start. This graph has %d nodes.\n", g->numNode);
    for (i = 0; i < g->numNode; i++) {
        label = g->existedNode[i];
        DEBUG_PRINT("Node %d has %d adjacent nodes | ", g->nodeList[label - 1].label, g->nodeList[label - 1].numNeighbors);
        for (j = 0; j < g->nodeList[label - 1].numNeighbors; j++) {
            DEBUG_PRINT("%d ", g->nodeList[label - 1].neighbors[j]);
            /*
            if (g->nodeList[i].neighbors != NULL) {  // if we remove the node, we still leave the entry there, hence we need to check if the list of neighbor is NULL or not. If NULL, that means the node was removed from the graph.
                DEBUG_PRINT("%d ", g->nodeList[i].neighbors[j]);
            }
             */
        }
        DEBUG_PRINT("\nPrintGraph end\n");
    }
}

// Why sort does not work after node 1????
void sortGraphNodeNeighbors(graph *g)
{
    int i;
    
    if (g == NULL)
        return;
    
    for (i = 0; i < g->numNode; i++) {
        if (g->nodeList[i].numNeighbors > 1) {
            DEBUG_PRINT("++++++++++++++++++++++++++++++++++++++++++++++++++++");
            myqsort(g->nodeList[i].neighbors, 0, g->nodeList[i].numNeighbors-1);
        }
    }
}

int getFirstParallelEdge(graph *g, int node)
{
    int i;
    if (g == NULL)
        return 0;
    if (g->nodeList[node - 1].numNeighbors < 2)
        return 0;
    
    for (i = 0; i < g->nodeList[node - 1].numNeighbors - 1; i++)
        if (g->nodeList[node - 1].neighbors[i] == g->nodeList[node - 1].neighbors[i + 1])
            return g->nodeList[node - 1].neighbors[i];
    
    return 0;
}

void printParallelEdge(graph *g)
{
    int i, j;
    
    for (i = 0; i < g->numNode; i++) {
        j = getFirstParallelEdge(g, i + 1);
        if (j > 0)
            DEBUG_PRINT("Node %d and node %d has parallel edges\n", i + 1, j);
    }
}

// remove an entry from an array by shifting left the rest of the array
// and initialize the left over to 0.
void removeFromArray(int *a, int index, int length)
{
    int i;

    DEBUG_PRINT("removeFromArray start. \n Remove index %d of length %d\n", index, length);
    if (index == length - 1)    // don't need to do if the removed item is the last item
        return;
    
    // Shift left array by one starting at index
    for (i = index; i < length - 1; i++) {
        a[i] = a[i + 1];
    }
    
    // clear the last entry
    a[length - 1] = 0;
}

// - remove tail from the neighbor list of a head
// - merge the neighbor list of tail to head
void handleHeadNeighbor(graph *g, int head, int tail)
{
    int i, j, k, newNumNeighbor;
    node *n1 = NULL, *n2 = NULL;
    int *neighbor = NULL;

    DEBUG_PRINT("removeNeighbor %d from neighbor list of %d start\n", tail, head);
    n1 = &g->nodeList[head - 1];//getNodeFromLabel(g, nodeLabel);
    if (n1 == NULL)
        return;
    n2 = &g->nodeList[tail - 1];
    if (n2 == NULL)
        return;
    
    // remove tail from the neighbor list of head
    for (i = 0; i < n1->numNeighbors && n1->numNeighbors > 0; i++) {
        if (tail == n1->neighbors[i] || head == n1->neighbors[i]) {
            /*
            if (head == 6)
                DEBUG_PRINT(";;;;;removeFromArray index %d correspond to node %d, and the total numNeighbor is %d\n", i, n1->neighbors[i], n1->numNeighbors);
             */
            DEBUG_PRINT("Working on node %d ;;;;;removeFromArray index %d correspond to node %d, and the total numNeighbor is %d\n", n1->label, i, n1->neighbors[i], n1->numNeighbors-1);
            removeFromArray(n1->neighbors, i, n1->numNeighbors);
            n1->numNeighbors--;
            i--;
        }
    }

    // remove tail from the neighbor list of tail itself
    for (i = 0; i < n2->numNeighbors && n2->numNeighbors > 0; i++) {
        if (tail == n2->neighbors[i] || head == n2->neighbors[i]) {
            DEBUG_PRINT("Working on node %d ;;;;;removeFromArray index %d correspond to node %d, and the total numNeighbor is %d\n", n2->label, i, n2->neighbors[i], n2->numNeighbors-1);
            removeFromArray(n2->neighbors, i, n2->numNeighbors);
            n2->numNeighbors--;
            i--;
        }
    }

    // merge tail's neighbor list to head's neighbor list
    newNumNeighbor = n1->numNeighbors + n2->numNeighbors;
    neighbor = (int *)malloc(newNumNeighbor * sizeof(int));
    for (k = i = j = 0; k < newNumNeighbor; k++) {
        if (i >= n1->numNeighbors)
            neighbor[k] = n2->neighbors[j++];
        else if (j >= n2->numNeighbors)
            neighbor[k] = n1->neighbors[i++];
        else if (n1->neighbors[i] <= n2->neighbors[j])
            neighbor[k] = n1->neighbors[i++];
        else
            neighbor[k] = n2->neighbors[j++];
    }
    free(n1->neighbors);
    n1->neighbors = neighbor;
    n1->numNeighbors = newNumNeighbor;

    if (n1->numNeighbors == 0) {
        DEBUG_PRINT("%d has 0 neighbor after merge with %d. Remove node %d\n", head, tail, n1->label);
        removeNode(g, n1->label);
    }
    
    if (n2->numNeighbors == 0) {
        DEBUG_PRINT("%d has 0 neighbor after merge with %d. Remove node %d\n", tail, head, n2->label);
        removeNode(g, n2->label);
    }
    
    DEBUG_PRINT("removeNeighbor end\n");
}

// remove a node specified by label from the graph.
void removeNode(graph *g, int label)
{
    node *n;
    int i;
    
    if (g == NULL)
        return;

    DEBUG_PRINT("removeNode %d start\n", label);

    n = &g->nodeList[label - 1];//getNodeFromLabel(g, label);
    if (n == NULL)
        return;
    DEBUG_PRINT("numNeighbors of %d is: %d\n", n->label, n->numNeighbors);
    DEBUG_PRINT(";;;;;Existed nodes are: ");
    for (i = 0; i < g->numNode; i++)
        DEBUG_PRINT("%d ", g->existedNode[i]);
    DEBUG_PRINT("\n");
    for (i = 0; i < g->numNode; i++)
        if (label == g->existedNode[i]) {
            DEBUG_PRINT("Remove %d from existedNode\n", label);
            removeFromArray(g->existedNode, i, g->numNode);
            i--;
            g->numNode--;
        }
    n->numNeighbors = 0;
    if (n->neighbors != NULL) {
        free(n->neighbors);
        n->neighbors = NULL;
    }
    
    DEBUG_PRINT("removeNode end\n");
}

// tail will be replaced by head
void replaceNode(graph *g, int head, int tail)
{
    int i, j, label;
    
    if (g == NULL)
        return;
    DEBUG_PRINT("replaceNode %d with %d start\n", tail, head);
    
    for (i = 0; i < g->numNode; i++) {        // go through each node
        label = g->existedNode[i];
        DEBUG_PRINT("label is %d\n", label);
        if (g->nodeList[label - 1].label != tail)    // skip entry of the old node
            for (j = 0; j < g->nodeList[label - 1].numNeighbors; j++)   // each neighbor
                if (g->nodeList[label - 1].neighbors[j] == tail) // replace the old neighbor with the new neighbor
                    g->nodeList[label - 1].neighbors[j] = head;
    }
    
    for (i = 0; i < g->numNode; i++)
        printNeighbor(g, g->existedNode[i]);
    DEBUG_PRINT("replaceNode end\n");
}

// node1 has more or equal number of neighbors than node2
void contract(graph *g, int node1, int node2)
{
    DEBUG_PRINT("Contract %d and %d start\n", node1, node2);
    
    //printf("replaceNode start\n");
    // go through the list of neighbors of other nodes, replace node2 with node1
    replaceNode(g, node1, node2);
    //printf("replaceNode end\n");


    //printf("removeNeighbor %d from neighbor list of %d start\n", node2, node1);
    // remove the node2 from the list of neighbors of node1 --> 1 step of removing self loops
    handleHeadNeighbor(g, node1, node2);
    //printf("removeNeighbor end\n");
   
    //printf("removeNode %d start\n", node2);
    // set the list of neighbors of node2 to NULL --> search for neighbor is easier, and the way we traverse the list of nodes based on the node number is also easier. This step also remove self loops.
    removeNode(g, node2);
    //printf("removeNode end\n");
    DEBUG_PRINT("Contract %d and %d end\n", node1, node2);

}

int myRand(int limit)
{
    return rand() % limit;
}

void myPrng(void)
{
    time_t t;
    
    srand((unsigned)time(&t));  // initializes random number generator
}

node *getNodeFromLabel(graph *g, int label)
{
    printf("getNodeFromLabel %d start\n", label);
    if (g == NULL)
        return NULL;
    printf("getNodeFromLabel %d end\n", label);
    return &g->nodeList[label - 1];
    
}

void pickRandomEdge(graph *g, int *label1, int *label2)
{
    node *n1 = NULL;
    int i, j;
    
    DEBUG_PRINT("pickRandomEdge start\n");
    if (g == NULL || label1 == NULL || label2 == NULL)
        return;
    do {
        i = myRand(g->numNode);
        /*DEBUG_PRINT("random %d | ", i);
        for (j = 0; j < g->numNode; j++)
            printf("%d ", g->existedNode[j]);
        printf("\n");
         */
        n1 = &g->nodeList[g->existedNode[i] - 1];//getNodeFromLabel(g, g->existedNode[i]);
        //DEBUG_PRINT("Total neightbor of %d: %d\n", n1->label, n1->numNeighbors);
    } while(n1->numNeighbors <= 0);
    
    *label1 = n1->label;
    *label2 = n1->neighbors[myRand(n1->numNeighbors)];
    DEBUG_PRINT("Random edge %d to %d\n", *label1, *label2);
    printNeighbor(g, *label1);
    printNeighbor(g, *label2);
    DEBUG_PRINT("pickRandomEdge end\n");
}

void validateExistedNode(graph *g)
{
    int i;
    if (g == NULL)
        return;
    
    for (i = 0; i < g->numNode; i++)
        if (g->nodeList[g->existedNode[i] - 1].numNeighbors == 0) {
            DEBUG_PRINT("Node %d was removed but still existed in existedNode\n", g->existedNode[i]);
            assert(FALSE);
        }
}

void printNeighbor(graph *g, int label)
{
    int i;
    if (g == NULL)
        return;
    DEBUG_PRINT(";;;;; Node %d has %d neighbors | ", label, g->nodeList[label - 1].numNeighbors);
    for (i = 0; i < g->nodeList[label - 1].numNeighbors; i++)
        DEBUG_PRINT("%d ", g->nodeList[label - 1].neighbors[i]);
    DEBUG_PRINT("\n");
}
int randomContract(graph *g)
{
    int node1, node2;
    int i, j;
    
    if (g == NULL)
        return 0;
    
    while (g->numNode > 2) {
        DEBUG_PRINT("--------------g->numNode %d\n", g->numNode);
        pickRandomEdge(g, &node1, &node2);
        g->nodeList[node1 - 1].numNeighbors > g->nodeList[node2 - 1].numNeighbors ? contract(g, node1, node2) : contract(g, node2, node1);
        printNeighbor(g, 6);
        validateExistedNode(g);
        printGraph(g);
    }
    
    assert(g->numNode == 2);
    for (i = j = 0; j < 1; i++)
        if (g->nodeList[i].numNeighbors > 0) {
            j++;
            return g->nodeList[i].numNeighbors;
        }
    
    return 0;
}
