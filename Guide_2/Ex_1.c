#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t child;
    if (child = fork())
    { // father
        int status;
        int syscall_entering = 1;
        struct user_regs_struct user_regs;
        int options_set = 0;
        for (;;)
        {
            wait(&status);
            printf ( " Status = %d\n", status );
            if (WIFEXITED(status))
                return 0;
            if (options_set == 0)
            {
                ptrace ( PTRACE_SETOPTIONS , child , 0, 0 );
                options_set = 1;
            }
            if (syscall_entering == 1)
            {
                syscall_entering = 0;
                ptrace(PTRACE_GETREGS, child, 0, &user_regs);
                printf ( "[%d] Entering syscall % lld \n", child , user_regs . orig_rax );
                if (user_regs.orig_rax == SYS_openat)
                {
                    printf("[%d] Called open \n", child);
                }
                else if (user_regs.orig_rax == SYS_close)
                {
                    printf("[%d] Called close \n", child);
                }
            }
            else
            {
                syscall_entering = 1;
                printf ( "[%d] exiting syscall \n", child );
            }
            ptrace(PTRACE_SYSCALL, child, 0, 0);
        }
    }
    else
    { // child
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execvp(argv[1], argv + 1);
    }
    return 0;
}
