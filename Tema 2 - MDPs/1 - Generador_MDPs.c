/// Generador aleatorio de MDP con el formato definido
// Formato MDP
/*	N n */
/*	s_i a s_j T */
/*	s_i a s_j T */
/*	s_i a s_j T */
/*	s_i a s_j T */
/*	s_i a s_j T */


#include <stdio.h>

// Números aleatorios:
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Parámetros
#define verbose 0
#define extra_verbose 0
#define comment(x) if(verbose){x;}
#define extra_comment(x) if(extra_verbose){x;}
#define N_max 100 // Tamaño máximo de la matriz
#define n_max 30 // Máximo número de nodos

typedef struct transition_matrixes {
	int s_i[N_max]; // Nodo inicial
	int a[N_max]; // Acción a
	int s_j[N_max]; // Nodo de salida
	float p[N_max]; // Probabilidad de llegar al nodo de salida
} transition_matrix;

void show_transition_matrix (transition_matrix T, int N) {
	printf("s_i\ta\ts_j\tT (p)\n");
	for (int i = 0; i<N; i++) {
		printf("%i\t%i\t%i\t%f\n", T.s_i[i], T.a[i], T.s_j[i], T.p[i]);
	}
	printf("\n");
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

void print_transition_matrix (transition_matrix T, int N, int n, int i) {
	
	FILE *file_pointer;
	char directory[100] = "MDPs/random_mat_"; 
	strcat(directory, int_2_char(i));
	strcat(directory, ".txt");
	printf("%s", directory);
	
	file_pointer = fopen(directory, "w+");
	
	fprintf(file_pointer, "%i %i\n", N, n);
	
	int row = 0;
	while (row < N) {
		fprintf(file_pointer, "%i %i %i %f\n", T.s_i[row], T.a[row], T.s_j[row], T.p[row]); 
		row++;
	}
	
	fclose(file_pointer);
}

float probability_evolution (float probability, int identifier) {
	// Caso probabilidad nueva acción
	if (identifier == 0) {
		return probability / 2; // Por ejemplo
	// Caso probabilidad nueva salida
	} else {
		return probability / 1.5; // Por ejemplo
	}
}

int main() {
	srand(time(NULL));   // Inicialización para la generación de números aleatorios con rand()
	int i_mat = 0; // Índice de la matriz con la que trabajamos
	int n_mat = 10; // Número de matrices
	
	/// Parámetros y creación de la matriz
	int s_j;
	int n, N;
	transition_matrix T;
	float base_p_another_action = 0.7; // Probabilidad base de que un nodo tenga una acción adicional
	float base_p_another_exit = 0.5; // Probabilida base de que una acción tenga una salida adicional
	float another_action_p = base_p_another_action;
	float another_exit_p = base_p_another_exit;

	// Eliminación de duplicados
	int rep_a[n_max][N_max][n_max];
	
	printf("Cuántas matrices desea crear? (n<=1000)\n");
	scanf("%i", &n_mat);
	
	printf("Desea que las matrices tengan el número prefijado de nodos (6), uno escogido (n>1) o uno al azar dada una cota superior? (1, 2, 3)\n");
	int ans, new_n;
	scanf("%i", &ans);
	if (ans == 2 || ans == 3) {
		printf("Elija el número prefijado o la cota para el número de nodos (n<=30): \n");
		scanf("%i", &new_n);
	}
	
	/// Fijando parámetros
	if (ans == 1) {
		n = 6; // Por defecto
	}
	if (ans == 2) { // Modificado
		n = new_n;
	}
	
	while (i_mat < n_mat) {	
		if (ans == 3) { // Al azar con cota superior para cada iteración
			if (new_n < 2) {
				printf("Argumento no válido.");
				exit(0);
			}
			do {
				n = rand() % new_n + 1; // Con el "%" podemos obtener un 0, caso que no nos interesa
			} while (n <= 1); // (n>1) para casos no triviales
		}
		
		printf("n = %i\n", n);
		
		N = n; // Tamaño de la matriz - 1
		int actions[n]; // Número de acciones - 1 ya definidas para cada nodo	
	
		extra_comment(printf("Sin nada:\n"))
		extra_comment(show_transition_matrix(T, N))
			
		// Incluimos todos los nodos básicos antes de nada
		for (int i = 0; i < n; i++) {
			T.s_i[i] = i;
			T.a[i] = 0; // Contamos las acciones de forma diferente para cada nodo
			do {s_j = rand() % n;} while (s_j == i);
			T.s_j[i] = s_j; // Conectamos con otro nodo
			T.p[i] = rand() + 1; // Luego vamos a normalizar, así que no es necesario que estos valores sean siquiera probabilidades o float, pero sí que sean >0
			actions[i] = 0; // Ya tenemos una acción definida para cada nodo
		}
		
		extra_comment(printf("Nodos básicos:\n"))
		extra_comment(show_transition_matrix(T, N))
		
		float another_action, another_exit;
		int a, new_action = 0;
		
		/// Para cada nodo
		for(int node = 0; node < n; node++) {
			comment(printf("Nodo %i\n", node))
			/// Vamos a añadir otra acción u otra salida?
			another_action = (float)rand()/(float)(RAND_MAX) < base_p_another_action;
			another_exit = (float)rand()/(float)(RAND_MAX) < base_p_another_exit;
					
			while (another_exit || another_action || new_action) {
				/// Nos fijamos en una acción ya definida, al azar
				a = rand() % (actions[node] + 1); 
				comment(printf("\tNos fijamos en la acción %i.\n", a))
				
				/// another_exit
				if (another_exit || new_action) {
					comment(printf("\t\tAñadimos una salida adicional para la acción %i.\n", a))
					T.s_i[N] = node;
					T.s_j[N] = rand() % n;
					T.a[N] = a;
					T.p[N] = rand() + 1;
					N++;
					
					// Con esto logramos una evolución en la probabilidad de cada uno
					another_exit_p = probability_evolution(another_exit_p,0);
					another_exit = (float)rand()/(float)(RAND_MAX) < another_exit_p;
				}
			
				comment(printf("\n"))
				
				/// another_action
				if (another_action) {
					new_action = 1; // Como hemos añadido una nueva acción debería considerar nuevamente todas las acciones desde el nodo en cuestión
					T.s_i[N] = node;
					T.s_j[N] = rand() % n;
					actions[node]++;
					T.a[N] = actions[node];
					comment(printf("\tAñadimos una acción nueva con identificador %i.\n", actions[node]))
					T.p[N] = rand() + 1;
					N++;
					another_action_p = probability_evolution(another_action_p,0);
					another_action = (float)rand()/(float)(RAND_MAX) < another_action_p;
				} else {
					new_action = 0;
				}
				comment(printf("\n\n"))
			}
		}
		
		
		extra_comment(printf("Tras anothers:\n"))
		extra_comment(show_transition_matrix(T, N))
		
		/// Aseguramos la conexión del grafo
		// Notemos que no basta con que se pueda llegar de un nodo a otro cualquiera al ser dirigido, sino que es necesario que dado uno sea posible llegar a cualquier otro.
		
		int visited_node[n]; // Guardaremos aquí, para cada nodo, si está conectado con el nodo i en el que nos centramos
		int unvisited; // Nodo desconectado
		int from_node; // Nodo con el que lo conectaremos
		
		// Buscaremos uno que no hayamos visitado aún a partir de i, y si no hay ninguno seguimos hasta que los hayamos recorrido todos
		
		for (int i = 0; i<n; i++) { // Para cada uno de los n nodos
			comment(printf("Partimos del nodo %i.\n", i))
			visited_node[i] = 1; // Empezamos visitando el nodo i
			for (int node = 0; node < n; node++) {  
				if (node != i) {
					visited_node[node] = 0; // Los otros aún no han sido visitados
				}
			}
			
			// Implementamos el algoritmo de visita voraz
			for (int rep = 0; rep < n; rep++) { // Repitiéndolo este número de veces garantizamos que hayamos visitado todo lo posible
				for (int node = 0; node < n; node++) {
					if (visited_node[node]) { // Centrándonos en aquellos ya visitados
						for (int j = 0; j<N; j++) { // Recorremos T buscando conexiones con node
							if (T.s_i[j] == node) { 
								visited_node[T.s_j[j]] = 1; // Y añadimos aquellos que estamos visitando partiendo de i
							}
						}
					}
				}
			}
			// Con esto ya tenemos un vector visited_node que nos indica los nodos conectados con el nodo i
			
			do {
				// Buscamos uno desconectado (no visitado), que guardaremos en unvisited
				for (int node = 0; node < n; node++) { 
					if (!visited_node[node]) { 
						unvisited = node;
						comment(printf("El nodo %i no está conectado.\n", unvisited))
						
						do {
							from_node = rand() % n;
							comment(printf("\tCandidato a from_node: %i\n", from_node))
						} while (!visited_node[from_node]);
						
						T.s_i[N] = from_node;
						T.a[N] = rand() % (actions[from_node] + 1); // Lo añadimos como salida para una acción ya definida
						T.s_j[N] = unvisited;
						comment(printf("\tCreamos una salida de acción conectando %i->%i.\n", from_node, unvisited))
						T.p[N] = rand() + 1;
						N++;
						
						visited_node[unvisited] = 1;
						
						break;
					}
					
					if (node == n-1) {
						unvisited = -1; 
						comment(printf("Todos los nodos han sido visitados. Detenemos la búsqueda.\n"))
					}
				}
			} while (unvisited != -1); // Mientras aún haya alguno desconectado
		}
		
		extra_comment(printf("Tras conectar:\n"))
		extra_comment(show_transition_matrix(T, N))
		
		comment(printf("\n"))
		
		/// Eliminación de duplicados
		// Podríamos llegar a una matriz con las siguientes entradas:
		// 0       1       1       0.440150
		// 0       1       1       0.559850
			
		// Contamos apariciones
		for (int j = 0; j<N; j++) { // Recorremos filas
			rep_a[T.s_i[j]][T.a[j]][T.s_j[j]] = 0;  // Y anotamos la 0-ésima aparición para cada trío (s_i, a, s_j)
		}
		
		for (int j = 0; j<N; j++) { // Recorremos filas
			rep_a[T.s_i[j]][T.a[j]][T.s_j[j]]++;  // Y anotamos apariciones
		}
		
		// Si (duplicado) {eliminamos ~ movemos arriba y podamos} 
		for (int j = 0; j<N; j++) {
			if (rep_a[T.s_i[j]][T.a[j]][T.s_j[j]] > 1) {
				// Mandamos la última fila (la N-1) a la j-ésima (la que habímos visto que estaba repetida) y hacemos N--
				T.s_i[j] = T.s_i[N-1];
				T.a[j] = T.a[N-1];
				T.s_j[j] = T.s_j[N-1];
				T.p[j] = T.p[N-1];
				rep_a[T.s_i[j]][T.a[j]][T.s_i[j]]--;
				N--; 
			}
		}
			
		extra_comment(printf("Tras eliminar duplicados:\n"))
		extra_comment(show_transition_matrix(T, N))
		
		/// Normalización de las probabilidades generadas
		float sum_p;
		
		// Para cada nodo
		for (int node = 0; node < n; node++) {
			// Para cada acción
			for (a = 0; a <= actions[node]; a++) { // <= dado que actions[i] es (nº de acciones en node) - 1
				// Establecemos el total (sum_{a x node} p) en 0.
				sum_p = 0;
				
				// Y lo calculamos
				for (int j = 0; j<N; j++) {
					if (T.a[j] == a && T.s_i[j] == node) {
						sum_p += T.p[j];
					}
				}
				
				// Actualizamos ahora según el total cada p asociada a "a x s_i"
				for (int j = 0; j<N; j++) {
					if (T.a[j] == a && T.s_i[j] == node) {
						T.p[j] /= sum_p;
					}
				}
				
				
			}
		}
		
		
		
		/// Impresión por pantalla
		show_transition_matrix(T, N);
		
		/// Guardado de la matriz
		print_transition_matrix(T, N, n, i_mat);
		
		i_mat++; // Pasamos a la siguiente matriz
		printf("\n\n================================================================\n");
		printf("================================================================\n\n");
	}
}
