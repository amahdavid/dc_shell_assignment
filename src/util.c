#include <string.h>
#include "state.h"
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <malloc.h>
#include <stdlib.h>
#include "shell.h"
char *get_path(const struct dc_env *env, struct dc_error *err)
{

    char *path_env = getenv("PATH");
    char **path;
    if (path_env != NULL){
        path = malloc(sizeof (char *) * strlen(path_env));
        char *token = strtok(path_env, ":");
        int i = 0;
        while (token != NULL){
            path[i++] = token;
            token = strtok(NULL, ":");
        }
    }
    // should it be return path or path env?
    return path_env;
}

const char *get_prompt(const struct dc_env *env, struct dc_error *err){
    char *ps1_env = getenv("PS1");
    if (ps1_env == NULL){
        return "$ ";
    } else
    {
        return ps1_env;
    }
}

char **parse_path(const struct dc_env *env, struct dc_error *err, char *path_str){
    char **path = NULL;
    int path_len = 0;
    char *token = strtok(path_str, "/");
    while (token != NULL){
        path = realloc(path, sizeof (char *) * path_len);
        if (path == NULL){
            dc_error_has_error(err);
            return NULL;
        }
        path[path_len - 1] = token;
        token = strtok(NULL, "/");
    }

    path = realloc(path, sizeof (char *) * (path_len + 1));
    path[path_len] = 0;

    return path;
}

void do_reset_state(const struct dc_env *env,
                    struct dc_error *err, struct state *state) {
    free(state->current_line);
    state->current_line = NULL;
    memset(err, 0, sizeof(state));
    // should return type be void or int?
    //return READ_COMMANDS;
}

char *expand_path(const struct dc_env *env, struct dc_error *err, char *file){
    if (file[0] == '~'){
        char *home_directory = getenv("HOME");
        if (home_directory == NULL){
            // handle error
            printf("ERROR: HOME environment variable not found\n");
            return NULL;
        }
        size_t home_directory_len = strlen(home_directory);
        size_t file_len = strlen(file);
        char *expanded_file = malloc(home_directory_len + file_len);
        if (expanded_file == NULL){
            printf("ERROR: failed to allocate memory for expanded file\n");
            return NULL;
        }
        strcpy(expanded_file, home_directory);
        strcpy(expanded_file + home_directory_len, file + 1);
        return expanded_file;
    } else{
        // file path does not start with ~, return original path
        return file;
    }
}