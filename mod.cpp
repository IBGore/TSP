#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h> 

using namespace std;

struct Point{
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

int** matrix;
vector<Point> points;
int n;
int* TOP;
int topI;
int startPoint = 111;

void makeMatrix(){
    matrix = new int*[n];
    for(int a = 0; a < n; a++){
        matrix[a] = new int[n];

        for(int b = 0; b < n; b++){
            float x = pow(points.at(b).x - points.at(a).x, 2);
            float y = pow(points.at(b).y - points.at(a).y, 2);
            matrix[a][b] = (int) (sqrt(x + y) + 0.5);
        }
    }
}

void prims(){
    points.at(startPoint).key = 1;

    //for each point
    for(int a = 0; a < n; a++){
        int minKey = -1;
        int minIndex = -1;

        //find lowest key value
        for(int b = 0; b < n; b++){
            if(points.at(b).visited) continue;

            if(minKey == -1){
                minKey = points.at(b).key;
                minIndex = b; 
                continue;
            }
            if(points.at(b).key != INT_MAX && points.at(b).key < minKey){
                minKey = points.at(b).key;
                minIndex = b;
            }
        }

        points.at(minIndex).visited = true;
        int L = points.at(minIndex).last;
        if(L != -1){
            points.at(L).children.push_back(minIndex);
        }

        //update key values from new lowest key
        for(int c = 0; c < n; c++){
            int dist = matrix[minIndex][c];

            if(!points.at(c).visited && points.at(c).key > dist){
                points.at(c).key = dist;
                points.at(c).last = minIndex;
            }
        }
    }
}

void topSort(int index){
    for(int i = 0; i < points.at(index).children.size(); i++){
        topSort(points.at(index).children.at(i));
    }
    TOP[topI] = index;
    topI -= 1;
}

void addTopChild(int start, int next){
    Point* P = & points.at(start);
    Point* P2 = & points.at(next);
    for(int i = 0; i < P->children.size(); i++){
        if(P->children.at(i) == next){
            P->cons[P->nC] = next;
            P->nC += 1;
            P2->cons[P2->nC] = start;
            P2->nC += 1;

            int temp = P->end;
            points.at(P->end).end = next;
            P2->end = temp;
            return;
        }
    }
    P->hasTC = false;
}

void addClosest(int in){
    Point* P = & points.at(in);
    //exit(1);
    if(P->nC == 2){
        cout << "FULL" << endl;
        return;
    }
    int minD = INT_MAX;
    int minIndex = -1;
    for(int i = 0; i < n; i++){
        if(in == i || i == P->end || (P->nC == 1 && P->cons[0] == i)){
            continue;
        }
        assert(i != P->end);
        if(points.at(i).nC < 2 && matrix[in][i] < minD){
            minD = matrix[in][i];
            minIndex = i;
        }
    }
    if(minIndex == -1){
        P->cons[P->nC] = P->end;
        P->nC += 1;
        points.at(P->end).cons[points.at(P->end).nC] = in;
        points.at(P->end).nC += 1;

        return;

        // cout << P->end << endl;
        // cout << points.at(P->end).end << endl;
        // cout << "-----" << endl;
    }
    

    P->cons[P->nC] = minIndex; 
    P->nC += 1;
    Point* P2 = & points.at(minIndex);
    P2->cons[P2->nC] = in;
    P2->nC += 1;

    int e1 = P->end;
    int e2 = P2->end;
    points.at(P->end).end = e2;
    points.at(P2->end).end = e1;
}

int findClosest(int in){
    Point* P = & points.at(in);
    //exit(1);
    if(P->nC == 2){
        cout << "FULL" << endl;
        return INT_MAX;
    }
    int minD = INT_MAX;
    int minIndex = -1;
    for(int i = 0; i < n; i++){
        if(in == i || i == P->end || (P->nC == 1 && P->cons[0] == i)){
            continue;
        }
        assert(i != P->end);
        if(points.at(i).nC < 2 && matrix[in][i] < minD){
            minD = matrix[in][i];
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
    for(int i = n - 1; i >= 0; i--){
        if(points.at(i).nC < 2){
            Pair Pa;
            Pa.minD = findClosest(i);
            Pa.in = i;
            par.push_back(Pa);
        }
    }

    sort(par.begin(), par.end(), parSort);

    for(int i = 0; i < par.size(); i++){
        if(points.at(par.at(i).in).nC < 2) addClosest(par.at(i).in);
    }

    // for(int i = n - 1; i >= 0; i--){
    //     if(points.at(TOP[i]).nC < 2 && points.at(TOP[i]).hasTC == false){
    //         addClosest(TOP[i]);
    //     }
    // }
}

void printPath(){
    int sum = 0;
    int prev = startPoint;
    int cur = points.at(startPoint).cons[0];
    int next;
    int* path = new int[n];
    int pos = 0;

    for(int i = 1; i < n; i++){
        sum += matrix[prev][cur];
        //cout << cur << endl;
        path[pos] = cur;
        pos += 1;

        Point* P = & points.at(cur);
        
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
    sum += matrix[prev][startPoint];
    //cout << startPoint << endl;
    path[pos] = startPoint;
    pos += 1;
    cout << sum << endl;
    for(int i = 0; i < n; i++){
        cout << path[i] << endl;
    }
    delete[] path;
}

int main(){
    ifstream infile("example-3.txt");
    int a, b, c;
    Point p;

    while(infile >> a >> b >> c){
        p.x = b;
        p.y = c;
        p.last = -1;
        p.end = a;
        p.nC = 0;
        points.push_back(p);
    }

    n = points.size();

    TOP = new int[n];
    topI = n - 1;

    makeMatrix();
    

    prims();
    topSort(startPoint);
    for(int i = 0; i < n - 1; i++){
    addTopChild(TOP[i], TOP[i + 1]);
    }

    addTops();

    for(int i = 0; i < n; i++){
        //cout << i << endl;
        if(points.at(i).nC < 2) addClosest(i);
    }

    for(int i = 0; i < n; i++){
        if(points.at(i).nC < 2) cout << i << " " << points.at(i).nC << endl;
    }

    printPath();

    delete[] TOP;
    for(int i = 0; i < n; i++){
        delete[] matrix[i];
    }
    delete[] matrix;
    
}

