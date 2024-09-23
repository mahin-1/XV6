#include "types.h"
#include "user.h"
#include "date.h"
int main(int argc, char *argv[])
{
    struct rtcdate r;
    if (date(&r))
    {
        printf(2, "date failed\n");
        exit();
    }
    printf(2, "year: %d\nMonth: %d\nDate: %d\nHour: %d\nMinute: %d\nSecond: %d\n", r.year, r.month, r.day, r.hour, r.minute, r.second);
    // your code to print the time in any format you like...
    /* example output format should be like this:
          Year: 2016
       Month: 1 or January
       Date: 26
       Hour: 15
       Minute: 12
       Second: 11
    */
    exit();
}
