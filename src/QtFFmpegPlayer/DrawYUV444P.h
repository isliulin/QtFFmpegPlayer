#pragma once
#include "DrawYUVBase.h"
class DrawYUV444P : public DrawYUVBase
{
public:
	DrawYUV444P(const char* url, int width, int height);
	~DrawYUV444P();

	virtual void InitializeCanvas();

protected:

	//刷新显示
	void paintGL();

	//初始化GL
	void initializeGL();

	//窗口尺寸变化
	void resizeGL(int w, int h);

	void run();
};

