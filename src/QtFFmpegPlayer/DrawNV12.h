#pragma once
#include "DrawYUVBase.h"
class DrawNV12 : public DrawYUVBase
{

public:
	DrawNV12(const char* url, int width, int height);
	~DrawNV12();

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

