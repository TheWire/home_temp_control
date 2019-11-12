#include <XY_gpio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

XY_gpio::XY_gpio(PinMap map)
{
    if(map == BCM)
    {
        system("gpio -g");
    }
    else if(map == WIRINGPI)
    {
        //do nothing
    }
    else if(map == PHYS)
    {
        system("gpio -1");
    }
    else if(map == SYS)
    {
        system("gpio -g");
    }
    else
    {
        //donothing
    }
    
}

void XY_gpio::setPinMode(int pin, PinMode mode)
{
    char command[23];
    char _mode[10];
    if(mode == IN)
    {
        sprintf(_mode, "in");
    }
    else if(mode == OUT)
    {
        sprintf(_mode, "out");
    }
    else
    {
        sprintf(_mode, "out"); //placeholder
    }
    sprintf(command, "gpio mode %d %s", pin, _mode);
    system(command);
}

void XY_gpio::digiPinWrite(int pin, PinState state)
{
    int value;
    char command[16];
    if(state == GPIO_LOW)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }
    sprintf(command, "gpio write %d %d", pin, value);
    system(command);
}

void XY_gpio::microWait(int uSeconds)
{
    usleep(uSeconds);
}