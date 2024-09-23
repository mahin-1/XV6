#include "types.h"
#include "user.h"
#include "date.h"

int arrGlobal[10000];

int main(int argc, char *argv[])
{
    // int arrLocal[10000];
    // for (int i = 0; i < 10000; i++)
    // {
    //     arrLocal[i] = 1;
    //     arrLocal[0] = arrLocal[i];
    //     /* code */
    // }
    // printf(2, "%d\n", arrLocal[0]);

    if (pgtPrint())
    {
        printf(2, "Access failed\n");
        exit();
    }
    printf(2, "Done\n");
    printf(2, "%d\n", arrGlobal[9999]);
    if (pgtPrint())
    {
        printf(2, "Access failed\n");
        exit();
    }
    exit();
}
