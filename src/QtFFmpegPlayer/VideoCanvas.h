#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QMutex>
struct AVFrame;
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
	void Repaint2(unsigned char* yuv[]);
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

	unsigned char* datas[3] = { 0 };
};

