#include <XY_433.h>
using namespace std;

XY_433_error::XY_433_error (Error_type err, std::string msg) {
	error = err;
	error_message = msg;
}

XY_433_error::Error_type XY_433_error::getError() {
	return error;
}

std::string XY_433_error::toString() {
	return error_message;
}

XY_433_rec::XY_433_rec(int receiverPin) {
	static bool recOn;
	if(recOn) {
		throw XY_433_error(XY_433_error::RECEIVER_SETUP_ERR, string( "Only one receiver can be setup at one time."));
	}
	recOn = true;
	//defaults();
	XY_gpio io(XY_gpio::WIRINGPI);
	gpio = &io;
	try {
		mSetupRec(receiverPin);
	}
	catch(XY_433_error err) {
		throw err;
	}
}

void XY_433_rec::mSetupRec(int pin) {
	mReceiverPin = pin;
	gpio->setPinMode(mReceiverPin, XY_gpio::IN);
	//pullUpDnControl(mReceiverPin, PUD_UP);
	try {
		mSetupInterrupt();
	}
	catch(XY_433_error err) {
		throw err;
	}
}

void XY_433_rec::mSetupInterrupt() {
	//int ret = wiringPiISR(mReceiverPin, INT_EDGE_BOTH, interruptHandler);
	//if(ret > 0) {throw XY_433_error(XY_433_error::WIRINGPI_ERR, string( "Interrupt setup failed"));}
}

void XY_433_trans::defaults() {
	mTransOn = false;
	mRecOn = false;
	mInterruptOn = false;
	mPulse = 350;
	mLongPulse = 3;
	mShortPulse = 1;
	mRepeat = 10;
	mFinalHigh = 1;
	mFinalLow = 31;
	mMaxReceipt = MAX_RECEIPT;
	mTimeout = 4000;
}

XY_433_trans::XY_433_trans(int transmitterPin) {
	defaults();
	XY_gpio io(XY_gpio::WIRINGPI);
	gpio = &io;
	mSetupTrans(transmitterPin);
}

/*
void XY_433_rec::defaults() {
	mPulse = 350;
	mLongPulse = 3;
	mShortPulse = 1;
	mRepeat = 10;
	mFinalHigh = 1;
	mFinalLow = 31;
}
*/

void XY_433_trans::setRepeat(int repeatNumber) {
	mRepeat = repeatNumber;
}

int XY_433_trans::getRepeat() {
	return mRepeat;
}

void XY_433_trans::setPulseLength(int pulseLength) {
	mPulse = pulseLength;
}
int XY_433_trans::getPulseLength() {
	return mPulse;
}

void XY_433_trans::setShortPulse(int pulseMultiplier) {
	mShortPulse = pulseMultiplier;
}

void XY_433_trans::setLongPulse(int pulseMultiplier) {
	mLongPulse = pulseMultiplier;
}

int XY_433_trans::getShortPulse() {
	return mShortPulse;
}

int XY_433_trans::getLongPulse() {
	return mLongPulse;
}


void XY_433_trans::send(long long code, int bits) {
	int i, j, r;
	long long mask;
	for(r = 0; r < mRepeat; r++) {
		mask = 0x1;
		mask <<= bits - 1;
		for(i = 0; i < bits; i++) {
			if((code & mask)> 0) {
				gpio->digiPinWrite(mTransmitterPin, XY_gpio::GPIO_HIGH);
				gpio->microWait(mPulse * mLongPulse);
				gpio->digiPinWrite(mTransmitterPin, XY_gpio::GPIO_LOW);
				gpio->microWait(mPulse * mShortPulse);
			}
			else { 
				gpio->digiPinWrite(mTransmitterPin, XY_gpio::GPIO_HIGH);
				gpio->microWait(mPulse * mShortPulse);
				gpio->digiPinWrite(mTransmitterPin, XY_gpio::GPIO_LOW);
				gpio->microWait(mPulse * mLongPulse);
			}
			mask >>= 1;
		}
		gpio->digiPinWrite(mTransmitterPin, XY_gpio::GPIO_HIGH);
		gpio->microWait(mPulse * mFinalHigh);
		gpio->digiPinWrite(mTransmitterPin, XY_gpio::GPIO_LOW);
		gpio->microWait(mPulse * mFinalLow);
	}
}


void XY_433_trans::mSetupTrans(int pin) {
	mTransmitterPin = pin;
	gpio->setPinMode(mTransmitterPin, XY_gpio::OUT);
}
