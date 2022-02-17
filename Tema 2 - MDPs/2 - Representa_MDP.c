#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define N_max 100 // El número máximo de nodos x acciones que admitiremos (número de filas de T)
#define n_max 100 // El número máximo de nodos

#define verbose 1
#define comment(x) if(verbose){x;} // Para comentar basándonos en el verbose (ejemplo: comment(printf("1 = %i", 1)))

typedef struct transition_matrixes {
	int s_i[N_max]; // Nodo inicial
	int a[N_max]; // Acción a
	int s_j[N_max]; // Nodo de salida
	float p[N_max]; // Probabilidad de llegar al nodo de salida
} transition_matrix;

typedef struct reward_matrixes {
	int s_i[n_max]; // Los diferentes nodos
	float r[n_max]; // Las diferentes recompensas
} reward_matrix;

void show_transition_matrix (transition_matrix T, int N) {
	printf("s_i\ta\ts_j\tT (p)\n");
	for (int i = 0; i<N; i++) {
		printf("%i\t%i\t%i\t%f\n", T.s_i[i], T.a[i], T.s_j[i], T.p[i]);
	}
	printf("\n");
}

void show_reward_matrix (reward_matrix r, int n) {
	printf("s_i\tr\n");
	for (int i = 0; i<n; i++) {
		printf("%i\t%f\n", r.s_i[i], r.r[i]);
	}
	printf("\n");
}

void show_graph_transition_and_reward(transition_matrix T, reward_matrix r, int N, int n) {
	for (int node = 0; node < n; node++) {
		printf("\ns_i: %i\n", node);
		for (int i = 0; i<N; i++) {
			if (T.s_i[i] == node) {
				printf("|\n|\n|\ta:%i\tp:%f\n\\----------------------------------> s_j: %i\n", T.a[i], T.p[i], T.s_j[i]);
			}
		}
	}	
	printf("\n");
}

int main () {
	/// Preguntas iniciales
	// int verbose = 1; // PENDIENTE
	int print_graph = 1;
	int print_structures = 1;
	int default_conf = 1;
	printf("Desea usar el \"default display\"? (1 ó 0)\n");

	/// Control temporal
	clock_t t_init = clock();
	
	/// Lectura del MDP
	if(print_structures) {printf("Partimos de la siguiente estructura de datos:\n");}
	transition_matrix T;

	// Formato MDP
		/*	N n */
		/*	s_i a s_j T */
		/*	s_i a s_j T */
		/*	s_i a s_j T */
		/*	s_i a s_j T */
		/*	s_i a s_j T */
		
	FILE *file_pointer;
	// char directory[100] = "MDP/MDP_1.txt"; // Caso sencillo
	// char directory[100] = "MDP/MDP_2.txt"; // Otro caso sencillo
	char directory[100]; 
	printf("Escriba la dirección del MDP, o 1 si desea usar una predefinida:\n");
	scanf("%s", directory); 
	if (directory[0] == '1') {
		printf("Usando dirección por defecto: ");
		strcpy(directory, "MDP/MDP_1.txt");
		printf("%s", directory);
	} 

	file_pointer = fopen(directory, "r");

	int N, n;
	fscanf(file_pointer, "%i %i\n", &N, &n);
	
	int row = 0;
	int s_i[N_max], s_j[N_max], a[N_max];
	float p[N_max];
	
	// while(!feof(file_pointer)) { // Clásico y alternativo: PENDIENTE: ELIMINAR
	comment(printf("Lectura de la matriz:\n"))
	while(row < N) {
		fscanf(file_pointer, "%i %i %i %f\n", &s_i[row], &a[row], &s_j[row], &p[row]);
		// comment(printf("%i\t%i\t%i\t%f\n", s_i[row], a[row], s_j[row], p[row])) // PENDIENTE: ELIMINAR
		row++;
	}
	
	fclose(file_pointer);
	
	for (int i = 0; i<N; i++) {
		T.s_i[i] = s_i[i];
		T.s_j[i] = s_j[i];
		T.a[i] = a[i];
		T.p[i] = p[i];
	}
	
	if(print_structures) {
		printf("Estructura de la matriz:\n");
		show_transition_matrix(T, N);
	}
	
	reward_matrix r; 
	int s_i_r[n_max] = {0,1,2,3};
	float r_r[n_max] = {1, 3, 5, 9};
	for (int i = 0; i<n; i++) {
		r.s_i[i] = s_i_r[i];
		r.r[i] = r_r[i];
	}
	
	if(print_structures) {show_reward_matrix(r, n);}
	
	if(print_graph) {show_graph_transition_and_reward(T, r, N, n);}
}

