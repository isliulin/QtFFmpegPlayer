#include "VideoCanvas3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define GET_STR(x) #x

//����shader
const char* vertexCode = GET_STR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void)
	{
		gl_Position = vertexIn;
	}
);
//ƬԪshader
const char* fragmentCode = GET_STR(
	uniform sampler2D tex1;
	varying vec2 textureOut;
	void main(void)
	{
		vec3 rgb = texture2D(tex1, textureOut);
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
	qDebug() << "initializeGL";
	initializeOpenGLFunctions();

	qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vertexCode);
	qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, fragmentCode);
	//���ö�������
	program.bindAttributeLocation("vertexIn", 0);
	qDebug() << program.link();
	qDebug() << program.bind();

	//���ݶ���Ͳ�������
	//����
	static const GLfloat ver[] = {
		-1.0f,	-1.0f,
		1.0f,	-1.0f,
		-1.0f,	1.0f,
		1.0f,	1.0f
	};
	//��������
	static const GLfloat tex[] = {
		0.0f,	1.0f,
		1.0f,	1.0f,
		0.0f,	0.0f,
		1.0f,	0.0f
	};
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, ver);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, tex);
	glEnableVertexAttribArray(1);

	unis[0] = program.uniformLocation("tex1");
	
	glGenTextures(3, &texs);
	//Y
	glBindTexture(GL_TEXTURE_2D, texs);
	//�Ŵ���С�����������Բ�ֵ  GL_NEAREST(Ч�ʸߣ�������������)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//�������ʿռ�(�Կ��ռ䣩
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	int width, height, nrChannels;
	texRawData[0] = stbi_load("F:/Http/tex/tex2.jpg", &width, &height, &nrChannels, 0);
	texRawData[1] = stbi_load("F:/Http/tex/tex3.jpg", &width, &height, &nrChannels, 0);
	texRawData[2] = stbi_load("F:/Http/tex/tex4.jpg", &width, &height, &nrChannels, 0);
}

void VideoCanvas3::paintGL()
{
	glActiveTexture(GL_TEXTURE0);
	//�󶨵�����
	glBindTexture(GL_TEXTURE_2D, texs);
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[0]);
	//�޸Ĳ�������
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, texRawData[0]);
	//��shader��������
	glUniform1i(unis[0], 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



void VideoCanvas3::resizeGL(int w, int h)
{

}
