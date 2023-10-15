#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void print_points(long long int size, float** points) {
	//int size = sizeof(points)/sizeof(int);
	cout << "Datos: \n";

	for(long long int i = 0; i < size; i++)
		cout << "("  << points[i][0] << ", " << points[i][1] << "), Clasif: " << points[i][2] << "\n";
}

float** gen_data(long long int size) {
	const int def_size = 20;
	
	// Usa un tamaño default si size <=0
	if(size <= 0)
		size = def_size;
	
	float** points = new float*[size];
	
	// Llena el arreglo de puntos
	for(long long int i = 0; i < size; i++) {
		float r1 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)); //crea un float aleatorio entre 0 y 1
		float r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		points[i] = new float[3];
		points[i][0] = (int)(1000*r1)/1000.0; // trunca el float a 3 decimales
		points[i][1] = (int)(1000*r2)/1000.0;
		points[i][2] = 0.0;
		//cout << "("  << points[i][0] << ", " << points[i][1] << "), Clasif: " << points[i][2] << "\n";
	}
	
	return points;
}

void save_to_CSV(string file_name, float** points, long long int size) {
    fstream fout;
    fout.open(file_name, ios::out);
    
    for(long long int i = 0; i < size; i++) {
        fout << points[i][0] << ","
             << points[i][1] << ","
             << points[i][2] << "\n";
    }
}

int main(int argc, char *argv[]) {
	srand(time(NULL)); // cambia la semilla del rng
	long long int size = 20;
	float** points = gen_data(size);
	
	save_to_CSV("test_20.csv", points, size);
	print_points(size, points);
	
	for(long long int i = 0; i < size; i++)
        delete[] points[i];
    delete[] points;
	return 0;
}

