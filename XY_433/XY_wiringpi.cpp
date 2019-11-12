#include <XY_gpio.h>
#include <wiringPi.h>

XY_gpio::XY_gpio(PinMap map)
{
    if(map == BCM)
    {
        wiringPiSetupGpio();
    }
    else if(map == WIRINGPI)
    {
        wiringPiSetup();
    }
    else if(map == PHYS)
    {
        wiringPiSetupPhys();
    }
    else if(map == SYS)
    {
        wiringPiSetupSys();
    }
    else
    {
        wiringPiSetup();
    }
    
}

void XY_gpio::setPinMode(int pin, PinMode)
{
    if(PinMode == OUT)
    {
        return pinMode(pin, OUTPUT);
    }
    else 
    {
        return pinMode(pin, INPUT)
    }
}

void XY_gpio::digiPinWrite(int pin, PinState state)
{
    int value;
    if(state == GPIO_LOW)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }
    digitalWrite(pin, value);
}

void XY_gpio::microWait(int uSeconds)
{
    delayMicroseconds(uSeconds);
}