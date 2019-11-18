#include <iostream>
#include <HomeThermo.h>

using namespace std;

#define CONFIGPATH "../config/conf"

int main()
{
    HomeThermo thermo(CONFIGPATH, LOG_DEV);
    thermo.begin();
    return 0;
}