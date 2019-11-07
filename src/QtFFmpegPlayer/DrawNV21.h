#pragma once
#include "DrawYUVBase.h"
class DrawNV21 : public DrawYUVBase
{
public:
	DrawNV21(const char* url, int width, int height);
	~DrawNV21();

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

