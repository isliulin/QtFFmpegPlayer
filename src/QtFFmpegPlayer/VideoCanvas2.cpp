#include "VideoCanvas2.h"

#include "stb_image.h"
#define GET_STR(x) #x
float vertices[] = {
	//顶点			//材质
	-1.0f, -1.0f,	0.0f,	1.0f,
	 1.0f, -1.0f,	1.0f,	1.0f,
	 -1.0f,  1.0f,	0.0f,	0.0f,
	 1.0f,	1.0f,	1.0f,	0.0f
};

const char* vertexShaderSource = GET_STR(
	attribute vec4 aPos;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main() {
		textureOut = textureIn;
		gl_Position = aPos;
	}
);

const char* fragmentShaderSource = GET_STR(
	uniform sampler2D tex1;
	uniform sampler2D tex2;
	uniform sampler2D tex3;
	varying vec2 textureOut;
	void main() {
		vec3 rgb = texture2D(tex1, textureOut);
		gl_FragColor = vec4(rgb, 1.0);
	}
);

VideoCanvas2::VideoCanvas2(QWidget* parent) :QOpenGLWidget(parent) {}
VideoCanvas2::~VideoCanvas2() {}


void VideoCanvas2::initializeGL()
{
	initializeOpenGLFunctions();
	qDebug() << "initializeOpenGLFunctions";

	//glClearColor(1, 0, 0, 1);

	unsigned char shaderProgram = CreateShader();

	//创建顶点数组，把数据都传递给GPU
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	unis[0] = glGetUniformLocation(shaderProgram, "tex1") ;
	unis[1] = glGetUniformLocation(shaderProgram, "tex2");
	unis[2] = glGetUniformLocation(shaderProgram, "tex3");

	int width, height, nrChannels;
	texRawData[0] = stbi_load("F:/Http/tex/tex2.jpg", &width, &height, &nrChannels, 0);
	texRawData[1] = stbi_load("F:/Http/tex/tex3.jpg", &width, &height, &nrChannels, 0);
	texRawData[2] = stbi_load("F:/Http/tex/tex4.jpg", &width, &height, &nrChannels, 0);

	//创建材质
	glGenTextures(3, texs);
	for (int i = 0; i < 3; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texs[i]);
		//放大缩小过滤器，线性插值  GL_NEAREST(效率高，但马赛克严重)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//创建材质空间(显卡空间）
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texRawData[1]);
	}

	//glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize[0]);
	//修改材质内容
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, texRawData[0]);
	//与shader变量关联
	glUniform1i(unis[0], 0);

}

void VideoCanvas2::paintGL()
{


	//绑定到材质
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	


	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}


void VideoCanvas2::resizeGL(int w, int h)
{

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

	unsigned int shaderProgram;
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
