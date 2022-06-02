#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_VERTEX 10000
#define MAX_EDGE 50000000

typedef struct{
    int v1, v2;
    int weight; 
} graphNode;

graphNode** edges;                                              //saves all pointers to edges(min heap)
graphNode** tree;
int vertices[MAX_VERTEX];
int totalWeight = 0;
int branchcount = 0;                                            //counts established branches to break loop at n-1

void InsertToHeap(graphNode* node, int index);                  //insert to min heap
void HeapSort(FILE** fp, int size, int vertexSize);             //pop out edges in increasing order and call CreateTree
void Heapify(int size, int i);                                  //reestablish min heap
void swap(graphNode** a, graphNode** b);
void CreateTree(FILE** fp, graphNode* node);
int getParent(int vertex);                                      //returns root parent of node
int main(int argc, char *argv[])
{
    FILE* fp, *write;
    int vertexSize;
    int edgeSize;  
    int i;
    time_t start, finish;
    double duration;

    if (argc != 2){
        printf("usage: ./hw3 input_filename\n");
        return 0;
    }
    else if(!(fp = fopen(argv[1], "r"))){
        printf("The input file does not exist.\n");
        return 0;
    }
    else{
        start = clock();
        fscanf(fp, "%d", &vertexSize);
        fscanf(fp, "%d", &edgeSize);
        edges = (graphNode**)malloc(sizeof(graphNode*)*edgeSize);
        tree = (graphNode**)malloc(sizeof(graphNode*)*(vertexSize-1));
        for (i = 0; i < edgeSize; i++){
            graphNode* temp = (graphNode*)malloc(sizeof(graphNode));
            fscanf(fp, "%d %d %d", &temp->v1, &temp->v2, &temp->weight);
            InsertToHeap(temp, i);
        }
    }
    write = fopen("hw3_result.txt", "w");
    for (i = 0; i < vertexSize; i++){
        vertices[i] = i;                                       //initialize each vertex's parent to itself
    }
    HeapSort(&write, edgeSize, vertexSize);
    fprintf(write, "%d\n", totalWeight);
    if (branchcount == vertexSize - 1){
        fprintf(write, "CONNECTED\n");
    }
    else fprintf(write, "DISCONNECTED\n");
    finish = clock();
    duration = (double)(finish - start)/CLOCKS_PER_SEC;
    printf("output written to hw3_result.txt.\n");
    printf("running time : %f seconds\n", duration);
    for (i = 0; i < edgeSize; i++)
        free(edges[i]);
    free(edges);
    free(tree);
    fclose(fp);
    fclose(write);
    return 0;
}

void InsertToHeap(graphNode* node, int index){
    int parent;
    //int i = index;
    
    while (index > 0){
        parent = (index-1)/2;
        if (node->weight >= edges[parent]->weight){
            edges[index] = node;
            return;
        }
        else{
            edges[index] = edges[parent];
            index = parent;
        }
    }
    edges[0] = node;                                                    //if inserting for the first time;
}

void HeapSort(FILE** fp, int size, int vertexSize){
    int i;

    for (i = size-1; i >= 0; i--){
        if (branchcount == vertexSize-1) break;
        swap(&edges[0], &edges[i]);
        //edges[i] contains next smallest weight edge here
        CreateTree(fp, edges[i]);
        Heapify(i, 0);
    }
}

void Heapify(int size, int i){
    int left = i*2+1;
    int right = i*2+2;
    int max = i;

    if (left < size && edges[left]->weight < edges[max]->weight)
        max = left;
    if (right < size && edges[right]->weight < edges[max]->weight)
        max = right;
    if (max != i){
        swap(&edges[i], &edges[max]);
        Heapify(size, max);
    }   
}

void swap(graphNode** a, graphNode** b){
    graphNode* tmp = *a;
    *a = *b;
    *b = tmp;
}

void CreateTree(FILE** fp, graphNode* node){
    int v1, v2;
    
    v1 = getParent(node->v1);
    v2 = getParent(node->v2);
    if (v1 != v2){                                  //different parent means no cycles created
        fprintf(*fp, "%d %d %d\n", node->v1, node->v2, node->weight);
        totalWeight += node->weight;
        if (v1 < v2){                               //unify parents towards smaller indexed vertex
            vertices[v2] = v1;
        }
        else vertices[v1] = v2;
        branchcount++;
    }
}

int getParent(int vertex){
    for (; vertices[vertex] >= 0; vertex = vertices[vertex])
        if (vertices[vertex] == vertex) break;
    return vertex;
}