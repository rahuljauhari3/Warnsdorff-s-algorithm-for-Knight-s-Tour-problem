#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

typedef struct
{
	int x, y;
} pair;

typedef struct
{
	int x, y, degree;
} argument;

int N;
int x_modifier[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
int y_modifier[8] = {1, 2, 2, 1, -1, -2, -2, -1};
int degree_of_rotation[4] = {0, 90, 180, 270};
int total_squares;
int solved_status = 0, print_status = 0;
int whoSolved = -1;
int throughput[4] = {0};
int thread_count = 0;
int debugCheck = 1;
int is_multithreaded = 1; // 1 for multi-threaded mode, 0 for single-threaded mode
int enable_timeout = 0;   // set to 1 to enable snorlax timeout thread in multi-threaded mode

// Improved barrier synchronization using pthread condition variables
pthread_mutex_t barrier_mutex;
pthread_cond_t barrier_cond;
int barrier_count = 0;
int barrier_threshold = 4;

pthread_mutex_t solutionLock; // This mutex lock is for making sure that if multiple threads find the solution at the same time, then only one thread sets the solved_status as 1 and outputs the path.
pthread_mutex_t throughput_mutex; // Protect throughput array from race conditions

int validate(int x, int y, int visited[N][N])
{
	return (x >= 0 && x < N && y >= 0 && y < N && visited[x][y] == -1);
}

int find_number(int x, int y, int visited[N][N])
{
	int neighbour_count = 0;
	for (int i = 0; i < 8; i++)
	{
		neighbour_count += validate(x + x_modifier[i], y + y_modifier[i], visited);
	}
	return neighbour_count;
}

pair rotate_anticlockwise(int x, int y, int degree)
{
	pair rotated_coords;

	if (degree == 0 || degree == 360)
	{
		rotated_coords.x = x;
		rotated_coords.y = y;
	}
	else if (degree == 90)
	{
		rotated_coords.x = N - y - 1;
		rotated_coords.y = x;
	}
	else if (degree == 180)
	{
		rotated_coords.x = N - x - 1;
		rotated_coords.y = N - y - 1;
	}
	else
	{
		rotated_coords.x = y;
		rotated_coords.y = N - x - 1;
	}

	return rotated_coords;
}

void print_rotated_path(pair path[], int n, int degree)
{
	pair rotated_coords;
	for (int i = 0; i < n; i++)
	{
		rotated_coords = rotate_anticlockwise(path[i].x, path[i].y, 360 - degree);
		printf("%d,%d|", rotated_coords.x, rotated_coords.y);
	}
}

int Solve(int x, int y, int count, int degree, int visited[N][N], pair path[])
{
	if (solved_status == 1)
		return 1;

	// Improved barrier synchronization with early termination check
	if (is_multithreaded && N >= 15 && throughput[degree / 90] == total_squares / 2)
	{
		pthread_mutex_lock(&barrier_mutex);
		barrier_count++;
		
		if (debugCheck)
			printf("Thread #%d reached barrier (count: %d)\n", degree / 90, barrier_count);
		
		if (barrier_count == barrier_threshold)
		{
			if (debugCheck)
				printf("All threads reached barrier - releasing all\n");
			pthread_cond_broadcast(&barrier_cond);
		}
		else
		{
			// Wait for all threads to reach barrier
			while (barrier_count < barrier_threshold && solved_status == 0)
			{
				pthread_cond_wait(&barrier_cond, &barrier_mutex);
			}
		}
		pthread_mutex_unlock(&barrier_mutex);
		
		// Check if solution was found while waiting
		if (solved_status == 1)
			return 1;
	}

	int mi = 8, length = 0;
	int *xcoordinate = (int *)malloc(sizeof(int));
	int *ycoordinate = (int *)malloc(sizeof(int));

	visited[x][y] = 0;
	
	// Thread-safe throughput increment
	pthread_mutex_lock(&throughput_mutex);
	throughput[(degree / 90)]++;
	pthread_mutex_unlock(&throughput_mutex);
	
	count++;

	if (count == total_squares)
	{
		pthread_mutex_lock(&solutionLock);
		if (solved_status != 1)
		{
			solved_status = 1;
			print_status = 1;
			print_rotated_path(path, total_squares, degree);
			whoSolved = degree / 90;
		}
		pthread_mutex_unlock(&solutionLock);
		return 1;
	}

	for (int k = 0; k < 8; k++)
	{
		int new_x = x + x_modifier[k];
		int new_y = y + y_modifier[k];
		if (validate(new_x, new_y, visited))
		{
			int a = find_number(new_x, new_y, visited);
			if (a == 0 && count != total_squares - 1)
				continue;
			if (a < mi)
			{
				mi = a;
				length = 0;
				free(xcoordinate);
				free(ycoordinate);
				xcoordinate = (int *)malloc(sizeof(int));
				ycoordinate = (int *)malloc(sizeof(int));
				xcoordinate[length] = new_x;
				ycoordinate[length] = new_y;
				length++;
			}
			else if (a == mi)
			{
				xcoordinate = (int *)realloc(xcoordinate, (length + 1) * sizeof(int));
				ycoordinate = (int *)realloc(ycoordinate, (length + 1) * sizeof(int));
				xcoordinate[length] = new_x;
				ycoordinate[length] = new_y;
				length++;
			}
		}
	}

	if (length == 0)
	{
		visited[x][y] = -1;
		return 0;
	}

	for (int k = 0; k < length; k++)
	{
		// Check for early termination before each recursive call
		if (solved_status == 1)
		{
			free(xcoordinate);
			free(ycoordinate);
			return 1;
		}
		
		path[count].x = xcoordinate[k];
		path[count].y = ycoordinate[k];
		if (Solve(xcoordinate[k], ycoordinate[k], count, degree, visited, path) == 1)
		{
			free(xcoordinate);
			free(ycoordinate);
			return 1;
		}
	}

	free(xcoordinate);
	free(ycoordinate);

	visited[x][y] = -1;
	return 0;
}

void *begin_tour(void *args)
{
	int visited[N][N];
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			visited[i][j] = -1;
		}
	}

	int temp_degree = ((argument *)args)->degree;
	pair rotated_coords = rotate_anticlockwise(((argument *)args)->x, ((argument *)args)->y, temp_degree);

	pair path[total_squares];
	path[0].x = rotated_coords.x;
	path[0].y = rotated_coords.y;

	int *ret_value = (int *)malloc(sizeof(int));

	if (Solve(rotated_coords.x, rotated_coords.y, 0, temp_degree, visited, path) == 0)
		ret_value[0] = 0;
	else
		ret_value[0] = 1;
	return (void *)ret_value;
}

void *snorlax(void *arg)
{
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 100000000;

	nanosleep(&ts, NULL);
	
	pthread_mutex_lock(&solutionLock);
	if (solved_status == 0) {
		solved_status = 1;
		if (debugCheck)
			printf("Timeout reached - forcing termination\n");
	}
	pthread_mutex_unlock(&solutionLock);
	
	return NULL;
}

static void reset_globals()
{
	solved_status = 0;
	print_status = 0;
	whoSolved = -1;
	barrier_count = 0;
	for (int i = 0; i < 4; ++i)
		throughput[i] = 0;
}

static void print_time_label(const char *label, struct timeval tv)
{
	char buf[64];
	struct tm *tm_info = localtime(&tv.tv_sec);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
	printf("%s: %s.%06ld\n", label, buf, (long)tv.tv_usec);
}

static double elapsed_ms(struct timeval start, struct timeval end)
{
	long sec = end.tv_sec - start.tv_sec;
	long usec = end.tv_usec - start.tv_usec;
	return (double)sec * 1000.0 + (double)usec / 1000.0;
}

static int run_multi(int StartX, int StartY)
{
	reset_globals();
	is_multithreaded = 1;

	argument args[4];
	pthread_t tid[4];
	pthread_t tkiller;

	// Initialize synchronization objects
	pthread_mutex_init(&barrier_mutex, NULL);
	pthread_cond_init(&barrier_cond, NULL);
	pthread_mutex_init(&solutionLock, NULL);
	pthread_mutex_init(&throughput_mutex, NULL);

	if (enable_timeout)
		pthread_create(&tkiller, NULL, snorlax, NULL);

	for (int i = 0; i < 4; i++)
	{
		args[i].x = StartX;
		args[i].y = StartY;
		args[i].degree = degree_of_rotation[i];
		pthread_create(&tid[i], NULL, begin_tour, (void *)&args[i]);
	}

	int flag = 0;

	for (int i = 0; i < 4; i++)
	{
		void *t;
		pthread_join(tid[i], &t);
		flag += *((int *)t);
		free(t);
	}

	// Clean up timeout thread if it was created
	if (enable_timeout)
	{
		pthread_cancel(tkiller);
		pthread_join(tkiller, NULL);
	}

	if (flag == 0 || print_status == 0)
		printf("No Possible Tour");
	else if (debugCheck)
		printf("\nSolved by thread #%d. Throughput: (%d, %d, %d, %d).\n", whoSolved, throughput[0], throughput[1], throughput[2], throughput[3]);

	// Clean up synchronization objects
	pthread_mutex_destroy(&barrier_mutex);
	pthread_cond_destroy(&barrier_cond);
	pthread_mutex_destroy(&solutionLock);
	pthread_mutex_destroy(&throughput_mutex);

	return (flag != 0 && print_status != 0);
}

static int run_single(int StartX, int StartY)
{
	reset_globals();
	is_multithreaded = 0;

	pthread_mutex_init(&solutionLock, NULL);
	int solved = 0;

	for (int idx = 0; idx < 4 && !solved; ++idx)
	{
		int temp_degree = degree_of_rotation[idx];
		int visited[N][N];
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				visited[i][j] = -1;

		pair rotated_coords = rotate_anticlockwise(StartX, StartY, temp_degree);
		pair path[total_squares];
		path[0].x = rotated_coords.x;
		path[0].y = rotated_coords.y;

		int ret = Solve(rotated_coords.x, rotated_coords.y, 0, temp_degree, visited, path);
		solved = (ret == 1 && print_status == 1);
	}

	if (!solved)
		printf("No Possible Tour");
	else if (debugCheck)
		printf("\nSolved in single-threaded mode. Throughput: (%d, %d, %d, %d).\n", throughput[0], throughput[1], throughput[2], throughput[3]);

	return solved;
}

int main(int argc, char *argv[])
{
	if (argc < 4 || argc > 5)
	{
		printf("Usage: ./Knight.out grid_size StartX StartY [mode] (mode: single|multi|both)\n");
		exit(-1);
	}

	N = atoi(argv[1]);
	int StartX = atoi(argv[2]);
	int StartY = atoi(argv[3]);

	total_squares = N * N;

	const char *mode = (argc == 5 ? argv[4] : "both");

	if (debugCheck)
		printf("Mode: %s\n", mode);

	struct timeval start, end;

	if (strcmp(mode, "single") == 0 || strcmp(mode, "s") == 0)
	{
		gettimeofday(&start, NULL);
		print_time_label("Start time (single)", start);
		int ok = run_single(StartX, StartY);
		gettimeofday(&end, NULL);
		print_time_label("End time (single)", end);
		printf("Total duration (single): %.3f ms\n", elapsed_ms(start, end));
		return ok ? 0 : 1;
	}
	else if (strcmp(mode, "multi") == 0 || strcmp(mode, "m") == 0)
	{
		gettimeofday(&start, NULL);
		print_time_label("Start time (multi)", start);
		int ok = run_multi(StartX, StartY);
		gettimeofday(&end, NULL);
		print_time_label("End time (multi)", end);
		printf("Total duration (multi): %.3f ms\n", elapsed_ms(start, end));
		return ok ? 0 : 1;
	}
	else
	{
		// both
		int overall_ok = 1;

		// single first
		gettimeofday(&start, NULL);
		print_time_label("Start time (single)", start);
		int ok_single = run_single(StartX, StartY);
		gettimeofday(&end, NULL);
		print_time_label("End time (single)", end);
		printf("Total duration (single): %.3f ms\n\n", elapsed_ms(start, end));
		overall_ok &= ok_single;

		// multi next
		gettimeofday(&start, NULL);
		print_time_label("Start time (multi)", start);
		int ok_multi = run_multi(StartX, StartY);
		gettimeofday(&end, NULL);
		print_time_label("End time (multi)", end);
		printf("Total duration (multi): %.3f ms\n", elapsed_ms(start, end));
		overall_ok &= ok_multi;

		return overall_ok ? 0 : 1;
	}
}
