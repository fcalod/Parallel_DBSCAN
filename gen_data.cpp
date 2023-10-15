#include <cmath>

using namespace std;

void print_points(int num_points, int** points) {
	//int num_points = sizeof(points)/sizeof(int);
	cout << "Datos: \n";

	for(int i = 0; i < num_points; i++) {
		cout << "("  << points[i][0] << ", " << points[i][1] << ")\n";
	}
}

int main(int argc, char *argv[]) {
	int num_points = 20;
	//int** points = new int*[num_points];
	int** points = new int*[num_points];
	srand(time(NULL)); // cambia la semilla del rng
	
	for(int i = 0; i < num_points; i++) {
		points[i] = new int[2];
		points[i][0] = rand();
		points[i][1] = rand();
		//cout << "("  << points[i][0] << ", " << points[i][1] << ")\n";
	}
	
	//cout << rand() << "\n";
	print_points(num_points, points);
	
	return 0;
}

