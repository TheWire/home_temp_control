class TimeTemp
{
	public:
		TimeTemp(int, float, bool);
		int getTime() const;
		float getTemp() const;
		bool isSingle();
		bool operator< (const TimeTemp&) const;
	private:
		int _time;
		float _temp;
		bool _single;
};