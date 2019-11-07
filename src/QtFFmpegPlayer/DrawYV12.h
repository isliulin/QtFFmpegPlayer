#pragma once
#include "DrawYUVBase.h"
class DrawYV12 : public DrawYUVBase
{
public:
	DrawYV12(const char* url, int width, int height);
	~DrawYV12();

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

