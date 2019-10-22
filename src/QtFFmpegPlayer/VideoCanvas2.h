#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QThread>
#include <QMutex>
//////////////////////////////////////////////////////////////////////////
//ʹ��ԭʼ��������
class VideoCanvas2 : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	VideoCanvas2(QWidget* parent = Q_NULLPTR);
	~VideoCanvas2();

protected:
	//ˢ����ʾ
	void paintGL();

	//��ʼ��GL
	void initializeGL();

	//���ڳߴ�仯
	void resizeGL(int w, int h);

	void closeEvent(QCloseEvent *event);

private:
	QMutex mutex;

	unsigned int CreateShader();

	//��shader��ȡ�е�yuv������ַ
	GLuint unis[3] = { 0 };
	//opengltexture��ַ
	GLuint texs[3] = { 0 };

	int width = 1920;
	int height = 1080;

	unsigned char* texRawData[4] = { 0 };

	void run();

	bool isExit = false;

	int updateTime = 0;
};

