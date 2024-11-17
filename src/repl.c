#include "repl.h"
#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>



typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND

} MetaCommandResult;

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;
typedef enum { STATEMENT_INSERT, STATEMENT_SHOW } StatementType;

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

void print_prompt() { 
    printf("db > "); 
}

void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read =
        getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Erreur de lecture.\n");
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
            printf("Erreur de syntaxe. Format attendu : insert <parent_id> <direction> <content>\n");
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }
        if (statement->direction != 'G' && statement->direction != 'D') {
            printf("La direction doit être G (gauche) ou D (droite).\n");
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }
        return PREPARE_SUCCESS;
    }

    if (strcmp(input_buffer->buffer, "show") == 0) {
        statement->type = STATEMENT_SHOW;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement, ArbreNoeud* racine, int* idCourant) {
    if (statement->type == STATEMENT_INSERT) {
        if (ajouterDonneeTexte(racine, statement->parent_id, statement->content, statement->direction, idCourant) == -1) {
            printf("Erreur : Impossible d'insérer le nœud.\n");
        } else {
            printf("Nœud inséré avec succès.\n");
        }
    } else if (statement->type == STATEMENT_SHOW) {
        if (racine == NULL) {
            printf("Erreur : L'arbre est vide.\n");
        } else {
            printf("\nStructure de l'arbre :\n");
            afficherArbreTexte(racine, 0);
        }
    }
}

void repl(ArbreNoeud* racine, int* idCourant) {
    printf("Bienvenue dans votre base de données basée sur un arbre binaire !\n");
    printf("Voici les commandes que vous pouvez utiliser :\n");
    printf("  - .exit      : Quitter le programme\n");
    printf("  - insert     : Ajouter un nœud à l'arbre\n");
    printf("                Syntaxe : insert <parent_id> <direction> <content>\n");
    printf("  - show       : Afficher l'arbre\n");
    printf("\n");

    InputBuffer* input_buffer = new_input_buffer();

    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Commande non reconnue : '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                execute_statement(&statement, racine, idCourant);
                break;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Mot-clé non reconnu au début de '%s'.\n",
                       input_buffer->buffer);
                continue;
        }
    }
}
