#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
class VideoCanvas3 : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	VideoCanvas3(QWidget* parent = Q_NULLPTR);
	~VideoCanvas3();

protected:
	//ˢ����ʾ
	void paintGL();

	//��ʼ��GL
	void initializeGL();

	//���ڳߴ�仯
	void resizeGL(int w, int h);

private:
	//shader 
	QGLShaderProgram program;

	//��shader��ȡ�е�yuv������ַ
	GLuint unis[3] = { 0 };


	int width = 1920;
	int height = 1080;

	GLuint texs = 0;

	unsigned char* texRawData[3] = { 0 };
};

