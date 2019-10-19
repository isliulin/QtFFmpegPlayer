#pragma once
extern "C"
{
#include <libavutil/rational.h>
}

class PlayerUtility
{
public:
	
	~PlayerUtility();

	inline static PlayerUtility *Get()
	{
		static PlayerUtility pu;
		return &pu;
	}

	inline static double r2d(AVRational r)
	{
		return r.den == 0 ? 0 : (double)r.num / (double)r.den;
	}

	void av_strerror2(int errnum, const char* log = nullptr);
private:
	PlayerUtility();
};

