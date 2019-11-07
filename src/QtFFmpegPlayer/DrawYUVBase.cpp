#include "DrawYUVBase.h"
#include <QDebug>



DrawYUVBase::DrawYUVBase(const char* url, int width, int height)
{
	this->width = width;
	this->height = height;
	fp = fopen(url, "rb");
}


DrawYUVBase::~DrawYUVBase()
{
	isExit = true;
	wait();
}



