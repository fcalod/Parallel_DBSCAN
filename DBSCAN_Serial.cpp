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
    float xi = points[p][0];
    float yi = points[p][1];
    float distance;

    for(long long int i = 0; i < size; i++) {
        distance = sqrt(pow(xi- points[i][0],2) + pow(yi- points[i][1],2));
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
		//getline(ss, val, ',');
		points[point_number][2] = 0; // 0 -> noise, 1 -> core

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
    //const long long int sizes[8] = {20000, 40000, 80000, 120000, 140000, 160000, 180000, 200000};
    const long long int size = 10000;
    const string input_file_name = "CSV/"+to_string(size)+"_data.csv";
    const string output_file_name = "CSV/"+to_string(size)+"_results1.csv";
    clock_t start, end;
    srand(time(NULL)); // cambia la semilla del rng
    float** points = new float*[size];
    
    // Carga los datos
	load_CSV(input_file_name, points, size);
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
