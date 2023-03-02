// Create a C program that prints the effective and real UID of the current process, as well as its effective and real GIDs

#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("Effective UID: %d Real UID: %d Effective GID: %d Real GID: %d \n", geteuid(), getuid(), getegid(), getgid());
    return 0;
}