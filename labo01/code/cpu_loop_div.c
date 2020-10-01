#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define n_sec 5

int main(void)
{

	time_t start_time = time(NULL);
	unsigned nb_iterations = 0;
	float a = 50;
	while (time(NULL) < (start_time + n_sec)) {
		//float c = a / b;
		a /= 2;
		++nb_iterations;
	}
	printf("nombre d iteration %d\n", nb_iterations); 
	return EXIT_SUCCESS;
}
