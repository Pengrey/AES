#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>

static char child_stack[1048576];

void uts_name()
{
    struct utsname name;

    uname( &name );
    printf( "Host name: %s\n", name.nodename );
}

int child()
{
    printf( "---- Child UTS names\n" );
    uts_name();
    sethostname( "AES_host", 8 );
    uts_name();

    return 0;
}

int main()
{
    int status;

    printf( "---- Parent UTS names\n" );
    uts_name();

    clone( child, child_stack + sizeof(child_stack), CLONE_NEWUSER | CLONE_NEWUTS, 0 );

    sleep( 1 );

    wait( &status );

    printf( "---- Parent UTS names\n" );
    uts_name();

    return 0;
}
