#include "types.h"
#include "user.h"
#include "date.h"

int arrGlobal[10000];

int main(int argc, char *argv[])
{
    // int arrLocal[10000];
    // arrLocal[0] = 1;
    // printf(2, "%d\n", arrLocal[0]);

    if (pgtPrint())
    {
        printf(2, "Access failed\n");
        exit();
    }
    printf(2, "Done\n");

    exit();
}
