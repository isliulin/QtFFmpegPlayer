#pragma once
#include <sys/timeb.h>
extern "C"
{
#include <libavutil/rational.h>
}
class AudioPlay;
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

	//获取时间戳ms 
	inline long long GetNowMs()
	{
		struct timeb t;
		ftime(&t);
		return 1000 * t.time + t.millitm;
	}

	//取值范围为[0, maxvalue]
	int Random(int maxvalue);

	long long audioPts;

	long long justWritePts;
	AudioPlay* ap;

	int GetCurrentAudioPTS();

	bool isPause = false;

	bool isRunAudioTestThread = false;

	class UDPReceiver* udp = NULL;

private:
	PlayerUtility();

	
};

