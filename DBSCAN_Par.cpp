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

int cont = 0;

void regionQuery(float** points, int p, float epsilon, long long int size, list<int> &vecinos) {	
    long long int i;
    float distance;
    float xi = points[p][0];
    float yi = points[p][1];
	cont = 0;
    # pragma omp for schedule(static) 
	for(i = 0; i < size; i++) {
	    distance = sqrt(pow(xi- points[i][0],2) + pow(yi- points[i][1],2));		
	    if (distance < epsilon)
	    	#pragma omp critical 
	    	{
	        	vecinos.push_front(i);				
	        }
	}
}
void noise_detection(float** points, float epsilon, int min_samples, long long int size) {
	int* cluster = new int[size]; 
    int c = 0; // cantidad de clusters
	list<int> vecinos;
	list<int> vecinos2;


	# pragma omp parallel for
	for(long long int j = 0; j < size; j++){
		cluster[j] = 0; // -1 -> Noise // 0 -> Unvisited // C - Visited, part of cluster C
	}

	long long int i=0;
	
	for( i=1; i < size; i++) {
		vecinos.clear();
		if(cluster[i]==0) 
		{
			#pragma omp parallel
			{
				regionQuery(points, i, epsilon, size, vecinos);
			}
			if(vecinos.size() < min_samples) {
					cluster[i] = -1;
					points[i][2] = 0;		
			}
			else
			{	
				c++;
				cluster[i] = c;
				points[i][2] = 1;

				int q;
				
					while(!vecinos.empty()) 
					{
						//cout<< omp_get_num_threads() << endl;
					
						q = vecinos.front();
						vecinos.pop_front();
							
						if(cluster[q]==0) 
						{
							if (cluster[q] <=0 ) 
							{
								cluster[q] = c;
								points[q][2] = 1;
							}
							#pragma omp parallel
							{			
								regionQuery(points,q,epsilon,size, vecinos2);
							}
							if(vecinos.size() >= min_samples) {
							// Union de Stacks
		
								vecinos.merge(vecinos2);
								vecinos.sort();
								vecinos.unique();
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
    const long long int size = 20000;
    const string input_file_name = "CSV/"+to_string(size)+"_data.csv";
    const string output_file_name = "CSV/"+to_string(size)+"_results.csv";
	const int number_threads = 32;
	omp_set_dynamic(0);
    omp_set_num_threads(number_threads);
  

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
	
    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    
    return 0;
}

