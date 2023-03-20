#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <Camera.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
//上一帧的鼠标位置初始值设置为屏幕的中心
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

int main()
{
  //---------------------------------------------------------------------------------------
  //初始化并配置GLFW
  //---------------------------------------------------------------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //---------------------------------------------------------------------------------------
  //创建窗口对象
  //---------------------------------------------------------------------------------------
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to creat window" << std::endl;
    glfwTerminate();
    return -1;
  }
  //设置为当前线程context
  glfwMakeContextCurrent(window);
  //注册回调 窗口大小调整时调用
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //注册回调 鼠标一移动mouse_callback函数就会被调用
  glfwSetCursorPosCallback(window, mouse_callback);
  //注册回调 鼠标一滚动scroll_callback函数就会被调用
  glfwSetScrollCallback(window, scroll_callback);

  // 隐藏光标并捕捉它
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  //---------------------------------------------------------------------------------------
  //初始化GLAD 载入所有OpenGL函数指针
  //---------------------------------------------------------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  //启用深度测试
  glEnable(GL_DEPTH_TEST);

  //---------------------------------------------------------------------------------------
  //创建shader
  //---------------------------------------------------------------------------------------
  // ShaderProgram ourShader("..\\..\\shaders\\shader.vs", "..\\..\\shaders\\shader.fs");
  ShaderProgram ourShader("..\\..\\shaders\\coordinate_systems.vs", "..\\..\\shaders\\coordinate_systems.fs");

  //---------------------------------------------------------------------------------------
  //设定顶点数据和缓冲区并配置顶点属性
  // VBO顶点缓冲对象 在GPU内存中储存大量顶点 可以一次性的发送一大批数据到显卡上
  // VAO顶点数组对象 被绑定后任何随后的顶点属性调用都会储存在这个VAO中 只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行  便于在不同顶点数据和属性配置之间切换
  // EBO元素缓冲对象 同VBO是一个缓冲区，存储 OpenGL 用来决定要绘制哪些顶点的索引 用于防止顶点叠加时重复绘制
  // VAO也跟踪VBO绑定。在绑定VAO时，绑定的最后一个元素缓冲区对象存储为VAO的元素缓冲区对象EBO。然后，绑定到VAO也会自动绑定该EBO。
  //---------------------------------------------------------------------------------------

  //设置顶点属性
  // 2D
  //   float vertices[] = {
  // //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
  //      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
  //      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
  //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
  //     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
  //   };
  // 3D 颜色随便填的
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

      0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f};

  //绘制三角形序号
  unsigned int indices[] = {
      // 索引从0开始! 此例的索引(0,1,2,3)就是顶点数组vertices的下标，这样可以由下标代表顶点组合成矩形
      0, 1, 3, // 第一个三角形
      1, 2, 3  // 第二个三角形
  };

  //定义10个立方体位置
  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),
      glm::vec3(-1.3f, 1.0f, -1.5f)};

  unsigned int VBO, VAO, EBO;
  //由缓冲ID生成一个VBO和VAO和EBO
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  // 1. 绑定VAO
  glBindVertexArray(VAO);
  // 2. 把顶点数组复制到缓冲中供OpenGL使用
  //新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  //把之前定义的顶点数据复制到缓冲的内存中
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  //把indices数组复制到缓冲
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  // 3. 设置顶点属性指针
  //位置属性
  //指定OpenGL该如何解释顶点数据 第一个参数指定我们要配置的顶点属性的位置值(location = 0), 第二个参数指定顶点属性的大小, 第三个参数指定数据的类型, 第四个参数定义我们是否希望数据被标准化(被映射到0（对signed数据是-1）到1之间), 第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔 (紧密排列时可以设置为0来让OpenGL决定具体步长是多少), 第六个参数表示位置数据在缓冲中起始位置的偏移量(Offset)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  //以顶点属性位置值作为参数，启用顶点属性
  glEnableVertexAttribArray(0);
  //颜色属性
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //纹理坐标属性
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  //取消绑定(非必须)
  //这是允许的，对 glVertexAttribPointer 的调用将 VBO 注册为顶点属性的绑定顶点缓冲区对象，因此之后我们可以安全地解除绑定
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  //之后可以取消绑定 VAO，这样其他 VAO 调用就不会意外修改此 VAO，但这种情况很少发生。修改其他VAO无论如何都需要调用 glBindVertexArray，所以我们通常不会在没有必要时取消绑定 VAO(VBO也不会)
  // glBindVertexArray(0);

  //---------------------------------------------------------------------------------------
  //创建并载入纹理
  //---------------------------------------------------------------------------------------
  //创建纹理对象
  unsigned int texture1, texture2;
  // texture1
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  //为当前绑定的纹理对象设置环绕、过滤方式
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //加载并生成纹理
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); // OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部 翻转y轴
  unsigned char *data = stbi_load("..\\..\\images\\container.jpg", &width, &height, &nrChannels, 0);
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。第四个和第五个参数设置最终的纹理的宽度和高度。下个参数应该总是被设为0（历史遗留的问题）。第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。最后一个参数是真正的图像数据。
    glGenerateMipmap(GL_TEXTURE_2D);
    //生成了纹理和相应的多级渐远纹理后，释放图像的内存
    stbi_image_free(data);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }

  // texture2
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  //为当前绑定的纹理对象设置环绕、过滤方式
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //加载并生成纹理
  data = stbi_load("..\\..\\images\\awesomeface.png", &width, &height, &nrChannels, 0);
  if (data)
  {
    // RGB改成RGBA,这个图是四通道图
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。第四个和第五个参数设置最终的纹理的宽度和高度。下个参数应该总是被设为0（历史遗留的问题）。第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。最后一个参数是真正的图像数据。
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  //生成了纹理和相应的多级渐远纹理后，释放图像的内存
  stbi_image_free(data);

  //告诉OpenGL每个着色器采样器属于哪个纹理单元
  // 在设置uniform变量之前激活着色器程序
  ourShader.use();
  ourShader.set_uniform("texture1", 0);
  ourShader.set_uniform("texture2", 1);

  //---------------------------------------------------------------------------------------
  //渲染
  //---------------------------------------------------------------------------------------
  //绘制方式
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //第一个参数表示将其应用到所有的三角形的正面和背面，第二个参数绘制方式。
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //改为填充

  while (!glfwWindowShouldClose(window)) // GLFW退出前一直运行
  {
    // per-frame time logic 确保在所有硬件上移动速度都一样
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window); //输入控制

    //渲染指令
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);               //设置清空屏幕所用的颜色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空屏幕的颜色缓冲 清除深度缓冲

    //在绑定纹理之前先激活纹理单元(使用多个纹理时)
    glActiveTexture(GL_TEXTURE0);
    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // transform
    // glm::mat4 transform = glm::mat4(1.0f);
    // transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
    // transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

    //更新uniform之前必须先使用程序
    ourShader.use(); //激活程序对象

    //把变换矩阵传给着色器
    // ourShader.set_uniform("transform", 1, GL_FALSE, glm::value_ptr(transform));

    //更新uniform颜色
    // float timeValue = glfwGetTime();
    // float greenValue = sin(timeValue) / 2.0f + 0.5f;
    // ourShader.set_uniform("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

    // 3D
    // glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = camera.GetViewMatrix();
    // projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // ourShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));
    ourShader.set_uniform("view", 1, GL_FALSE, glm::value_ptr(view));
    ourShader.set_uniform("projection", 1, GL_FALSE, glm::value_ptr(projection));

    //画三角形
    glBindVertexArray(VAO); //只有一个VAO时没必要再绑定一次 绑定到VAO也会自动绑定该EBO
    // glDrawArrays(GL_TRIANGLES, 0, 3);//第二个参数指定顶点数组的起始索引 最后一个参数指定打算绘制多少个顶点
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //第二个参数是我们打算绘制顶点的个数 第三个参数是索引的类型 最后一个参数里指定EBO中的偏移量（或者传递一个索引数组，当不在使用索引缓冲对象的时候）
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    for (unsigned int i = 0; i < 10; i++)
    {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * (i + 1);
      model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      ourShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window); //交换颜色缓冲 迭代中被用来绘制，并且将会作为输出显示在屏幕上
    glfwPollEvents();        //检查有没有触发什么事件 更新窗口状态，并调用对应的回调函数
  }

  //不用后取消分配
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  //释放/删除之前的分配的所有资源
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  //设置窗口维度
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

//监听鼠标移动事件
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  //防止第一次获取焦点的时候摄像机突然跳一下(鼠标移动进窗口的那一刻，鼠标回调函数就会被调用)
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  //计算鼠标距上一帧的偏移量
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; //相反的，y坐标是从底部往顶部依次增大的

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}