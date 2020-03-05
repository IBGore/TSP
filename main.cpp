#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include<bits/stdc++.h> 

using namespace std;

struct Point{
    int x;
    int y;
    int key = INT_MAX;
    int last = -1;
    bool visited = false;
};

int** matrix;
vector<Point> points;
int n;

void makeMatrix(){
    matrix = new int*[n];
    for(int a = 0; a < n; a++){
        matrix[a] = new int[n];

        for(int b = 0; b < n; b++){
            float x = pow(points.at(b).x - points.at(a).x, 2);
            float y = pow(points.at(b).y - points.at(a).y, 2);
            matrix[a][b] = (int) sqrt(x + y);
        }
    }
}

void prims(){
    points.at(0).key = 0;

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

int main(){
    ifstream infile("test-input-1.txt");
    int a, b, c;
    Point p;

    while(infile >> a >> b >> c){
        p.x = b;
        p.y = c;
        p.last = -1;
        points.push_back(p);
    }

    n = points.size();

    makeMatrix();
    prims();

}