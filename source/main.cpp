#include <iostream>
#include <HomeThermo.h>

#define CONFIGPATH "../config/config"

int main()
{
    HomeThermo thermo(CONFIGPATH);
    thermo.begin();
    return 0;
}