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

	//���ܳɹ���� ���ͷ�frame�ռ�
	void Repaint(AVFrame *frame);

	//����YUV����
	void Repaint(unsigned char* yuv[]);

	bool isExit = false;

protected:
	//ˢ����ʾ
	void paintGL();

	//��ʼ��GL
	void initializeGL();

	//���ڳߴ�仯
	void resizeGL(int w, int h);


private:
	QMutex mutex;
	//shader����
	QGLShaderProgram program;
	//��shader��ȡ�е�yuv������ַ
	GLuint unis[3] = { 0 };
	//opengltexture��ַ
	GLuint texs[3] = { 0 };

	int width = 1920;
	int height = 1080;

	//YUV����
	unsigned char* datas[3] = { 0 };

	int callUpdate = 0;
	int updateTime = 0;

	bool isRepainting = false;
	
};

