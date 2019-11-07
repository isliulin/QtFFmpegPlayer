#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QThread>
#define GET_STR(x) #x
#define A_VER 0
#define T_VER 1
enum FragmentType
{
	I420,
	YV12,
	NV12,
	NV21,
	YUV444P,
	I422
};
class DrawYUVBase : public QOpenGLWidget, public QOpenGLFunctions, public QThread
{
public:
	DrawYUVBase(const char* url, int width, int height);
	virtual ~DrawYUVBase();

	virtual void InitializeCanvas() = 0;

protected:

	//顶点shader
	char* vertexFrag = GET_STR(
		attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;

	void main(void)
	{
		gl_Position = vertexIn;
		textureOut = textureIn;
	}
	);

	//shader程序
	QGLShaderProgram program;
	//从shader获取中的yuv变量地址
	GLuint unis[3] = { 0 };

	int width = 0;
	int height = 0;

	unsigned char* datas[8] = { 0 };

	//opengltexture地址
	GLuint texs[3] = { 0 };

	bool isDrawing = false;

	FILE *fp = NULL;

	bool isExit = false;

};

