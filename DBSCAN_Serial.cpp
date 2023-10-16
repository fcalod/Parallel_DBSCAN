#include <omp.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <stack>

#include <list>

#include <set>
#include <algorithm>

using namespace std;


void regionQuery(float** points, int p, float epsilon, long long int size, list<int> &vecinos){
    //vecinos.empty();
    for(long long int i = 0; i < size; i++){
        float distance = sqrt(pow(points[p][0] - points[i][0],2) + pow(points[p][1] - points[i][1],2));
        if ( distance < epsilon )
            vecinos.push_front(i);
    }
}

void noise_detection(float** points, float epsilon, int min_samples, long long int size) {

   int* cluster = new int[size]; 
   for(long long int i = 0; i < size; i++)
        cluster[i] = 0; // -1 -> Noise // 0 -> Unvisited // C - Visited, part of cluster C
   int c = 0;
   list<int> vecinos;
   list<int> vecinos2;

   for (long long int i=0; i < size; i++) {
       //Si no hemos vistado el punto, hacer el proceso
       vecinos.clear();
       if(cluster[i]==0){
           regionQuery(points,i,epsilon,size, vecinos);
           if (vecinos.size()<min_samples){
                cluster[i] = -1;
                points[i][2] = 0;
           }         
           else
           {
                c++;
                cluster[i] = c;
                points[i][2] = 1;
                while (!vecinos.empty())
                {
                    int q = vecinos.front();
                    vecinos.pop_front();
                    if(cluster[q]==0){
                        if (cluster[q] <=0 ){
                            cluster[q] = c;
                            points[q][2] = 1;
                        }
                            
                        regionQuery(points,q,epsilon,size, vecinos2);
                        if (vecinos.size()>=min_samples){
                        // Union de Stacks
                        vecinos.merge(vecinos2);
                        vecinos2.clear();
                        }    
                    }
                }
            }    
        }
    }        
    
    cout << c << "Clusters, ->" << "Complete" << "\n"; 
  
    delete[] cluster;

}


   /*
   DBSCAN(D, eps, MinPts)
   C = 0
   for each unvisited point P in dataset D  // for
      mark P as visited // 
      NeighborPts = regionQuery(P, eps)
      if sizeof(NeighborPts) < MinPts
         mark P as NOISE
      else
         C = next cluster
         expandCluster(P, NeighborPts, C, eps, MinPts)

expandCluster(P, NeighborPts, C, eps, MinPts)
   add P to cluster C
   for each point P' in NeighborPts
      if P' is not visited
         mark P' as visited
         NeighborPts' = regionQuery(P', eps)
         if sizeof(NeighborPts') >= MinPts
            NeighborPts = NeighborPts joined with NeighborPts'
      if P' is not yet member of any cluster
         add P' to cluster C

regionQuery(P, eps)
   return all points within P's eps-neighborhood (including P)
   
   
   */

 
void load_CSV(string file_name, float** points, long long int size) {
    ifstream in(file_name);
    if (!in) {
        cerr << "Couldn't read file: " << file_name << "\n";
    }
    long long int point_number = 0; 
    while (!in.eof() && (point_number < size)) {
        char* line = new char[12];
        streamsize row_size = 12;
        in.read(line, row_size);
        string row = line;
        //cout << stof(row.substr(0, 5)) << " - " << stof(row.substr(6, 5)) << "\n";
        points[point_number][0] = stof(row.substr(0, 5));
        points[point_number][1] = stof(row.substr(6, 5));
        point_number++;
    }
}

void save_to_CSV(string file_name, float** points, long long int size) {
    fstream fout;
    fout.open(file_name, ios::out);
    for (long long int i = 0; i < size; i++) {
        fout << points[i][0] << ","
             << points[i][1] << ","
             << points[i][2] << "\n";
    }
}

int main(int argc, char** argv) {

    const float epsilon = 0.03;
    const int min_samples = 10;
    const long long int size = 4000;
    const string input_file_name = "CSV/"+to_string(size)+"_data.csv";
    const string output_file_name = "CSV/"+to_string(size)+"_results.csv";    
    float** points = new float*[size];


    for(long long int i = 0; i < size; i++) {
        points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }

    load_CSV(input_file_name, points, size);
    
    noise_detection(points, epsilon, min_samples, size); 
        
    save_to_CSV(output_file_name, points, size);

    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    
    return 0;
}
