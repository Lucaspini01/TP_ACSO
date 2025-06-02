#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

typedef int Canal[2];

void crear_procesos(int n, int start, int valor_inicial, Canal *canales, int *pipe_padre) {
	int buffer[1];
	buffer[0] = valor_inicial;

	for (int i = 0; i < n; i++) {
		pid_t hijo = fork();

		if (hijo < 0) {
			perror("Error al hacer fork");
			exit(1);
		}

		if (hijo == 0) {
			int anterior = (i - 1 + n) % n;

			for (int j = 0; j < n; j++) {
				if (j != anterior) close(canales[j][READ]);
				if (j != i)        close(canales[j][WRITE]);
			}
			close(pipe_padre[READ]);

			if (read(canales[anterior][READ], buffer, sizeof(int)) > 0) {
				buffer[0]++;
				if (i == start)
					write(pipe_padre[WRITE], buffer, sizeof(int));
				else
					write(canales[i][WRITE], buffer, sizeof(int));
			}

			close(canales[anterior][READ]);
			close(canales[i][WRITE]);
			close(pipe_padre[WRITE]);
			exit(0);
		}
	}
}

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Uso: %s <n> <c> <s>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int n = atoi(argv[1]);
	int mensaje_inicial = atoi(argv[2]);
	int inicio = atoi(argv[3]);

	printf("Se crearán %d procesos, se enviará el valor %d desde el proceso %d\n", n, mensaje_inicial, inicio);

	Canal canales[n];
	int pipe_padre[2];
	pipe(pipe_padre);

	for (int i = 0; i < n; i++) {
		if (pipe(canales[i]) == -1) {
			perror("No se pudo crear un pipe");
			exit(1);
		}
	}

	crear_procesos(n, inicio, mensaje_inicial, canales, pipe_padre);

	for (int i = 0; i < n; i++) {
		if (i != inicio)
			close(canales[i][WRITE]);
		if (i != (inicio - 1 + n) % n)
			close(canales[i][READ]);
	}
	close(pipe_padre[WRITE]);

	write(canales[inicio][WRITE], &mensaje_inicial, sizeof(int));

	close(canales[inicio][WRITE]);

	int resultado;
	read(pipe_padre[READ], &resultado, sizeof(int));
	close(pipe_padre[READ]);

	for (int i = 0; i < n; i++) wait(NULL);

	printf("Mensaje final recibido por el padre: %d\n", resultado);
	return 0;
}
