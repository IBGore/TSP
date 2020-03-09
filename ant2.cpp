#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h> 
#include <thread>
#include <mutex> 

#define numAnts 100

using namespace std;

struct Point{
    int x;
    int y;
    int key = INT_MAX;
    int last = -1;
    bool visited = false;
};

struct Edge{
    int d;
    float oldP;
    float newP;
};

Edge** pMatrix;
vector<Point> points;
int n;
int shortP = 10000000;
mutex mtx;


void makePMatrix(){
    pMatrix = new Edge*[n];
    for(int a = 0; a < n; a++){
        pMatrix[a] = new Edge[n];

        for(int b = 0; b < n; b++){
            float x = pow(points.at(b).x - points.at(a).x, 2);
            float y = pow(points.at(b).y - points.at(a).y, 2);
            int dist = (int) sqrt(x + y);
            pMatrix[a][b].d = dist;
            pMatrix[a][b].oldP = 10.0; //default starting pharamone value
            pMatrix[a][b].newP = 0;
        }
    }
}

int getNext(int pos, bool* vE){
    float sum = 0;
    float* arr = (float*) malloc(n * sizeof(float));
    float rnd;
    int i;

    for(i = 0; i < n; i++){
        if(vE[i] == false){
            assert(pos < n);
            assert(i != pos);
            sum += (float) pMatrix[pos][i].oldP / (float) pow(pMatrix[pos][i].d, 4);
        }
        
        arr[i] = sum;
    }
    rnd = (float) rand() / (float) (RAND_MAX) * sum;
    i = 0;
    while(vE[i] | arr[i] < rnd){
        i++;
    }

    assert(pos != i);
    free(arr);
    return i;
}

void updateLen(int Len){
    if(Len < shortP) shortP = Len;
    mtx.unlock();
}

int doAnt(int start){
    int pos = start;
    int Len = 0;
    int count;
    int* pathList = (int*) calloc(n, sizeof(int));
    bool* pathVisit = (bool*) calloc(n, sizeof(bool));
    float q;

    pathList[n-1] = pos;
    for(int i = 0; i < n - 1; i++){
        pathVisit[pos] = true;
        int next = getNext(pos, pathVisit);
        if(pos == next){
            perror("failed logic");
            exit(1);
        }
        pathList[i] = pos;
        Len += pMatrix[pos][next].d;
        pos = next;
    }
    Len += pMatrix[pos][pathList[n-1]].d;

    int prev = n -1;

    q = 100 / Len;

    
    for(int i = 0; i < n; i++){
        pMatrix[prev][i].newP += q;
    }

    free(pathList);
    free(pathVisit);

    //printf("Len: %d\n", Len);
    mtx.lock();
    updateLen(Len);
    return Len;
}

void updateP(){
    for(int i = 0; i< n; i++){
        for(int j = 0; j < n; j++){
            pMatrix[i][j].oldP = pMatrix[i][j].oldP * 0.75 + pMatrix[i][j].newP;
            pMatrix[i][j].newP = 0;
        }
    }
}

void sendAnts(int num, int blocks){
    thread* threads = new thread[num];

    for(int b = 0; b < blocks; b++){
        for(int i = 0; i < num; i++){
        threads[i] = thread(doAnt, rand() % n);
        }

        for(int i = 0; i < num; i++){
            threads[i].join();
        }
    }
    
    delete[] threads;
    updateP();
}

int main(){
    ifstream infile("test-input-3.txt");
    int a, b, c;
    Point p;

    while(infile >> a >> b >> c){
        p.x = b;
        p.y = c;
        p.last = -1;
        points.push_back(p);
    }

    n = points.size();
    makePMatrix();
    for(int i = 0; i < 100; i++){
        sendAnts(32, 15);
        printf("Path Min: %d\n", shortP);
    }
    
}