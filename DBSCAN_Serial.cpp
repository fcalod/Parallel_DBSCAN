#include <omp.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include "gen_data.cpp"

using namespace std;

void noise_detection(float** points, float epsilon, int min_samples, long long int size) {
    cout << "Step 0" << "\n"; 
    
    for (long long int i=0; i < size; i++) {
        points[i][2] = rand() % 2;
    }
    
    cout << "Complete" << "\n"; 
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
    
    /*
    long long int point_number = 0; 
    
    while (!in.eof() && (point_number < size)) {
        char* line = new char[12];
        streamsize row_size = 12;
        in.read(line, row_size);
        string row = line;
        
        points[point_number] = new float[3];
        points[point_number][0] = stof(row.substr(0, 5));
        points[point_number][1] = stof(row.substr(6, 5));
        cout << stof(row.substr(0, 5)) << " - " << stof(row.substr(6, 5)) << "\n";
        points[point_number][2] = 0;
        point_number++;
    }*/
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
    const long long int size = 20;
    const string input_file_name = to_string(size) + "_data.csv";
    const string output_file_name = to_string(size) + "_results.csv";    
    srand(time(NULL)); // cambia la semilla del rng
    //float** points = gen_data(size);
    float** points = new float*[size];
    load_CSV(input_file_name, points, size);
	
	/*
    for(long long int i = 0; i < size; i++) {
        points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }*/
	
	// carga los datos
    //load_CSV(input_file_name, points, size);
    print_data(size, points);
    // Clasifica
    //noise_detection(points, epsilon, min_samples, size); 
    // Guarda los resultados
    //save_to_CSV(output_file_name, points, size);
	
    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    return 0;
}

