#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "shader.h"
#include "stb_image.h"

#define WIDTH  800
#define HEIGHT 600

// ==== Минимальные матричные функции ====
static void identity4(float* m) {
	for (int i = 0; i < 16; ++i) m[i] = 0.0f;
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

static void multiply4(float* r, const float* a, const float* b) {
	float tmp[16];
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col) {
			tmp[row * 4 + col] = 0.0f;
			for (int k = 0; k < 4; ++k) tmp[row * 4 + col] += a[row * 4 + k] * b[k * 4 + col];
		}
	for (int i = 0; i < 16; ++i) r[i] = tmp[i];
}
static void rotateY4(float* m, float angle) {
	float r[16];
	identity4(r);
	float c = cosf(angle), s = sinf(angle);
	r[0]  = c;
	r[2]  = s;
	r[8]  = -s;
	r[10] = c;
	multiply4(m, r, m);
}
static void perspective4(float* m, float fovy, float aspect, float znear, float zfar) {
	float f = 1.0f / tanf(fovy / 2.0f);
	identity4(m);
	m[0]  = f / aspect;
	m[5]  = f;
	m[10] = (zfar + znear) / (znear - zfar);
	m[11] = -1.0f;
	m[14] = (2.0f * zfar * znear) / (znear - zfar);
	m[15] = 0.0f;
}
static void translate4(float* m, float x, float y, float z) {
	float t[16];
	identity4(t);
	t[12] = x;
	t[13] = y;
	t[14] = z;
	multiply4(m, t, m);
}

const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec2 aTexCoord;\n"
                                 "uniform mat4 model;\n"
                                 "uniform mat4 view;\n"
                                 "uniform mat4 projection;\n"
                                 "out vec2 TexCoord;\n"
                                 "void main() {\n"
                                 "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                 "    TexCoord = aTexCoord;\n"
                                 "}\n";

const char* fragmentShaderSource = "#version 330 core\n"
                                   "in vec2 TexCoord;\n"
                                   "out vec4 FragColor;\n"
                                   "uniform sampler2D texture1;\n"
                                   "void main() {\n"
                                   "    FragColor = texture(texture1, TexCoord);\n"
                                   "}\n";

// Вершины куба: 36 вершин, каждая с позицией (x, y, z) и координатами текстуры (u, v)
float vertices[] = {
        // позиции                                  // tex coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,
        -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,
        -0.5f, 1.0f, 1.0f,0.5f,  0.5f,  -0.5f,
        1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f,
        1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,
        -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,
        0.5f,  1.0f, 1.0f,0.5f,  0.5f,  0.5f,
        1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f,
        1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f,
        0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f,
        -0.5f, 0.0f, 1.0f,-0.5f, -0.5f, -0.5f,
        0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f,
        0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,
        0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f,
        -0.5f, 0.0f, 1.0f,0.5f,  -0.5f, -0.5f,
        0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f,
        0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,
        -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f,
        0.5f,  1.0f, 0.0f,0.5f,  -0.5f, 0.5f,
        1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f,
        0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,
        0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,
        0.5f,  1.0f, 0.0f,0.5f,  0.5f,  0.5f,
        1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f,
        0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f,
};

int main() {
	// Инициализация GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rotating Cube", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// GLAD
	if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	// Компиляция шейдеров
	unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

	// VBO/VAO
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Загрузка текстуры
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int texWidth, texHeight, nrChannels;

	unsigned char* data = stbi_load("resources/cube_texture.png", &texWidth, &texHeight, &nrChannels, 0);
	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		fprintf(stderr, "Failed to load texture\n");
	}
	stbi_image_free(data);

	// Используем текстурный юнит 0
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

	// Основной цикл
	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);

		glClearColor(0.08f, 0.10f, 0.13f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		// Матрица model (вращение)
		float model[16];
		identity4(model);
		rotateY4(model, (float) glfwGetTime());

		// Матрица view (камера)
		float view[16];
		identity4(view);
		translate4(view, 0.0f, 0.0f, -3.0f);

		// Матрица projection (перспектива)
		float projection[16];
		perspective4(projection, (float) M_PI / 4.0f, (float) WIDTH / HEIGHT, 0.1f, 100.0f);

		// Передача матриц в шейдеры
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Очистка ресурсов
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &texture);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
