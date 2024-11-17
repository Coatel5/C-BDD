#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "btree.h"

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;
typedef enum { STATEMENT_INSERT } StatementType;

typedef struct {
  StatementType type;
  int parent_id; 
  char direction; 
  char content[100]; 
} Statement;

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;
  return input_buffer;
}

void print_prompt() { printf("db > "); }

void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer) {
  free(input_buffer->buffer);
  free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    close_input_buffer(input_buffer);
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    int args_assigned = sscanf(input_buffer->buffer, "insert %d %c %[^\n]",
                                &statement->parent_id, &statement->direction, statement->content);
    if (args_assigned < 3) {
      printf("Syntax error. Correct format: insert <parent_id> <direction> <content>\n");
      return PREPARE_UNRECOGNIZED_STATEMENT;
    }
    if (statement->direction != 'G' && statement->direction != 'D') {
      printf("Direction must be G (left) or D (right).\n");
      return PREPARE_UNRECOGNIZED_STATEMENT;
    }
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement, FILE* fichier, long racineOffset, int* idCourant) {
  if (statement->type == STATEMENT_INSERT) {
    if (ajouterDonnee(fichier, statement->parent_id, statement->content, statement->direction, idCourant) == -1) {
      printf("Error: Unable to insert node.\n");
    } else {
      printf("Node inserted successfully.\n");
    }
  }
}

void repl(FILE* fichier, long racineOffset, int* idCourant) {
    printf("Bienvenue dans votre base de données basée sur un arbre binaire !\n");
    printf("Voici les commandes que vous pouvez utiliser :\n");
    printf("  - .exit      : Quitter le programme\n");
    printf("  - insert     : Ajouter un nœud à l'arbre\n");
    printf("                Syntaxe : insert <parent_id> <direction> <content>\n");
    printf("\n");

    InputBuffer* input_buffer = new_input_buffer();

    while (true) {
        print_prompt();
        read_input(input_buffer);
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Commande non reconnue '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                execute_statement(&statement, fichier, racineOffset, idCourant);
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Mot-clé non reconnu au début de '%s'.\n",
                       input_buffer->buffer);
                continue;
        }
        printf("Commande exécutée.\n");
    }
}
