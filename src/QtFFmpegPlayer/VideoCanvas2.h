#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>

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

private:

	unsigned int CreateShader();

	//��shader��ȡ�е�yuv������ַ
	GLuint unis[3] = { 0 };
	//opengltexture��ַ
	GLuint texs[3] = { 0 };

	int width = 0;
	int height = 0;

	unsigned char* texRawData[3] = { 0 };

};

