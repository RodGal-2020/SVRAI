/// Tema 2 - Ejercicio 1
/* Implementar de forma genérica los algoritmos Value-Iteration y Q-Learning para 
trabajar sobre MDPs definidos a partir de redes aleatorias. Genera experimentos 
que te permitan determinar cuánto tarda en converger a medida que aumenta el 
número de estados (ten cuidado de asegurarte de que el grafo de estados sea 
conexo), incluyendo un pequeño análisis de la importancia de los posibles 
parámetros de cada algoritmo. */


/// Formato MDP
/*	N n */
/*	s_i a s_j T */
/*	s_i a s_j T */
/*	s_i a s_j T */
/*	s_i a s_j T */
/*	s_i a s_j T */


#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

/// Parámetros del programa
#define N_max 100 // El número máximo de nodos x acciones que admitiremos (número de filas de T)
#define n_max 100 // El número máximo de nodos

#define verbose 0 // Imprimir mensajes informativos?
#define comment(x) if(verbose){x;} // Para comentar basándonos en el verbose (ejemplo: comment(printf("1 = %i", 1)))

#define error_time 1 // Si algo tarda más de 1 segundo lo guardamos como error

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

reward_matrix read_reward_matrix (char dir[100]) {
	reward_matrix r_exit;
	
	FILE *reward_m;
	reward_m = fopen(dir, "r");
	
	int row = 0;
	
	while (!feof(reward_m)) {
		fscanf(reward_m, "%i %f\n", &r_exit.s_i[row], &r_exit.r[row]);
		row++;
	}
	
	fclose(reward_m);
	return r_exit;
}

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

float max_a(int node, transition_matrix T, float u[n_max], int N, int n) {	
	// max_a sum_{s'} T(s,a,s') u(s')
	
	// Ahora obtenemos la suma para cada posible a, estableciendo cada acción como -1 una vez la hayamos usado.
	int a; // La acción en la que nos centraremos en cada paso
	float sTxu = 0; // El máximo buscado, de la suma de los productos T·u para la acción óptima
	float candidate_sTxu = sTxu; // El nuevo candidato a ser el sTxu, sum_{s'} T(s,a,s') u(s') para una acción concreta
	
	comment(printf("Arrancamos la búsqueda de mejor acción en el nodo %i\n", node))
	while(1) {
		// Nos centramos en una acción que no hayamos visitado ya, o finalizamos la búsqueda y devolvemos nuestro sTxu
		for (int i=0; i<N; i++) {
			if (T.a[i] != -1 && T.s_i[i] == node) { // Si la acción es aceptable (no ha sido seleccionada) y el nodo es el correcto
				a = T.a[i]; 
				comment(printf("\tNos centramos en la acción a = %i\n", a))
				break; // Como ya tenemos una acción seleccionada no buscamos más
			}
			
			if (i == N - 1) { // Si hemos llegado hasta aquí estamos en el último paso y lo anterior no se ha ejecutado, así que no quedan acciones disponibles.
				return sTxu;
			}
		}
		
		for (int i=0; i<N; i++) { // Recorremos la matriz para cacular Txu para esta acción
			if (T.a[i] == a && T.s_i[i] == node) { // Si la acción y el nodo son los correctos
				// sum_{s'} T(s,a,s') u(s')
				candidate_sTxu += T.p[i] * u[T.s_j[i]]; // Añadimos a lo acumulado lo indicado
				comment(printf("\t\t\tAñadimos el producto %f x %f a la suma candidata\n", T.p[i], u[T.s_j[i]]))
				T.a[i] = -1; // Tachamos la acción para futuras iteraciones
			}
		}
		
		comment(printf("\t\tTomando en %i la acción %i se obtiene una Txu = %f\n", node, a, candidate_sTxu))
		if (candidate_sTxu > sTxu) {
			comment(printf("Actualizamos el máximo: %f -> %f", sTxu, candidate_sTxu))
			sTxu = candidate_sTxu; 
		} 
		
		candidate_sTxu = 0; // Reseteamos y seguimos con el bucle buscando otras acciones
	}
}

void show_u_exit(float u[n_max], int n) {
	printf("s_i\tu\n");
	for (int i = 0; i<n; i++) {
		printf("%i\t%f\n", i, u[i]);
	}
	printf("\n");
}

void save(char dir[100], transition_matrix T, reward_matrix r, int N, int n, float gamma, float eps, float u[n_max]) {
	FILE *save_dir;
	
	save_dir = fopen(dir, "w+");
	
	// Cabecera (N, n)
	fprintf(save_dir, "N = %i, n = %i\n", N, n);
	
	// T
	fprintf(save_dir, "========================================================================================\n");
	fprintf(save_dir, "Matriz de transición T\n");
	fprintf(save_dir, "s_i\ta\ts_j\tT (p)\n");
	for (int i = 0; i<N; i++) {
		fprintf(save_dir, "%i\t%i\t%i\t%f\n", T.s_i[i], T.a[i], T.s_j[i], T.p[i]);
	}
	fprintf(save_dir, "\n");

	// r
	fprintf(save_dir, "========================================================================================\n");
	fprintf(save_dir, "Matriz de recompensas T\n");
	fprintf(save_dir, "s_i\tr\n");
	for (int i = 0; i<n; i++) {
		fprintf(save_dir, "%i\t%f\n", r.s_i[i], r.r[i]);
	}
	fprintf(save_dir, "\n");
	
	// grafo
	fprintf(save_dir, "========================================================================================\n");
	fprintf(save_dir, "Grafo asociado a T\n");
	for (int node = 0; node < n; node++) {
		fprintf(save_dir, "\ns_i: %i\n", node);
		for (int i = 0; i<N; i++) {
			if (T.s_i[i] == node) {
				fprintf(save_dir, "|\n|\n|\ta:%i\tp:%f\n\\----------------------------------> s_j: %i\n", T.a[i], T.p[i], T.s_j[i]);
			}
		}
	}	
	fprintf(save_dir, "\n");
	
	// Value-Iteration
	fprintf(save_dir, "========================================================================================\n");
	fprintf(save_dir, "Resultados de Value-Iteration\n");
	fprintf(save_dir, "gamma = %f, eps = %f\n", gamma, eps);
	// u
	fprintf(save_dir, "s_i\tu\n");
	for (int i = 0; i<n; i++) {
		fprintf(save_dir, "%i\t%f\n", i, u[i]);
	}
	fprintf(save_dir, "\n");
	
	fclose(save_dir);
	
	// Q-Learning
	fprintf(save_dir, "========================================================================================\n");
	fprintf(save_dir, "Resultados de Q-Learning\n");
	
	printf("Dados guardados correctamente.\n");
	
	printf("Esta funcionalidad (QL) aún no está disponible.\n");
}

char * int_2_char (int num) {
	static char num_char[3];
	if (num < 10) {
		num_char[0] = '0';
		num_char[1] = '0';
		num_char[2] = '0' + num;
		return num_char;
	} else if (num < 100) {
		num_char[0] = '0';
		num_char[1] = '0' + num / 10 ;
		num_char[2] = '0' + num % 10;
		return num_char;
	} else {
		num_char[0] = '0' + num / 100;
		num_char[1] = '0' + num % 100 / 10 ;
		num_char[2] = '0' + num % 10;
		return num_char;
	}
}

int main () {
	/// Preguntas iniciales
	int print_graph = 0;
	int print_structures = 0;
	
/*	float gamma = 0.5;*/
/*	float eps = 0.1; */
	
	float gammas[3] = {0.5, 0.2, 0.1};
	float epss[3] = {0.2, 0.15, 0.01};
	
	float gamma = gammas[rand() % 3];
	float eps = epss[rand() % 3]; 
	
	int default_conf = 1;
	printf("Desea usar el \"default\"? (1 ó 0)\n");
	scanf("%i", &default_conf);
	if (default_conf) {
		printf("default: verbose = %i, print_graph = 0, print_structures = 0, gamma = 0.5, 0.2 ó 0.1), épsilon = 0.2, 0.15 ó 0.01.\n", verbose);
	} else {
		printf("\nverbose? (1 ó 0)\n");
		printf("0, modificar el .c para cambiar esto\n");
		printf("\nprint_graph? (1 ó 0)\n");
		scanf("%i", &print_graph);
		printf("\n\nprint_structures? (1 ó 0)\n");
		scanf("%i", &print_structures);
		printf("\ngamma: ");
		scanf("%f", &gamma);
		printf("\neps: ");
		scanf("%f", &eps);
	}
	
	/// Parámetros
	srand(time(NULL));
	
	float u[n_max], up[n_max];		
		
	// Lectura
	int i_mat = 0;
	int n_mat;
	FILE *file_pointer;
	char directory_init[] = "MDPs/random_mat_"; 
	char directory[100];
	int N, n;
	int row;
	
	// Algoritmo Value-Iteration
	float d, m;
	int s_i[N_max], s_j[N_max], a[N_max];
	float p[N_max];
	transition_matrix T;
	
	// Control temporal
	clock_t t_init;	
	clock_t t_vi_1;
	clock_t t_end;
	double time_spent, t_vi_dif;
	
	// Guardado
	int r_mat = 0, save_all;
	char r_dir[100], save_dir[100];
	FILE *stats_pointer, *read_mat;
		
	printf("Indique el número de matrices que desea leer:\n");
	scanf("%i", &n_mat);
	
	while (i_mat < n_mat) {
		t_init = clock();	
		
		/// Lectura MDP
		if (n_mat == 1) {
			printf("Parece que quiere leer una matriz concreta. Escriba la dirección\n");
			scanf("%s", directory);
			printf("===============================================\n");
			printf("Accediendo al fichero %s\n", directory);	
			
			printf("Desea también recurrir a una matriz de recompensas particular? (1 ó 0)\n");
			scanf("%i", &r_mat);
			if (r_mat) {
				printf("Escriba la dirección, por favor:\n");
				scanf("%s", r_dir);
			}
		} else {
			strcpy(directory, directory_init);
			strcat(directory, int_2_char(i_mat));
			strcat(directory, ".txt");
			printf("===============================================\n");
			printf("Accediendo al fichero %s\n", directory);
		}
		
		
		file_pointer = fopen(directory, "r");
			
		fscanf(file_pointer, "%i %i\n", &N, &n);
		row = 0;
		
		comment(printf("Lectura de la matriz:\n"))
		while(row < N) {
			fscanf(file_pointer, "%i %i %i %f\n", &s_i[row], &a[row], &s_j[row], &p[row]);
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
		if (r_mat) {
			r = read_reward_matrix(r_dir);
		} else {
			printf("Generando matriz de recompensas aleatorias\n");
			for (int i = 0; i<n; i++) {
				r.s_i[i] = i;
				r.r[i] = rand() % 50;
			}
		}
		
		if(print_structures) {show_reward_matrix(r, n);}
		
		if(print_graph) {show_graph_transition_and_reward(T, r, N, n);}
		
		/// Value-Iteration
		printf("--------------------------------------------\n");
		printf("Arrancamos el algoritmo Value-Iteration.\n\n");
		t_vi_1 = clock();
			
		// Empezamos con u = 0
		for (int i = 0; i<n; i++) {
			u[i] = 0;
			up[i] = 0;
		}
		comment(printf("u preparada\n"));
				
		do { // Buscamos que lo ejecute al menos una vez
			
			// Control de errores
			t_vi_dif = (double)(clock() - t_vi_1) / CLOCKS_PER_SEC;
			comment(printf("t_vi_dif = %f''\n", t_vi_dif))
			if (t_vi_dif > error_time) {
				read_mat = fopen("stats/MDP_errors.txt", "a+");
				fprintf(read_mat, "%i\n", i_mat);
				fclose(read_mat);
				printf("Problemas de convergencia en VI. Detenemos el algoritmo.\n");
				break;
			}
				
			/* u' = u */
			for (int i = 0; i<n; i++) {
				u[i] = up[i]; 
			}
			comment(printf("Actualizamos u\n"))
			
			for (int node=0; node<n; node++) {
				m = max_a(node, T, u, N, n);
				up[node] = r.r[node] + gamma * m; /* u'(s) = r(s) + ? max_a S_{s'}T(s,a,s')u(s') */
				comment(printf("\n\tPara el nodo %i, max_a(%i, T, u) = %f\n", node, node, m))
				comment(printf("\tPara el nodo %i, u'(%i) = %f\n", node, node, up[node]))
			}
			comment(printf("Valores de u' actualizados para diferentes nodos:\n"))
				
			printf("s\tu\t\tu'\n");
			for(int node = 0; node<n; node++) {
				printf("%i\t%f\t%f\n",node, u[node], up[node]);
			}
			printf("\n");
			
			/*	¦	d = max_s |u'(s)-u(s)|*/
			d = abs(u[0] - up[0]);
			for (int node = 0; node<n; node++) {
				if (abs(u[node] - up[node]) > d) {
					d = abs(u[node] - up[node]);
				}
			}
			comment(printf("\td = %f\n", d))
			comment(printf("\teps * (1 - gamma) / gamma) = %f\n", eps * (1 - gamma) / gamma))
			if (d > eps * (1 - gamma) / gamma) {
				comment(printf("\tEntrando en una nueva iteración...\n"))
			} else {
				comment(printf("\tCondición de parada alcanzada.\n"))
			}	
		} while (d > eps * (1 - gamma) / gamma); /* mientras d > e(1-gamma)/gamma*/ 
		
		/*	Devolver u*/
		comment(printf("El algoritmo finaliza proporcionando la siguiente u:\n"))
		show_u_exit(up, n);
		
		/// Control temporal
		t_end = clock();
		time_spent = (double)(t_end - t_init) / CLOCKS_PER_SEC;
		printf("time_spent=%f''\n", time_spent);
		
		comment(printf("Guardamos las estadísticas del algoritmo en stats/MDP_sample.txt.\n"))
			
		stats_pointer = fopen("stats/MDP_sample.txt", "a+");
		// Formato: n_nodes time
		fprintf(stats_pointer, "%i %f %s %f %f\n", n, time_spent, "VI", gamma, eps); // Eliminamos up de aquí
		fclose(file_pointer);
		
		
		/// Q-Learning
		printf("\nArrancamos el algoritmo Q-Learning.\n");
		/*	Q-LEARNING*/
		/*		Inicializar Tabla Q(s, a) para cada s ? S y cada a ? A(s) (con 0’s o valores arbitrarios).*/
		/*		Repetir:*/
		/*		¦ Inicializar s.*/
		/*		¦ Hacer:*/
		/*		¦ ¦ Elegir a desde s usando una política derivada de Q.*/
		/*		¦ ¦ Ejecutar a, observar estado resultante s' y recompensa r.*/
		/*		¦ ¦	Q(s, a) ? Q(s, a) + a[r + ? max_a'Q(s', a') - Q(s, a)]*/
		/*			¦ ¦ s ? s'*/
		/*			+ +Hasta que s sea terminal*/
		
		if (n_mat == 1) {
			printf("Desea guardar los datos generados? (1 o 0)?\n");
			scanf("%i", &save_all);
			if (save_all) {
				printf("Especifique el directorio:\n");
				scanf("%s", save_dir);
				save(save_dir, T, r, N, n, gamma, eps, up);
			} else {
				printf("Programa finalizado");
			}
		}
		
		i_mat++; // Pasamos a la siguiente matriz
	}
}

