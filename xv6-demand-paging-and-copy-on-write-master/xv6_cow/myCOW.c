#include "types.h"
#include "user.h"
#include "date.h"

int arrGlobal[10000];

int main(int argc, char *argv[])
{
    if (pgtPrint())
    {
        printf(2, "Access failed\n");
        exit();
    }
    printf(2, "-----------------------------------Before fork-----------------------------------\n");
    int pid = fork();

    if (pid == 0)
    {
        if (pgtPrint())
        {
            printf(2, "Access failed\n");
            exit();
        }
        printf(2, "-------------------------------In child process----------------------------------\n");
        arrGlobal[6000] = 10;
        if (pgtPrint())
        {
            printf(2, "Access failed\n");
            exit();
        }
        printf(2, "------------------In child process accessing arrGlobal[6000]---------------------\n");
        exit();
    }
    if (pid > 0)
    {
        wait();
        arrGlobal[2000] = 10;

        if (pgtPrint())
        {
            printf(2, "Access failed\n");
            exit();
        }
        printf(2, "---------------------------------In parent process--------------------------------\n");

        exit();
    }
    return 0;
}
