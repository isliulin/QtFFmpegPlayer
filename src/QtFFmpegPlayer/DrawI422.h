#pragma once
#include "DrawYUVBase.h"
class DrawI422 : public DrawYUVBase
{
public:
	DrawI422(const char* url, int width, int height);
	~DrawI422();

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

