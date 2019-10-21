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
	//刷新显示
	void paintGL();

	//初始化GL
	void initializeGL();

	//窗口尺寸变化
	void resizeGL(int w, int h);

private:
	//shader 
	QGLShaderProgram program;

	//从shader获取中的yuv变量地址
	GLuint unis[3] = { 0 };


	int width = 1920;
	int height = 1080;

	GLuint texs = 0;

	unsigned char* texRawData[3] = { 0 };
};

