#include <temp.h>

TimeTemp::TimeTemp(int time, float temp, bool isSingle)
: _time(time), _temp(temp), _single(isSingle)
{
}

int TimeTemp::getTime() const
{
	return _time;
}

float TimeTemp::getTemp() const
{
	return _temp;
}

bool TimeTemp::operator< (const TimeTemp &a) const
{
	if(_time < a.getTime())
	{
		return true;
	}
	else
	{
		return false;
	}
}