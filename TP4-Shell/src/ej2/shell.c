#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define READ 0
#define WRITE 1
#define MAX_COMMANDS 500

// Elimina espacios iniciales y finales
char *eliminar_espacios(char *str) {
    while (isspace(*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
    return str;
}

// Verifica si una cadena contiene pipes consecutivos o mal ubicados
int contiene_pipes_invalidos(const char *linea) {
    int i = 0;
    int prev_pipe = 0;
    int hay_contenido = 0;
    while (linea[i]) {
        if (linea[i] == '|') {
            if (prev_pipe || i == 0) return 1;
            prev_pipe = 1;
        } else if (!isspace(linea[i])) {
            prev_pipe = 0;
            hay_contenido = 1;
        }
        i++;
    }
    if (prev_pipe) return 1;
    return !hay_contenido; 
}

// Convierte una línea en argv[], respetando comillas y tabs
int dividir_comando_en_argumentos(char *linea, char **args) {
    int arg_count = 0;
    char buffer[512];
    int pos = 0;
    int entre_comillas = 0;
    char c, quote = '\0';

    while ((c = *linea++)) {
        if ((c == '"' || c == '\'') && !entre_comillas) {
            entre_comillas = 1;
            quote = c;
        } else if (c == quote && entre_comillas) {
            entre_comillas = 0;
            quote = '\0';
        } else if (isspace(c) && !entre_comillas) {
            if (pos > 0) {
                buffer[pos] = '\0';
                if (arg_count >= 100) {
                    fprintf(stderr, "Error: demasiados argumentos\n");
                    exit(1);
                }
                args[arg_count] = malloc(strlen(buffer) + 1);
                strcpy(args[arg_count++], buffer);
                pos = 0;
            }
        } else {
            buffer[pos++] = c;
        }
    }
    if (entre_comillas) {
        fprintf(stderr, "Error: comillas sin cerrar\n");
        exit(1);
    }
    if (pos > 0) {
        buffer[pos] = '\0';
        if (arg_count >= 100) {
            fprintf(stderr, "Error: demasiados argumentos\n");
            exit(1);
        }
        args[arg_count] = malloc(strlen(buffer) + 1);
        strcpy(args[arg_count++], buffer);
    }
    args[arg_count] = NULL;
    return arg_count;
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
    dividir_comando_en_argumentos(segmento, args);

    if (args[0] == NULL || strlen(args[0]) == 0) {
        fprintf(stderr, "Error: comando vacío entre pipes\n");
        exit(1);
    }

    execvp(args[0], args);
    perror("execvp falló");
    exit(1);
}

int main() {
    char linea[512];
    char *segmentos[MAX_COMMANDS];
    int num_commands;

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

        if (linea[0] == '|') {
            fprintf(stderr, "Error de sintaxis: pipe al inicio\n");
            continue;
        }

        if (contiene_pipes_invalidos(linea)) {
            fprintf(stderr, "Error de sintaxis: pipes consecutivos o mal ubicados\n");
            continue;
        }

        num_commands = 0;
        char *token = strtok(linea, "|");
        int error_detectado = 0;

        while (token != NULL) {
            char *limpio = eliminar_espacios(token);
            if (strlen(limpio) == 0) {
                fprintf(stderr, "Error: comando vacío entre pipes\n");
                error_detectado = 1;
                break;
            }
            segmentos[num_commands++] = limpio;
            if (num_commands >= MAX_COMMANDS) {
                fprintf(stderr, "Error: demasiados comandos encadenados\n");
                exit(1);
            }
            token = strtok(NULL, "|");
        }

        if (error_detectado || num_commands == 0) continue;

        int anterior[2];
        int actual[2];

        for (int i = 0; i < num_commands; i++) {
            if (i < num_commands - 1) pipe(actual);

            pid_t pid = fork();
            if (pid == 0) {
                int primero = (i == 0);
                int ultimo = (i == num_commands - 1);
                ejecutar_segmento(segmentos[i], primero, ultimo, anterior, actual);
            }

            if (i > 0) {
                close(anterior[READ]);
                close(anterior[WRITE]);
            }

            if (i < num_commands - 1) {
                anterior[READ] = actual[READ];
                anterior[WRITE] = actual[WRITE];
            }
        }

        for (int i = 0; i < num_commands; i++) {
            wait(NULL);
        }
    }

    return 0;
}
