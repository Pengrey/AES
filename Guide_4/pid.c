#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

static char child_stack[1048576];

int child()
{
    printf( "Child: PID = %d, PPID = %d, EUID = %d\n", getpid(), getppid(), geteuid() );

    return 0;
}

int main()
{
    int child_pid, status;

    printf( "Parent: PID = %d, PPID = %d\n", getpid(), getppid() );

    child_pid = clone( child, child_stack + sizeof(child_stack), CLONE_NEWUSER | CLONE_NEWPID, 0 );

    printf( "Parent: child PID = %d (errno = %d)\n", child_pid, errno );

    waitpid( child_pid, &status, 0 );

    return 0;
}
