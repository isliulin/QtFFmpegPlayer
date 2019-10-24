#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QMutex>
#include <QWaitCondition>
struct AVFrame;
struct AVCodecParameters;
class VideoCanvas : public QOpenGLWidget, protected QOpenGLFunctions
{

	Q_OBJECT

public:
	VideoCanvas(QWidget* parent = Q_NULLPTR);
	~VideoCanvas();

	void Init(int width, int height);

	//不管成功与否， 都释放frame空间
	void Repaint(AVFrame *frame);

	//绘制YUV数据
	void Repaint(unsigned char* yuv[]);

	bool isExit = false;

protected:
	//刷新显示
	void paintGL();

	//初始化GL
	void initializeGL();

	//窗口尺寸变化
	void resizeGL(int w, int h);


private:
	QMutex mutex;
	//shader程序
	QGLShaderProgram program;
	//从shader获取中的yuv变量地址
	GLuint unis[3] = { 0 };
	//opengltexture地址
	GLuint texs[3] = { 0 };

	int width = 1920;
	int height = 1080;

	//YUV数据
	unsigned char* datas[3] = { 0 };

	int callUpdate = 0;
	int updateTime = 0;

	bool isRepainting = false;
	
};

