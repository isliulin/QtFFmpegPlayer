#pragma once
#include "DrawYUVBase.h"
#include "DrawI420.h"
#include "DrawYV12.h"
#include "DrawNV12.h"
#include "DrawNV21.h"
#include "DrawYUV444P.h"
#include "DrawI422.h"
class Factory
{
public:
	static DrawYUVBase* CreateCanvas(FragmentType fragType, const char* url, int width, int height)
	{
		if (fragType == I420) { return new DrawI420(url, width, height); }
		if (fragType == YV12) { return new DrawYV12(url, width, height); }
		if (fragType == NV12) { return new DrawNV12(url, width, height); }
		if (fragType == NV21) { return new DrawNV21(url, width, height); }
		if (fragType == YUV444P) { return new DrawYUV444P(url, width, height); }
		if (fragType == I422) { return new DrawI422(url, width, height); }
		return NULL;
	}
protected:
private:
};
