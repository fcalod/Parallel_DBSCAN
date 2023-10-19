#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include <stack>
#include <list>
#include <set>
#include <algorithm>
#include <time.h>
#include "gen_data.cpp"

using namespace std;

void regionQuery(float** points, int p, float epsilon, long long int size, list<int> &vecinos) {
    //vecinos.empty();
    for(long long int i = 0; i < size; i++) {
        float distance = sqrt(pow(points[p][0] - points[i][0],2) + pow(points[p][1] - points[i][1],2));
        if (distance < epsilon)
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

	for(long long int i=0; i < size; i++) {
	   //Si no hemos vistado el punto, hacer el proceso
	   vecinos.clear();
	   
	   if(cluster[i]==0) {
		   regionQuery(points, i, epsilon, size, vecinos);
		   
		   if(vecinos.size() < min_samples) {
		        cluster[i] = -1;
		        points[i][2] = 0;
		   } else {
		        c++;
		        cluster[i] = c;
		        points[i][2] = 1;
		        
		        while(!vecinos.empty()) {
		            int q = vecinos.front();
		            vecinos.pop_front();
		            
		            if(cluster[q]==0) {
		                if (cluster[q] <=0 ) {
		                    cluster[q] = c;
		                    points[q][2] = 1;
		                }
		                    
		                regionQuery(points,q,epsilon,size, vecinos2);

		                if(vecinos.size() >= min_samples) {
		                  // Union de Stacks
		                  vecinos.merge(vecinos2);
		                  vecinos2.clear();
		                  vecinos.sort();
		                  vecinos.unique();
		                }    
		            }
		        }
		    }    
		}
	}        

    cout << c << " Clusters, -> " << "Complete" << "\n"; 
  
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
    
    if (!in)
        cerr << "Couldn't read file: " << file_name << "\n";
    
    string line, val; //guardan lineas del archivo y valores dentro de las lineas
    long long int point_number = 0;
    
    // Lee el archivo una linea a la vez
    while(getline(in, line) && point_number < size) {
		stringstream ss(line);
		points[point_number] = new float[3];

		// Separa cada linea usando las comas
		getline(ss, val, ',');
		points[point_number][0] = stof(val);
		getline(ss, val, ',');
		points[point_number][1] = stof(val);
		getline(ss, val, ',');
		points[point_number][2] = stof(val);

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
    const long long int size = 8000;
    const string input_file_name = "CSV/"+to_string(size)+"_data.csv";
    const string output_file_name = "CSV/"+to_string(size)+"_results.csv";
    clock_t start, end;
    srand(time(NULL)); // cambia la semilla del rng
    float** points;
    
    ifstream in(input_file_name);
    
    // Si el archivo existe, lo carga
    if(in) {
    	points = new float*[size];
    	load_CSV(input_file_name, points, size);
    } else {
    	// Si no, genera datos nuevos y los guarda
    	points = gen_data(size);
    	save_to_CSV(input_file_name, points, size);
    }
  	
    //print_data(size, points);
    
    // Clasifica
    start = clock();
 	// unsync the I/O of C and C++. 
    noise_detection(points, epsilon, min_samples, size); 
    end = clock();
    // Guarda los resultados
    save_to_CSV(output_file_name, points, size);
	
	cout << "Time: " << fixed << double(end-start) / double(CLOCKS_PER_SEC) << " sec; size: " << size << endl;
	
    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    
    return 0;
}
