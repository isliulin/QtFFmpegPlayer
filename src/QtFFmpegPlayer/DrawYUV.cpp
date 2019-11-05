#include "DrawYUV.h"
#include <QDebug>

#define GET_STR(x) #x
#define A_VER 0
#define T_VER 1

DrawYUV::DrawYUV(FragmentType fragType, int width, int height, const char* url, QWidget* parent)
	: QOpenGLWidget(parent)
{
	crtFragmentType = fragType;
	this->width = width;
	this->height = height;
	fp = fopen(url, "rb");
}


DrawYUV::~DrawYUV()
{
	isExit = true;
	wait();
}

//顶点shader
const char* vertexFrag = GET_STR(
	attribute vec4 vertexIn;
attribute vec2 textureIn;
varying vec2 textureOut;

void main(void)
{
	gl_Position = vertexIn;
	textureOut = textureIn;
}
);
//片元shader I420
const char* I420_str = GET_STR(
	varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
void main(void)
{
	vec3 yuv;
	vec3 rgb;
	yuv.x = texture2D(tex_y, textureOut).r;
	yuv.y = texture2D(tex_u, textureOut).r - 0.5;
	yuv.z = texture2D(tex_v, textureOut).r - 0.5;
	rgb = mat3(1.0, 1.0, 1.0,
		0, -0.39465, 2.03211,
		1.13983, -0.58060, 0.0
	)* yuv;
	gl_FragColor = vec4(rgb, 1.0);
}
);
static const char *NV12_str = GET_STR(

	varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_uv;
void main() {
	vec3 yuv;
	vec3 rgb;
	yuv.r = texture2D(tex_y, textureOut).r;
	yuv.g = texture2D(tex_uv, textureOut).r - 0.5;
	yuv.b = texture2D(tex_uv, textureOut).a - 0.5;
	rgb = mat3(1.0, 1.0, 1.0,
		0.0, -0.39465, 2.03211,
		1.13983, -0.58060, 0.0)*yuv;
	//输出像素颜色
	gl_FragColor = vec4(rgb, 1.0);
}
);
static const char *NV21_str = GET_STR(

	varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_uv;
void main() {
	vec3 yuv;
	vec3 rgb;
	yuv.r = texture2D(tex_y, textureOut).r;
	yuv.g = texture2D(tex_uv, textureOut).a - 0.5;
	yuv.b = texture2D(tex_uv, textureOut).r - 0.5;
	rgb = mat3(1.0, 1.0, 1.0,
		0.0, -0.39465, 2.03211,
		1.13983, -0.58060, 0.0)*yuv;
	//输出像素颜色
	gl_FragColor = vec4(rgb, 1.0);
}
);


void DrawYUV::InitializeCanvas()
{

	if (!fp)
	{
		qDebug() << "open failed";
		return;
	}
	resize(width, height);

	if (crtFragmentType == FragmentType::I420)
	{
		datas[0] = new unsigned char[width * height];
		datas[1] = new unsigned char[width * height / 4];
		datas[2] = new unsigned char[width * height / 4];

		//创建材质
		glGenTextures(3, texs);

		glBindTexture(GL_TEXTURE_2D, texs[0]);
		//放大缩小过滤器，线性插值  GL_NEAREST(效率高，但马赛克严重)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//创建材质空间(显卡空间）
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, texs[1]);
		//放大缩小过滤器，线性插值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//创建材质空间(显卡空间）
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);


		glBindTexture(GL_TEXTURE_2D, texs[2]);
		//放大缩小过滤器，线性插值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//创建材质空间(显卡空间）
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
	}
	else if (crtFragmentType == NV12 || crtFragmentType == NV21)
	{
		datas[0] = new unsigned char[width * height];
		datas[1] = new unsigned char[width * height / 2];

		//创建材质
		glGenTextures(2, texs);

		glBindTexture(GL_TEXTURE_2D, texs[0]);
		//放大缩小过滤器，线性插值  GL_NEAREST(效率高，但马赛克严重)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//创建材质空间(显卡空间）
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, texs[1]);
		//放大缩小过滤器，线性插值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//创建材质空间(显卡空间）
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width / 2, height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
	}

}
void DrawYUV::run()
{
	while (!isExit)
	{
		if (isDrawing)
		{
			QThread::msleep(1);
			continue;
		}

		if (feof(fp) != 0)
		{
			fseek(fp, 0, SEEK_SET);
			qDebug() << "read end";
		}

		if (crtFragmentType == I420)
		{
			fread(datas[0], 1, width * height, fp);
			fread(datas[1], 1, width * height / 4, fp);
			fread(datas[2], 1, width * height / 4, fp);
		}
		else if (crtFragmentType == NV12 || crtFragmentType == NV21)
		{
			fread(datas[0], 1, width * height, fp);
			fread(datas[1], 1, width * height / 2, fp);
		}


		update();
		isDrawing = true;
		QThread::msleep(2);
	}

}
void DrawYUV::initializeGL()
{
	initializeOpenGLFunctions();
	qDebug() << "initializeGL";
	//加载shader脚本
	program.addShaderFromSourceCode(QGLShader::Vertex, vertexFrag);
	if (crtFragmentType == I420)
	{
		program.addShaderFromSourceCode(QGLShader::Fragment, I420_str);
	}
	else if (crtFragmentType == NV12)
	{
		program.addShaderFromSourceCode(QGLShader::Fragment, NV12_str);
	}
	else if (crtFragmentType == NV21)
	{
		program.addShaderFromSourceCode(QGLShader::Fragment, NV21_str);
	}

	//设置定点坐标的变量
	program.bindAttributeLocation("vertexIn", A_VER);
	//设置材质坐标
	program.bindAttributeLocation("textureIn", T_VER);

	program.link();
	program.bind();

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
	//顶点
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, false, 0, ver);
	glEnableVertexAttribArray(A_VER);

	//材质
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, false, 0, tex);
	glEnableVertexAttribArray(T_VER);


	//从shader获取材质
	if (crtFragmentType == I420)
	{
		unis[0] = program.uniformLocation("tex_y");
		unis[1] = program.uniformLocation("tex_u");
		unis[2] = program.uniformLocation("tex_v");
	}
	else if (crtFragmentType == NV12 || crtFragmentType == NV21)
	{
		unis[0] = program.uniformLocation("tex_y");
		unis[1] = program.uniformLocation("tex_uv");
	}

}


void DrawYUV::paintGL()
{

	if (crtFragmentType == FragmentType::I420)
	{
		glActiveTexture(GL_TEXTURE0);
		//绑定到材质
		glBindTexture(GL_TEXTURE_2D, texs[0]);
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[0]);
		//修改材质内容
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, datas[0]);
		//与shader变量关联
		glUniform1i(unis[0], 0);

		glActiveTexture(GL_TEXTURE1);
		//绑定到材质
		glBindTexture(GL_TEXTURE_2D, texs[1]);
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[1]);
		//修改材质内容
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, datas[1]);
		//与shader变量关联
		glUniform1i(unis[1], 1);

		glActiveTexture(GL_TEXTURE2);
		//绑定到材质
		glBindTexture(GL_TEXTURE_2D, texs[2]);
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[2]);
		//修改材质内容
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, datas[2]);
		//与shader变量关联
		glUniform1i(unis[2], 2);

	}
	else if (crtFragmentType == NV12 || crtFragmentType == NV21)
	{
		glActiveTexture(GL_TEXTURE0);
		//绑定到材质
		glBindTexture(GL_TEXTURE_2D, texs[0]);
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[0]);
		//修改材质内容
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, datas[0]);
		//与shader变量关联
		glUniform1i(unis[0], 0);

		glActiveTexture(GL_TEXTURE1);
		//绑定到材质
		glBindTexture(GL_TEXTURE_2D, texs[1]);
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[1]);
		//修改材质内容
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, datas[1]);
		//与shader变量关联
		glUniform1i(unis[1], 1);
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	isDrawing = false;
}


void DrawYUV::resizeGL(int w, int h)
{

}


