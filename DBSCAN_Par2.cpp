#include <omp.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>
#include <deque>
#include <map>
#include <time.h>

using namespace std;

void print_data(long long int size, float** points) {
	cout << "Datos: \n";
	
	for(long long int i = 0; i < size; i++)
		cout << "("  << points[i][0] << ", " << points[i][1] << "), Clasif: " << points[i][2] << "\n";
}

void print_arr(long long int size, int* arr) {
	cout << "================== \n";
	
	for(long long int i = 0; i < size; i++)
		cout << "i: " << i << ", " << arr[i] << "\n";
		
	cout << "================== \n";
}

void print_vec(vector<int> v) {
	cout << "================== vec \n";
	
	for(long long int i = 0; i < v.size(); i++)
		cout << "i: " << i << ", " << v.at(i) << "\n";
		
	cout << "================== \n";
}


void region_query(float** points, int p, float epsilon, long long int size, vector<int>* neighbors) {
    long long int i;
    float distance;
    float xp = points[p][0];
    float yp = points[p][1];
    
    // Encuentra todos los vecinos del punto con índice p
    # pragma omp parallel for
	for(i = 0; i < size; i++) {
		//cout << "Number cores " << omp_get_num_threads() << endl;
	    distance = sqrt(pow(xp - points[i][0], 2) + pow(yp - points[i][1], 2));
	    
	    if(distance < epsilon)
	    	# pragma omp critical
	    	{
	    	neighbors[p].push_back(i);
	    	}
	}
}

void noise_detection(float** points, vector<int>* neighbors, float epsilon, int min_samples, long long int size) {
	long long int i;
	int num_clusters = 0;
	//vector<int> clusters[size]; // guarda los clusters; no puede haber más de size/min_samples
	int* core = new int[size]; // guarda qué puntos son core
	map<int, int> visited; // mappea indices de puntos a: 0 -> no visitado, 1 -> visitado
	deque<int> explore; // explora los vecinos de puntos core
	
	//# pragma omp parallel shared(neighbors, num_clusters, core, visited, explore) private(i)
	{
		//# pragma omp for
		for(i = 0; i < size; i++) {
			visited[i] = 0;
			// Encuentra todos los vecinos del punto i
			region_query(points, i, epsilon, size, neighbors);
			
			// Clasifica el punto como core o non-core
			if(neighbors[i].size() >= min_samples)
				core[i] = 1; // 1 -> Core point // 0 -> Non-core point
			else
				core[i] = 0;
		}
		
		//# pragma omp barrier
		// Encuentra los clusters
		//# pragma omp parallel for 
	}
		//# pragma omp single
		{
		for(i = 0; i < size; i++) {
			// Si no se ha visitado el punto i, es parte de un nuevo cluster o es ruido
			if(!visited[i] && core[i] == 1) {
				explore.push_back(i);
				//clusters[num_clusters].push_back(i);
				
				//# pragma omp critical
				{
				// Explora los vecinos del punto core i y expande el cluster hasta terminarlo
				while(explore.size() > 0) {
					int p = explore.front();
					explore.pop_front();
					
					for(long long int k = 0; k < neighbors[p].size(); k++) {
						int neighbor = neighbors[p].at(k);
					
						if(!visited[neighbor]) {
							visited[neighbor] = 1;
							//clusters[num_clusters].push_back(neighbor);
							points[neighbor][2] = 1; // Clustered point
							
							// Si es core, se agrega al queue
							if(core[neighbor] == 1)
								explore.push_back(neighbor);
						}
					}
				}
				
				num_clusters++;
				}
			}
		}
		}
		
	//}
	
    cout << num_clusters << " Clusters, -> " << "Complete" << "\n"; 
    delete[] core;
}

void load_CSV(string file_name, float** points, long long int size) {
    ifstream in(file_name);
    
    if(!in)
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
    const long long int size = 120000;
    const string input_file_name = "CSV/"+to_string(size)+"_data.csv";
    const string output_file_name = "CSV/"+to_string(size)+"_results_par2.csv";
    omp_set_num_threads(4);
    srand(time(NULL)); // cambia la semilla del rng
    float** points = new float*[size];
    vector<int> neighbors[size]; // arreglo de listas de vecinos
    
  	// Carga los datos
	load_CSV(input_file_name, points, size);
	// Clasifica
	double start = omp_get_wtime();
	noise_detection(points, neighbors, epsilon, min_samples, size);
	double end = omp_get_wtime();
	// Guarda los resultados
	save_to_CSV(output_file_name, points, size);
	
	cout << "Time: " << fixed << (end-start) << " sec; size: " << size << endl;
	
    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    return 0;
}
