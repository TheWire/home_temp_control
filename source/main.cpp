#include <iostream>
#include <HomeThermo.h>

#define CONFIGPATH "../config/config"

int main()
{
    HomeThermo thermo(CONFIGPATH, LOG_DEV);
    thermo.begin();
    return 0;
}