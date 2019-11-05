#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QThread>
enum FragmentType
{
	I420,
	NV12,
	NV21
};
class DrawYUV : public QOpenGLWidget, public QOpenGLFunctions, public QThread
{
public:
	DrawYUV(FragmentType fragType, int width, int height, const char* url, QWidget* parent = Q_NULLPTR);
	~DrawYUV();

	void InitializeCanvas();

	bool isExit = false;

protected:
	//ˢ����ʾ
	void paintGL();

	//��ʼ��GL
	void initializeGL();

	//���ڳߴ�仯
	void resizeGL(int w, int h);

	void run();


private:

	FragmentType crtFragmentType = FragmentType::NV12;

	//shader����
	QGLShaderProgram program;
	//��shader��ȡ�е�yuv������ַ
	GLuint unis[3] = { 0 };

	int width = 0;
	int height = 0;

	unsigned char* datas[8] = { 0 };

	//opengltexture��ַ
	GLuint texs[3] = { 0 };

	bool isDrawing = false;

	FILE *fp = NULL;

};

