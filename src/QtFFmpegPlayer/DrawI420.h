#pragma once
#include "DrawYUVBase.h"
class DrawI420 : public DrawYUVBase
{
public:
	DrawI420(const char* url, int width, int height);
	~DrawI420();

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

