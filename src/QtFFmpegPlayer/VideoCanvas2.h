#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>

//////////////////////////////////////////////////////////////////////////
//使用原始方法绘制
class VideoCanvas2 : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	VideoCanvas2(QWidget* parent = Q_NULLPTR);
	~VideoCanvas2();

protected:
	//刷新显示
	void paintGL();

	//初始化GL
	void initializeGL();

	//窗口尺寸变化
	void resizeGL(int w, int h);

private:

	unsigned int CreateShader();

	//从shader获取中的yuv变量地址
	GLuint unis[3] = { 0 };
	//opengltexture地址
	GLuint texs[3] = { 0 };

	int width = 0;
	int height = 0;

	unsigned char* texRawData[3] = { 0 };

};

