#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static char child_stack[1048576];

int child()
{
    for (;;) {
        printf( "Child: PID = %d, EUID = %d, RUID = %d\n", getpid(), geteuid(), getuid() );
        sleep( 1 );
    }

    return 0;
}

int main()
{
    int child_pid, status;

    printf( "Parent: EUID = %d, RUID = %d\n", geteuid(), getuid() );

    child_pid = clone( child, child_stack + sizeof(child_stack), CLONE_NEWUSER, 0 );

    do {
        waitpid( child_pid, &status, 0 );
    } while (!WIFEXITED(status));

    return 0;
}
