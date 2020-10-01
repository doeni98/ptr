#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define n_sec 5

int main(void)
{

	time_t start_time = time(NULL);
	unsigned long nb_iterations = 0;
	while (time(NULL) < (start_time + n_sec)) {
		++nb_iterations;
	}
	printf("%lu cycles/s %d\n", nb_iterations/n_sec, getpid());
	return EXIT_SUCCESS;
}
