#ifndef ZINTMAIN__
#define ZINTMAIN__

#include <stdio.h>
#include <math.h>

int main() {
    printf("%lld\n", (long long int)(0x7FFFFFFF));
    printf("%lld\n", (long long int)(0x7FFFFFFF) * (long long int)(0x7FFFFFFF));

    printf("%lld\n", (long long int)(__INT32_MAX__) * (long long int)(__INT32_MAX__));
    printf("%lld\n", __LONG_LONG_MAX__);
}

#endif
