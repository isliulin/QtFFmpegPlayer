#include "DrawNV12.h"



//ƬԪshader NV12
char *NV12_fragment = GET_STR(

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
	//���������ɫ
	gl_FragColor = vec4(rgb, 1.0);
}
);

DrawNV12::DrawNV12(const char* url, int width, int height)
	: DrawYUVBase(url, width, height) {}


DrawNV12::~DrawNV12()
{
}

void DrawNV12::InitializeCanvas()
{

	if (!fp)
	{
		qDebug() << "open failed";
		return;
	}
	resize(width, height);

	datas[0] = new unsigned char[width * height];
	datas[1] = new unsigned char[width * height / 2];

	//��������
	glGenTextures(2, texs);

	glBindTexture(GL_TEXTURE_2D, texs[0]);
	//�Ŵ���С�����������Բ�ֵ  GL_NEAREST(Ч�ʸߣ�������������)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//�������ʿռ�(�Կ��ռ䣩
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texs[1]);
	//�Ŵ���С�����������Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//�������ʿռ�(�Կ��ռ䣩
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width / 2, height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);

}

void DrawNV12::paintGL()
{

	glActiveTexture(GL_TEXTURE0);
	//�󶨵�����
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[0]);
	//�޸Ĳ�������
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, datas[0]);
	//��shader��������
	glUniform1i(unis[0], 0);

	glActiveTexture(GL_TEXTURE1);
	//�󶨵�����
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[1]);
	//�޸Ĳ�������
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, datas[1]);
	//��shader��������
	glUniform1i(unis[1], 1);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	isDrawing = false;
}

void DrawNV12::initializeGL()
{
	initializeOpenGLFunctions();
	qDebug() << "initializeGL";
	//����shader�ű�
	program.addShaderFromSourceCode(QGLShader::Vertex, vertexFrag);
	program.addShaderFromSourceCode(QGLShader::Fragment, NV12_fragment);

	//���ö�������ı���
	program.bindAttributeLocation("vertexIn", A_VER);
	//���ò�������
	program.bindAttributeLocation("textureIn", T_VER);

	program.link();
	program.bind();

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
	//����
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, false, 0, ver);
	glEnableVertexAttribArray(A_VER);

	//����
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, false, 0, tex);
	glEnableVertexAttribArray(T_VER);


	//��shader��ȡ����
	unis[0] = program.uniformLocation("tex_y");
	unis[1] = program.uniformLocation("tex_uv");

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void DrawNV12::resizeGL(int w, int h)
{

}

void DrawNV12::run()
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
		}
		fread(datas[0], 1, width * height, fp);
		fread(datas[1], 1, width * height / 2, fp);

		update();
		isDrawing = true;
		QThread::msleep(1);
	}
}

