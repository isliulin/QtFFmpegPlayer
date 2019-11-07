#pragma once
#include "DrawYUVBase.h"
class DrawI420 : public DrawYUVBase
{
public:
	DrawI420(const char* url, int width, int height);
	~DrawI420();

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

