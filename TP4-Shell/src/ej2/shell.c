#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 1000
#define READ 0
#define WRITE 1

// Convierte una línea en argv[], respetando comillas simples y dobles
void tokenizar_con_comillas(char *linea, char **args) {
    int arg_count = 0;
    char buffer[256];
    int pos = 0;
    int entre_comillas = 0;
    char c, delimitador = '\0';

    while ((c = *linea++)) {
        if ((c == '"' || c == '\'') && !entre_comillas) {
            entre_comillas = 1;
            delimitador = c;
        } else if (c == delimitador && entre_comillas) {
            entre_comillas = 0;
        } else if (c == ' ' && !entre_comillas) {
            if (pos > 0) {
                buffer[pos] = '\0';
                args[arg_count] = malloc(strlen(buffer) + 1);
                strcpy(args[arg_count++], buffer);
                pos = 0;
            }
        } else {
            buffer[pos++] = c;
        }
    }
    if (pos > 0) {
        buffer[pos] = '\0';
        args[arg_count] = malloc(strlen(buffer) + 1);
        strcpy(args[arg_count++], buffer);
    }
    args[arg_count] = NULL;
}

// Ejecuta un segmento de la línea con redirecciones si es necesario
void ejecutar_segmento(char *segmento, int es_primero, int es_ultimo, int *entrada, int *salida) {

    if (!es_primero) {
        dup2(entrada[READ], STDIN_FILENO);
        close(entrada[READ]);
        close(entrada[WRITE]);
    }
    if (!es_ultimo) {
        close(salida[READ]);
        dup2(salida[WRITE], STDOUT_FILENO);
        close(salida[WRITE]);
    }

    char *args[100];
    tokenizar_con_comillas(segmento, args);
    execvp(args[0], args);
    perror("execvp falló");
    exit(1);
}

int main() {

    char linea[256];
    char *segmentos[MAX_COMMANDS];  
    int num_segmentos;

    while (1) {
        printf("Shell> ");
        fflush(stdout);

        if (!fgets(linea, sizeof(linea), stdin)) {
            printf("\nSaliendo del shell (EOF recibido)\n");
            break;
        }

        linea[strcspn(linea, "\n")] = '\0';

        if (strcmp(linea, "exit") == 0) {
            printf("Saliendo del shell (comando exit)\n");
            break;
        }

        num_segmentos = 0;
        char *token = strtok(linea, "|");
        while (token != NULL) {
            segmentos[num_segmentos++] = token;
            token = strtok(NULL, "|");
        }

        int anterior[2]; 
        int actual[2];   

        for (int i = 0; i < num_segmentos; i++) {
            if (i < num_segmentos - 1)
                pipe(actual);

            pid_t pid = fork();
            if (pid == 0) {
                int es_primero = (i == 0);
                int es_ultimo  = (i == num_segmentos - 1);
                ejecutar_segmento(segmentos[i], es_primero, es_ultimo, anterior, actual);
            }

            if (i > 0) {
                close(anterior[READ]);
                close(anterior[WRITE]);
            }

            if (i < num_segmentos - 1) {
                anterior[READ] = actual[READ];
                anterior[WRITE] = actual[WRITE];
            }
        }

        for (int i = 0; i < num_segmentos; i++) {
            wait(NULL);
        }
    }

    return 0;
}
