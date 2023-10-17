#include <omp.h>
//#include <bits/stdc++.h> 
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
    long long int i;
    float distance;
    
    # pragma omp parallel for collapse(1) default(shared) private(i, distance)
	for(i = 0; i < size; i++) {
	    distance = sqrt(pow(points[p][0] - points[i][0],2) + pow(points[p][1] - points[i][1],2));	
	    if (distance < epsilon)
	        vecinos.push_front(i);
	}
}

void noise_detection(float** points, float epsilon, int min_samples, long long int size) {
	int* cluster = new int[size]; 
	
	# pragma omp parallel for
	for(long long int i = 0; i < size; i++)
		cluster[i] = 0; // -1 -> Noise // 0 -> Unvisited // C - Visited, part of cluster C

	int c = 0; // cantidad de clusters
	list<int> vecinos;
	list<int> vecinos2;
	long long int i;

	//# pragma omp parallel for collapse(1) default(shared) private(vecinos,vecinos2,i)
	for( i=0; i < size; i++) {
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
		                }    
		            }
		        }
		    }    
		}
	}

    std::cout << c << " Clusters, -> " << "Complete" << "\n"; 
  
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
    const long long int size = 4000;
    const string input_file_name = "CSV/"+to_string(size)+"_data.csv";
    const string output_file_name = "CSV/"+to_string(size)+"_results.csv";
    clock_t start, end;
    omp_set_num_threads(8);
    srand(time(NULL)); // cambia la semilla del rng
    //float** points = gen_data(size);  
    float** points = new float*[size];
  
    /*
    for(long long int i = 0; i < size; i++) {
        points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }*/
	
	#pragma omp
	{
	  	// Carga los datos
		load_CSV(input_file_name, points, size);
		//print_data(size, points);
		// Clasifica
		double start = omp_get_wtime();
		
		noise_detection(points, epsilon, min_samples, size);
		
		double end = omp_get_wtime();
		// Guarda los resultados
		save_to_CSV(output_file_name, points, size);
		
		std::cout << "Time: " << fixed << (end-start) << " sec " << endl;
	}
	
	
    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    
    return 0;
}

