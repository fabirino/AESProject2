// #include "funcs.h"
#include <lxc/lxccontainer.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void print_menu() {

    printf("                  ____  __  ___     ___ \n");
    printf("  /\\/\\  _   _    / /\\ \\/ / / __\\   / _ \\_ __ ___   __ _ _ __ __ _ _ __ ___  \n");
    printf(" /    \\| | | |  / /  \\  / / /     / /_)/ '__/ _ \\ / _` | '__/ _` | '_ ` _ \\ \n");
    printf("/ /\\/\\ \\ |_| | / /___/  \\/ /___  / ___/| | | (_) | (_| | | | (_| | | | | | |\n");
    printf("\\/    \\/\\__, | \\____/_/\\_\\____/  \\/    |_|  \\___/ \\__, |_|  \\__,_|_| |_| |_|\n");
    printf("        |___/                                     |___/                     \n");

    printf("Choose an option:\n");
    printf("1. List containers\n");
    printf("2. Create container\n");
    printf("3. Destroy container\n");
    printf("4. Start container\n");
    printf("5. Stop container\n");
    printf("6. Execute command in container\n");
    printf("7. Establish a console connection with the container\n");
    printf("0. Exit\n");
}

void clear_screen() {
    printf("Press ENTER to clear the screen...");
    while (getchar() != '\n')
        ;
    printf("\033[H\033[J");
}

int main2() {
    struct lxc_container *c;
    int ret = 1;

    /* Setup container struct */
    c = lxc_container_new("apicontainer", NULL);
    if (!c) {
        fprintf(stderr, "Failed to setup lxc_container struct\n");
        goto out;
    }

    if (c->is_defined(c)) {
        fprintf(stderr, "Container already exists\n");
        /* Destroy the container */
        if (!c->destroy(c)) {
            fprintf(stderr, "Failed to destroy the container.\n");
            goto out;
        }
        goto out;
    }

    /* Create the container */
    if (!c->createl(c, "ubuntu", NULL, NULL, LXC_CREATE_QUIET,
                    "-d", "ubuntu", "-r", "trusty", "-a", "amd64", NULL)) {
        // if (!c->createl(c, "download", NULL, NULL, LXC_CREATE_QUIET,
        //                 "-d", "ubuntu", "-r", "trusty", "-a", "i386", NULL)) {
        fprintf(stderr, "Failed to create container rootfs\n");
        goto out;
    }

    /* Start the container */
    if (!c->start(c, 0, NULL)) {
        fprintf(stderr, "Failed to start the container\n");
        goto out;
    }

    /* Query some information */
    printf("Container state: %s\n", c->state(c));
    printf("Container PID: %d\n", c->init_pid(c));

    /* Stop the container */
    if (!c->shutdown(c, 30)) {
        printf("Failed to cleanly shutdown the container, forcing.\n");
        // sleep(10);
        if (!c->stop(c)) {
            fprintf(stderr, "Failed to kill the container.\n");
            goto out;
        }
    }
    printf("Container Stopped\n");
    /* Destroy the container */
    if (!c->destroy(c)) {
        fprintf(stderr, "Failed to destroy the container.\n");
        goto out;
    }
    printf("Container Destroyed\n");
    ret = 0;
out:
    lxc_container_put(c);
    return ret;
}

int main(int argc, char **argv) {

    int finish = 0;
    struct lxc_container *c;
    char container_name[64];

    while (!finish) {
        memset(container_name, 0, 64);
        print_menu();

        int option;
        scanf("%d", &option);
        fgetc(stdin); // Discard the newline character

        if (option != 0)
            printf("\033[H\033[J");

        switch (option) {
        case 1:
            printf("Listing containers...\n");
            struct lxc_container **containers;
            char **container_names;
            int num_containers;

            num_containers = list_all_containers(NULL, &container_names, &containers);

            if (num_containers < 0) {
                // fprintf(stderr, "Failed to list containers\n");
                fprintf(stderr, "There are no containers created\n");
                clear_screen();
                break;
            }

            printf("Number of containers: %d\n", num_containers);
            for (int i = 0; i < num_containers; i++) {
                printf("%s\n", container_names[i]);
            }

            free(containers);
            free(container_names);
            clear_screen();
            break;

        case 2:
            printf("Write the name of the container to create: ");
            fgets(container_name, 64, stdin);
            container_name[strcspn(container_name, "\n")] = 0;

            c = lxc_container_new(container_name, NULL);
            if (!c) {
                fprintf(stderr, "Failed to setup lxc_container struct\n");
                clear_screen();
                break;
            }

            if (c->is_defined(c)) {
                fprintf(stderr, "Container already '%s' exists\n", container_name);
                clear_screen();
                break;
            }

            printf("Creating container '%s'...\n", container_name);

            if (!c->createl(c, "download", NULL, NULL, LXC_CREATE_QUIET,
                            "-d", "ubuntu", "-r", "bionic", "-a", "amd64", NULL)) {
                fprintf(stderr, "Failed to create container rootfs\n");
                clear_screen();
                break;
            }

            printf("Container '%s' created successfully\n", container_name);

            clear_screen();
            break;

        case 3:
            printf("Write the name of the container to delete: ");
            fgets(container_name, 64, stdin);
            container_name[strcspn(container_name, "\n")] = 0;

            c = lxc_container_new(container_name, NULL);

            if (!c) {
                fprintf(stderr, "Failed to setup lxc_container struct\n");
                clear_screen();
                break;
            }

            if (!c->is_defined(c)) {
                fprintf(stderr, "Container does '%s' not exist\n", container_name);
                clear_screen();
                break;
            }

            printf("Destroying container...\n");
            if (!c->destroy(c)) {
                fprintf(stderr, "Failed to destroy container '%s'.\n", container_name);
                clear_screen();
                break;
            }

            printf("Container '%s' destroyed successfully\n", container_name);
            clear_screen();
            break;
        case 4:
            printf("Write the name of the container you want to start: ");
            fgets(container_name, 64, stdin);
            container_name[strcspn(container_name, "\n")] = 0;

            c = lxc_container_new(container_name, NULL);

            if (!c) {
                fprintf(stderr, "Failed to setup lxc_container struct\n");
                clear_screen();
                break;
            }

            if (!c->is_defined(c)) {
                fprintf(stderr, "Container '%s' does not exist\n", container_name);
                clear_screen();
                break;
            }

            printf("Starting container '%s' ...\n", container_name);
            /* Start the container */
            if (!c->start(c, 0, NULL)) {
                fprintf(stderr, "Failed to start the container '%s' \n", container_name);
                clear_screen();
                break;
            }

            printf("Container '%s' started successfully\n", container_name);

            printf("Container state: %s\n", c->state(c));
            printf("Container PID: %d\n", c->init_pid(c));

            clear_screen();
            break;
        case 5:
            printf("Write the name of the container you want to stop: ");
            fgets(container_name, 64, stdin);
            container_name[strcspn(container_name, "\n")] = 0;

            c = lxc_container_new(container_name, NULL);

            if (!c) {
                fprintf(stderr, "Failed to setup lxc_container struct\n");
                clear_screen();
                break;
            }

            if (!c->is_defined(c)) {
                fprintf(stderr, "Container does '%s' not exist\n", container_name);
                clear_screen();
                break;
            }

            printf("Stopping container '%s' ...\n", container_name);

            if (!c->shutdown(c, 30)) {
                // FIXME:
                printf("Failed to cleanly shutdown container '%s', forcing.\n", container_name);
                // sleep(10);
                if (!c->stop(c)) {
                    fprintf(stderr, "Failed to kill the container '%s'.\n", container_name);
                    clear_screen();
                    break;
                }
            }

            printf("Container '%s' stopped successfully\n", container_name);

            clear_screen();
            break;
        case 6:
            printf("Executing command in container...\n");

            clear_screen();
            break;
        case 7:
            printf("Establishing console connection with container...\n");

            clear_screen();
            break;
        case 0:
            printf("Exiting...\n");
            finish = 1;
            break;
        default:
            printf("Invalid option\n");
            clear_screen();
            break;
        }
    }

    return 0;
}
