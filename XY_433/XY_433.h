#include <XY_gpio.h>
#include <string>
#include <vector>

#define MAX_RECEIPT 67

class XY_433_error {
public:
	enum Error_type {WIRINGPI_ERR, RECEIVER_SETUP_ERR, INTERRUPT_ERR};
	XY_433_error(Error_type, std::string);
	XY_433_error::Error_type getError();
	std::string toString();
private:
	Error_type error;
	std::string error_message;
}; 

class XY_433_trans {
public:
	XY_433_trans(int);
	void setPulseLength(int);
	int getPulseLength();
	void setShortPulse(int);
	void setLongPulse(int);
	int getShortPulse();
	int getLongPulse();
	void setRepeat(int);
	int getRepeat();
	void send(long long, int);
private:
	int mTransmitterPin;
    bool mTransOn;
	bool mRecOn;
	bool mInterruptOn;
	int mPulse;
	int mLongPulse;
	int mShortPulse;
	int mRepeat;
	int mFinalHigh;
	int mFinalLow;
	void defaults();
	void mSetupTrans(int);
    int mMaxReceipt;
	int mTimeout;
	XY_gpio *gpio;
};

class XY_433_rec {
public:
	XY_433_rec(int);
private:
	XY_gpio *gpio;
	int mReceiverPin;
	//void defaults();
	void mSetupRec(int);
	void mSetupInterrupt();

};