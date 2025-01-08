/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 *   dlopen()
 *   dlclose()
 *   dlsym()
 */

/* research the above Needed API and design accordingly */


struct jitc {
    void *handle;
};

int jitc_compile(const char *input, const char *output){
    pid_t pid;
    int status;
    pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        char *gcc_path = "/usr/bin/gcc";
        char *argv[] = {"gcc", "-fPIC", "-shared", NULL, "-lm", "-o", NULL, NULL};
        argv[3] = (char *)input;
        argv[6] = (char *)output;
        printf("%s\n", argv[3]);
        printf("%s\n", argv[6]);
        execv(gcc_path, argv);
        perror("execv failed.\n");
        exit(EXIT_FAILURE);
    } else {
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed.\n");
            return -1;
        } 
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child did not exit successfully\n");
        }
    }
    return EXIT_SUCCESS;
}

struct jitc *jitc_open(const char *pathname){
    struct jitc *j = malloc(sizeof(struct jitc));
    char newPathname[50];
    sprintf(newPathname, "./%s", pathname);
    if (!j) return NULL;
    j->handle = dlopen(newPathname, RTLD_LAZY);
    if (j->handle){
        printf("We have j.handle.\n");
    }
    if (!j->handle) {
        printf("We dont have j.handle.\n");
        fprintf(stderr, "Error opening lebrary: %s\n", dlerror());
        free(j);
        return NULL;
    }
    return j;
}

void jitc_close(struct jitc *jitc){
    if (jitc->handle) {
        dlclose(jitc->handle);
    }
    free(jitc);
}

long jitc_lookup(struct jitc *jitc, const char *symbol){
    void *func = dlsym(jitc->handle, symbol);
    return (long)func;
}
