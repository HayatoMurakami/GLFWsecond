//
//  application.cpp
//  GLFW first
//
//  Created by 村上 颯斗 on 2021/03/28.
//

#include "application.h"

namespace game {

GLuint createProgram(std::string vertexShaderFile,
                     std::string fragmentShaderFile) {
  // 頂点シェーダの読み込み
  std::ifstream vertexIfs(vertexShaderFile, std::ios::binary);
  if (vertexIfs.fail()) {
    std::cerr << "Error: Can't open source file: " << vertexShaderFile
              << std::endl;
    return 0;
  }
  auto vertexShaderSource =
      std::string(std::istreambuf_iterator<char>(vertexIfs),
                  std::istreambuf_iterator<char>());
  if (vertexIfs.fail()) {
    std::cerr << "Error: Can't read source file: " << vertexShaderFile
              << std::endl;
    return 0;
  }
  const GLchar* vertexShaderSourcePointer = vertexShaderSource.c_str();

  // フラグメントシェーダの読み込み
  std::ifstream fragmentIfs(fragmentShaderFile, std::ios::binary);
  if (fragmentIfs.fail()) {
    std::cerr << "Error: Can't open source file: " << fragmentShaderFile
              << std::endl;
    return 0;
  }
  auto fragmentShaderSource =
      std::string(std::istreambuf_iterator<char>(fragmentIfs),
                  std::istreambuf_iterator<char>());
  if (fragmentIfs.fail()) {
    std::cerr << "Error: Can't read source file: " << fragmentShaderFile
              << std::endl;
    return 0;
  }
  const GLchar* fragmentShaderSourcePointer = fragmentShaderSource.c_str();

  // プログラムオブジェクトを作成
  const GLuint program = glCreateProgram();

  GLint status = GL_FALSE;
  GLsizei infoLogLength;

  // 頂点シェーダのコンパイル
  const GLuint vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderObj, 1, &vertexShaderSourcePointer, nullptr);
  glCompileShader(vertexShaderObj);
  glAttachShader(program, vertexShaderObj);

  // 頂点シェーダのチェック
  glGetShaderiv(vertexShaderObj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    std::cerr << "Compile Error in Vertex Shader." << std::endl;
  glGetShaderiv(vertexShaderObj, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 1) {
    std::vector<GLchar> vertexShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(vertexShaderObj, infoLogLength, nullptr,
                       vertexShaderErrorMessage.data());
    std::cerr << vertexShaderErrorMessage.data() << std::endl;
  }

  glDeleteShader(vertexShaderObj);

  // フラグメントシェーダのコンパイル
  const GLuint fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderObj, 1, &fragmentShaderSourcePointer, nullptr);
  glCompileShader(fragmentShaderObj);
  glAttachShader(program, fragmentShaderObj);

  // フラグメントシェーダのチェック
  glGetShaderiv(fragmentShaderObj, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    std::cerr << "Compile Error in Fragment Shader." << std::endl;
  glGetShaderiv(fragmentShaderObj, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 1) {
    std::vector<GLchar> fragmentShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(fragmentShaderObj, infoLogLength, nullptr,
                       fragmentShaderErrorMessage.data());
    std::cerr << fragmentShaderErrorMessage.data() << std::endl;
  }

  glDeleteShader(fragmentShaderObj);

  // プログラムのリンク
  glLinkProgram(program);

  // リンクのチェック
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 1) {
    std::vector<GLchar> programLinkErrorMessage(infoLogLength);
    glGetProgramInfoLog(program, infoLogLength, nullptr,
                        programLinkErrorMessage.data());
    std::cerr << programLinkErrorMessage.data() << std::endl;
  }

  return program;
}

bool Application::Run() {
  if (!Init()) {
    std::cerr << "Initialization error..." << std::endl;
    return false;
  }

  glfwSetTime(0.0);
  double delta_time = 0.0;
  double prev_time = 0.0;

  while (glfwWindowShouldClose(window_) == GL_FALSE) {
    const double time = glfwGetTime();
    delta_time = time - prev_time;
    prev_time = time;

    Update(delta_time);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

  glfwTerminate();

  return true;
}

bool Application::Init() {
  const GLuint width = 640;
  const GLuint height = 480;

  if (!InitWindow(width, height)) {
    std::cerr << "Error: InitWindow" << std::endl;
    return false;
  }

  // Shaderプログラムの作成
  program_ = createProgram("shader.vert", "shader.frag");

  // 三角形メッシュの作成
  triangle_ = Mesh::CreateTriangleMesh();

  return true;
}

bool Application::InitWindow(const GLuint width, const GLuint height) {
  glfwSetErrorCallback(
      [](auto id, auto description) { std::cerr << description << std::endl; });

  // GLFWの初期化
  if (!glfwInit()) {
    return false;
  }

  // OpenGL Version 4.1 Core Profileを選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // リサイズ不可
  //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // ウィンドウの作成
  window_ = glfwCreateWindow(width, height, "Window1", nullptr, nullptr);
  if (window_ == nullptr) {
    std::cerr << "Can't create GLFW window." << std::endl;
    return false;
  }
  glfwMakeContextCurrent(window_);

  // GLEWの初期化
  if (glewInit() != GLEW_OK) {
    std::cerr << "Can't initialize GLEW." << std::endl;
    return false;
  }

  // VSyncを待つ
  glfwSwapInterval(1);
  
  return true;
     
}

void Application::Update(const double delta_time) {
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program_);

  triangle_->Draw();
}

}  // namespace game
