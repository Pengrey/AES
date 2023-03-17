#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

static char child_stack[1048576];

int child()
{
    printf( "Child network namespace\n" );
    system( "ip link" );

    return 0;
}

int main()
{
    int status;

    printf( "Parent network namespace\n" );
    system( "ip link" );

    clone( child, child_stack + sizeof(child_stack), CLONE_NEWUSER | CLONE_NEWNET, 0 );

    wait( &status );

    return 0;
}
