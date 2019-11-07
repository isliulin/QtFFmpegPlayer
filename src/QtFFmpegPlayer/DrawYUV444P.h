#pragma once
#include "DrawYUVBase.h"
class DrawYUV444P : public DrawYUVBase
{
public:
	DrawYUV444P(const char* url, int width, int height);
	~DrawYUV444P();

	virtual void InitializeCanvas();

protected:

	//ˢ����ʾ
	void paintGL();

	//��ʼ��GL
	void initializeGL();

	//���ڳߴ�仯
	void resizeGL(int w, int h);

	void run();
};

