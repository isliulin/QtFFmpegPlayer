#include "VideoCanvas3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define GET_STR(x) #x
#include <QTime>
#include "PlayerUtility.h"
#include <QtConcurrent/QtConcurrent>
#define A_VER 3
#define T_VER 4
//顶点shader
const char* vertexCode = GET_STR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void)
	{
		textureOut = textureIn;
		gl_Position = vertexIn;
	}
);
//片元shader
const char* fragmentCode = GET_STR(
	uniform sampler2D tex1;
	uniform sampler2D tex2;
	varying vec2 textureOut;
	void main(void)
	{
		vec3 rgb = mix(texture2D(tex1, textureOut), texture2D(tex2, textureOut), 0.5);
		//mix(texture(tex1, TexCoord), texture(texture2, TexCoord), 0.2);
		//rgb = texture2D(tex2, textureOut);
		gl_FragColor = vec4(rgb, 1.0);
	}
);

VideoCanvas3::VideoCanvas3(QWidget* parent)
	: QOpenGLWidget(parent)
{
}


VideoCanvas3::~VideoCanvas3()
{
}

void VideoCanvas3::initializeGL()
{
	qDebug() << "initializeGL3";
	initializeOpenGLFunctions();

	qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vertexCode);
	qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, fragmentCode);
	//设置顶点数据
	program.bindAttributeLocation("vertexIn", A_VER);
	program.bindAttributeLocation("textureIn", T_VER);
	qDebug() << program.link();
	qDebug() << program.bind();

	//传递定点和材质坐标
	//顶点
	static const GLfloat ver[] = {
		-1.0f,	-1.0f,
		1.0f,	-1.0f,
		-1.0f,	1.0f,
		1.0f,	1.0f
	};
	//材质坐标
	static const GLfloat tex[] = {
		0.0f,	1.0f,
		1.0f,	1.0f,
		0.0f,	0.0f,
		1.0f,	0.0f
	};
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, GL_FALSE, 0, ver);
	glEnableVertexAttribArray(A_VER);

	glVertexAttribPointer(T_VER, 2, GL_FLOAT, GL_FALSE, 0, tex);
	glEnableVertexAttribArray(T_VER);

	unis[0] = program.uniformLocation("tex1");
	unis[1] = program.uniformLocation("tex2");
	
	glGenTextures(3, texs);
	//Y
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	//放大缩小过滤器，线性插值  GL_NEAREST(效率高，但马赛克严重)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//创建材质空间(显卡空间）
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	//Y
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	//放大缩小过滤器，线性插值  GL_NEAREST(效率高，但马赛克严重)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//创建材质空间(显卡空间）
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	//获取图片RGB数据
	int width, height, nrChannels;
	texRawData[0] = stbi_load("Image/tex2.jpg", &width, &height, &nrChannels, 0);
	texRawData[1] = stbi_load("Image/tex3.jpg", &width, &height, &nrChannels, 0);
	texRawData[2] = stbi_load("Image/tex4.jpg", &width, &height, &nrChannels, 0);
	
	QtConcurrent::run(this, &VideoCanvas3::run);
}

void VideoCanvas3::paintGL()
{
	glActiveTexture(GL_TEXTURE0);
	//绑定到材质
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[0]);
	//修改材质内容

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, texRawData[PlayerUtility::Get()->Random(2)]);
	//与shader变量关联
	glUniform1i(unis[0], 0);


	glActiveTexture(GL_TEXTURE1);
	//绑定到材质
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[0]);
	//修改材质内容

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, texRawData[PlayerUtility::Get()->Random(2)]);
	//与shader变量关联
	glUniform1i(unis[1], 1);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	updateTime++;
}



void VideoCanvas3::resizeGL(int w, int h)
{

}

void VideoCanvas3::run()
{
	int time = 0;
	while (true)
	{

		QThread::msleep(20);
		update();
		time++;
		if (time % 100 == 0)
		{
			qDebug() << "thread run:" << time << "update time:" << updateTime;
		}
	}
}
