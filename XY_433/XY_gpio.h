class XY_gpio
{
    public:

        enum PinMap
        {
            BCM,
            WIRINGPI,
            PHYS,
            SYS
        };

        enum PinMode
        {
            OUT,
            IN 
        };

        enum PinState
        {
            GPIO_LOW,
            GPIO_HIGH
        };

        XY_gpio(PinMap);
        void setPinMode(int, PinMode);
        void digiPinWrite(int, PinState);
        void microWait(int);
};