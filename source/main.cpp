#include <iostream>
#include <HomeThermo.h>

#define CONFIGPATH "../config/config"

int main()
{
    HomeThermo thermo(CONFIGPATH, LOG_STD);
    thermo.begin();
    return 0;
}