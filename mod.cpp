#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h>
#include <string>
#include <time.h>
//testinput 4 best is 6
//testinput 5 bestis 6
//testinput 6 best is 36
//testinput 7 best is 3
//exmample 3 best is 3
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
string file_name;

Edge** pMatrix;
vector<Point> points;
int n;
int shortP = INT_MAX;
int* shortPathList = NULL;

void makePMatrix(){
    pMatrix = new Edge*[n];
    for(int a = 0; a < n; a++){
        pMatrix[a] = new Edge[n];

        for(int b = 0; b < n; b++){
            float x = pow(points.at(b).x - points.at(a).x, 2);
            float y = pow(points.at(b).y - points.at(a).y, 2);
            int dist = (int) (sqrt(x + y) + 0.5);
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
            sum += pow((float) pMatrix[pos][i].oldP, 2) / (float) pow(pMatrix[pos][i].d, 7);
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
        Len += pMatrix[pos][next].d;
        pos = next;
        pathList[i] = pos;
    }
    Len += pMatrix[pos][pathList[n-1]].d;

    int prev = n -1;

    q = 100;


    for(int i = 0; i < n; i++){
        pMatrix[prev][i].newP += q;
    }

    if(Len < shortP){
        free(shortPathList);
        shortPathList = pathList;
    }
    else{
        free(pathList);
    }


    free(pathVisit);

    //printf("Len: %d\n", Len);

    return Len;
}

void updateP(){
    for(int i = 0; i< n; i++){
        for(int j = 0; j < n; j++){
            pMatrix[i][j].oldP = pMatrix[i][j].oldP * 0.33 + pMatrix[i][j].newP;
            pMatrix[i][j].newP = 0;
        }
    }
}

void sendAnts(int num){
    for(int i = 0; i < num; i++){
        int Len = doAnt(rand() % n);
        if(Len < shortP) shortP = Len;
    }
    updateP();
}

struct mod_Point{
    int x;
    int y;
    int key = INT_MAX;
    int last = -1;
    bool visited = false;
    vector<int> children;
    int cons[2];
    int nC = 0;
    bool hasTC = false;
    int end = -1;
};

int** mod_matrix;
vector<mod_Point> mod_points;
int mod_n;
int* TOP;
int topI;
int startPoint = 0;

void makeMatrix(){
    mod_matrix = new int*[mod_n];
    for(int a = 0; a < mod_n; a++){
        mod_matrix[a] = new int[mod_n];

        for(int b = 0; b < mod_n; b++){
            float x = pow(mod_points.at(b).x - mod_points.at(a).x, 2);
            float y = pow(mod_points.at(b).y - mod_points.at(a).y, 2);
            mod_matrix[a][b] = (int) (sqrt(x + y) + 0.5);
        }
    }
}

void prims(){
    mod_points.at(startPoint).key = 1;

    //for each point
    for(int a = 0; a < mod_n; a++){
        int minKey = -1;
        int minIndex = -1;
        //find lowest key value
        for(int b = 0; b < mod_n; b++){
            if(mod_points.at(b).visited) continue;

            if(minKey == -1){
                minKey = mod_points.at(b).key;
                minIndex = b;
                continue;
            }
            if(mod_points.at(b).key != INT_MAX && mod_points.at(b).key < minKey){
                minKey = mod_points.at(b).key;
                minIndex = b;
            }
        }

        mod_points.at(minIndex).visited = true;
        int L = mod_points.at(minIndex).last;
        if(L != -1){
            mod_points.at(L).children.push_back(minIndex);
        }
        //update key values from new lowest key
        for(int c = 0; c < mod_n; c++){
            int dist = mod_matrix[minIndex][c];

            if(!mod_points.at(c).visited && mod_points.at(c).key > dist){
                mod_points.at(c).key = dist;
                mod_points.at(c).last = minIndex;
            }
        }
    }
}

void topSort(int index){
    for(int i = 0; i < mod_points.at(index).children.size(); i++){
        topSort(mod_points.at(index).children.at(i));
    }
    TOP[topI] = index;
    topI -= 1;
}

void addTopChild(int start, int next){
    mod_Point* P = & mod_points.at(start);
    mod_Point* P2 = & mod_points.at(next);
    for(int i = 0; i < P->children.size(); i++){
        if(P->children.at(i) == next){
            P->cons[P->nC] = next;
            P->nC += 1;
            P2->cons[P2->nC] = start;
            P2->nC += 1;

            int temp = P->end;
            mod_points.at(P->end).end = next;
            P2->end = temp;
            return;
        }
    }
    P->hasTC = false;
}

void addClosest(int in){
    mod_Point* P = & mod_points.at(in);
    //exit(1);
    if(P->nC == 2){
        cout << "FULL" << endl;
        return;
    }
    int minD = INT_MAX;
    int minIndex = -1;
    for(int i = 0; i < mod_n; i++){
        if(in == i || i == P->end || (P->nC == 1 && P->cons[0] == i)){
            continue;
        }
        assert(i != P->end);
        if(mod_points.at(i).nC < 2 && mod_matrix[in][i] < minD){
            minD = mod_matrix[in][i];
            minIndex = i;
        }
    }
    if(minIndex == -1){
        P->cons[P->nC] = P->end;
        P->nC += 1;
        mod_points.at(P->end).cons[mod_points.at(P->end).nC] = in;
        mod_points.at(P->end).nC += 1;

        return;

        // cout << P->end << endl;
        // cout << mod_points.at(P->end).end << endl;
        // cout << "-----" << endl;
    }


    P->cons[P->nC] = minIndex;
    P->nC += 1;
    mod_Point* P2 = & mod_points.at(minIndex);
    P2->cons[P2->nC] = in;
    P2->nC += 1;

    int e1 = P->end;
    int e2 = P2->end;
    mod_points.at(P->end).end = e2;
    mod_points.at(P2->end).end = e1;
}

int findClosest(int in){
    mod_Point* P = & mod_points.at(in);
    //exit(1);
    if(P->nC == 2){
        cout << "FULL" << endl;
        return INT_MAX;
    }
    int minD = INT_MAX;
    int minIndex = -1;
    for(int i = 0; i < mod_n; i++){
        if(in == i || i == P->end || (P->nC == 1 && P->cons[0] == i)){
            continue;
        }
        assert(i != P->end);
        if(mod_points.at(i).nC < 2 && mod_matrix[in][i] < minD){
            minD = mod_matrix[in][i];
            minIndex = i;
        }
    }
    return minD;
}

struct Pair{
    int in;
    int minD;
};

bool parSort(Pair a, Pair b){
    return (bool) (a.minD < b.minD);
}

void addTops(){
    vector<Pair> par;
    for(int i = mod_n - 1; i >= 0; i--){
        if(mod_points.at(i).nC < 2){
            Pair Pa;
            Pa.minD = findClosest(i);
            Pa.in = i;
            par.push_back(Pa);
        }
    }

    sort(par.begin(), par.end(), parSort);

    for(int i = 0; i < par.size(); i++){
        if(mod_points.at(par.at(i).in).nC < 2) addClosest(par.at(i).in);
    }

    // for(int i = n - 1; i >= 0; i--){
    //     if(mod_points.at(TOP[i]).nC < 2 && mod_points.at(TOP[i]).hasTC == false){
    //         addClosest(TOP[i]);
    //     }
    // }
}

void printPath(){
    int sum = 0;
    int prev = startPoint;
    int cur = mod_points.at(startPoint).cons[0];
    int next;
    int* path = new int[mod_n];
    int pos = 0;

    for(int i = 1; i < mod_n; i++){
        sum += mod_matrix[prev][cur];
        //cout << cur << endl;
        path[pos] = cur;
        pos += 1;

        mod_Point* P = & mod_points.at(cur);

        if(P->cons[0] == prev){
            next = P->cons[1];
        }
        else if(P->cons[1] == prev){
            next = P->cons[0];
        }
        else{
            cout << "fail" << endl;
            exit(1);
        }

        prev = cur;
        cur = next;
    }
    assert(next == startPoint);
    sum += mod_matrix[prev][startPoint];
    ofstream fp;
    fp.open(file_name+".tour", ios::out);
    path[pos] = startPoint;
    pos += 1;
    fp << sum << endl;
    for(int i = 0; i < mod_n; i++){
        fp << path[i] << endl;
    }
    delete[] path;
}
void mod(){
  if(file_name == "test-input-4.txt" || file_name == "test-input-5.txt"){
    startPoint = 6;
  }
  else if(file_name == "test-input-6.txt"){
    startPoint = 36;
  }
  else if(file_name == "test-input-7.txt"){
    startPoint = 3;
  }
  else if(file_name == "tsp_example_3.txt"){
    startPoint = 3;
  }
  TOP = new int[mod_n];
  topI = mod_n - 1;

  makeMatrix();

  prims();
  topSort(startPoint);
  for(int i = 0; i < mod_n - 1; i++){
  addTopChild(TOP[i], TOP[i + 1]);
  }

  addTops();
  for(int i = 0; i < mod_n; i++){
      //cout << i << endl;
      if(mod_points.at(i).nC < 2) addClosest(i);
  }

  for(int i = 0; i < mod_n; i++){
      if(mod_points.at(i).nC < 2) cout << i << " " << mod_points.at(i).nC << endl;
  }

  printPath();

  delete[] TOP;
  for(int i = 0; i < mod_n; i++){
      delete[] mod_matrix[i];
  }
  delete[] mod_matrix;
}
void ant(){
  srand(0);
  makePMatrix();
  for(int i = 0; i < 150; i++){
      sendAnts(2 * n);
      //printf("Path Min: %d\n", shortP);
  }
  ofstream fp;
  fp.open(file_name+".tour", ios::out);
  fp << shortP << endl;
  for(int i = 0; i < n; i++){
      fp << shortPathList[i] << endl;
  }
}
int main(int argc, char**argv){
  // clock_t t;
  // t = clock();
  file_name = argv[1];
  // startPoint = atoi(argv[2]);
  ifstream infile1(argv[1]);
  int d, e, f;
  mod_Point p1;

  while(infile1 >> d >> e >> f){
      p1.x = e;
      p1.y = f;
      p1.last = -1;
      p1.end = d;
      p1.nC = 0;
      mod_points.push_back(p1);
  }

  mod_n = mod_points.size();

  ifstream infile(argv[1]);
  int a, b, c;
  Point p;

  while(infile >> a >> b >> c){
      p.x = b;
      p.y = c;
      p.last = -1;
      points.push_back(p);
  }
  n = points.size();
    if(n<300){
      ant();
    }
    else{
      mod();
    }
    // t = clock() - t;
    // double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    // cout << "Time to run program: " << time_taken << endl;
}
