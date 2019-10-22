#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QThread>
#include <QMutex>
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

	void closeEvent(QCloseEvent *event);

private:
	QMutex mutex;

	unsigned int CreateShader();

	//从shader获取中的yuv变量地址
	GLuint unis[3] = { 0 };
	//opengltexture地址
	GLuint texs[3] = { 0 };

	int width = 1920;
	int height = 1080;

	unsigned char* texRawData[4] = { 0 };

	void run();

	bool isExit = false;

	int updateTime = 0;
};

