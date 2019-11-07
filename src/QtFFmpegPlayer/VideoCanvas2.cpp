#include "VideoCanvas2.h"
#include "PlayerUtility.h"

#include <QtConcurrent/QtConcurrent>
#include <QMutexLocker>
#define GET_STR(x) #x

float vertices[] = {
	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 左上
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
};

const char* vertexShaderSource = GET_STR(
	attribute vec4 aPos;
	attribute vec3 aColor;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	varying vec3 colorOut;
	void main() {
		textureOut = textureIn;
		colorOut = aColor;
		gl_Position = aPos;
	}
);

const char* fragmentShaderSource = GET_STR(
	uniform sampler2D tex1;
	//uniform sampler2D tex2;
	//uniform sampler2D tex3;
	varying vec2 textureOut;
	varying vec3 colorOut;
	void main() {
		vec3 rgb = texture2D(tex1, textureOut);
		gl_FragColor = vec4(rgb, 1.0);
	}
);

VideoCanvas2::VideoCanvas2(QWidget* parent)
	: QOpenGLWidget(parent)
{
}
VideoCanvas2::~VideoCanvas2() {}


void VideoCanvas2::initializeGL()
{
	initializeOpenGLFunctions();
	
	qDebug() << "initializeOpenGLFunctions2";

	glClearColor(0.2, 0, 0, 1);

	GLuint shaderProgram = CreateShader();


	//创建顶点数组，把数据都传递给GPU
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
	//顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的
	glEnableVertexAttribArray(0);

	
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	
	unis[0] = glGetUniformLocation(shaderProgram, "tex1") ;
	//unis[1] = glGetUniformLocation(shaderProgram, "tex2");
	//unis[2] = glGetUniformLocation(shaderProgram, "tex3");

	int width, height, nrChannels;


	glGenTextures(3, texs);
	//创建一个图像缓冲对象并获取ID
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	//设置texturede过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glGenerateMipmap(GL_TEXTURE_2D);
	QtConcurrent::run(this, &VideoCanvas2::run);
}

void VideoCanvas2::paintGL()
{


	glActiveTexture(GL_TEXTURE0);
	//绑定到材质
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, texRawData[PlayerUtility::Get()->Random(2)]);
	//与shader变量关联
	glUniform1i(unis[0], 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	

	updateTime++;

	
}


void VideoCanvas2::resizeGL(int w, int h)
{

}

void VideoCanvas2::closeEvent(QCloseEvent *event)
{
	isExit = true;
	
}

unsigned int VideoCanvas2::CreateShader()
{
	
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << infoLog;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << infoLog;
	}

	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		qDebug() << "Link Error:" << infoLog;
	}
	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

void VideoCanvas2::run()
{
	int time = 0;
	
	while (!isExit)
	{

		QThread::msleep(1);
		update();
		time++;
		if (time % 100 == 0)
		{
			qDebug() << "thread run:" << time << "update time:" << updateTime;
		}

	}
}
