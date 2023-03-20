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
unsigned int loadTexture(char const * path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

//lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to creat window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    ShaderProgram lightingShader("..\\..\\shaders\\materials.vs", "..\\..\\shaders\\materials.fs");
    ShaderProgram lightCubeShader("..\\..\\shaders\\light_cube.vs", "..\\..\\shaders\\light_cube.fs");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Load textures
    unsigned int diffuseMap = loadTexture("..\\..\\images\\container2.png");
    unsigned int specularMap = loadTexture("..\\..\\images\\container2_specular.png");
    // unsigned int emissionMap = loadTexture("..\\..\\images\\matrix.jpg");
    
    lightingShader.use();
    lightingShader.set_uniform("material.diffuse", 0);
    lightingShader.set_uniform("material.specular", 1);
    // lightingShader.set_uniform("material.emission", 2);

    while (!glfwWindowShouldClose(window)) // GLFW退出前一直运行
    {
        // per-frame time logic 确保在所有硬件上移动速度都一样
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window); //输入控制

        //渲染指令
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.set_uniform("viewPos", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        //lightingShader.set_uniform("material.ambient", 1.0f, 0.5f, 0.31f);
        //lightingShader.set_uniform("material.diffuse", 1.0f, 0.5f, 0.31f);
        // lightingShader.set_uniform("material.specular", 0.5f, 0.5f, 0.5f);
        lightingShader.set_uniform("material.shininess", 32.0f);
         
        // directional light
        lightingShader.set_uniform("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.set_uniform("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.set_uniform("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.set_uniform("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.set_uniform("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        lightingShader.set_uniform("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.set_uniform("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.set_uniform("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.set_uniform("pointLights[0].constant", 1.0f);
        lightingShader.set_uniform("pointLights[0].linear", 0.09f);
        lightingShader.set_uniform("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.set_uniform("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        lightingShader.set_uniform("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.set_uniform("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.set_uniform("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.set_uniform("pointLights[1].constant", 1.0f);
        lightingShader.set_uniform("pointLights[1].linear", 0.09f);
        lightingShader.set_uniform("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.set_uniform("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        lightingShader.set_uniform("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.set_uniform("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.set_uniform("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.set_uniform("pointLights[2].constant", 1.0f);
        lightingShader.set_uniform("pointLights[2].linear", 0.09f);
        lightingShader.set_uniform("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.set_uniform("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        lightingShader.set_uniform("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.set_uniform("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.set_uniform("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.set_uniform("pointLights[3].constant", 1.0f);
        lightingShader.set_uniform("pointLights[3].linear", 0.09f);
        lightingShader.set_uniform("pointLights[3].quadratic", 0.032f);
        // spotLight
        lightingShader.set_uniform("spotLight.position", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        lightingShader.set_uniform("spotLight.direction",camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        lightingShader.set_uniform("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.set_uniform("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.set_uniform("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.set_uniform("spotLight.constant", 1.0f);
        lightingShader.set_uniform("spotLight.linear", 0.09f);
        lightingShader.set_uniform("spotLight.quadratic", 0.032f);
        lightingShader.set_uniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.set_uniform("spotLight.outerCutOff", glm::cos(glm::radians(15.0f))); 


/*光线可变
        glm::vec3 lightColor;
        lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0f));
        lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7f));
        lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3f));
        //降低影响
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        lightingShader.set_uniform("light.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
        lightingShader.set_uniform("light.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
*/       
        
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightingShader.set_uniform("view", 1, GL_FALSE, glm::value_ptr(view));
        lightingShader.set_uniform("projection", 1, GL_FALSE, glm::value_ptr(projection));

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));

        //bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
/*emission
        float mLight = static_cast<float>(1.5 + sin(glfwGetTime()));
        float mMove = static_cast<float>(glfwGetTime());
        lightingShader.set_uniform("matrixLight", mLight);
        lightingShader.set_uniform("matrixMove", mMove);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);
*/
        // render the cube
        glBindVertexArray(cubeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
//lightcube
        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.set_uniform("view", 1, GL_FALSE, glm::value_ptr(view));
        lightCubeShader.set_uniform("projection", 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(lightCubeVAO);
        for(unsigned int i = 0; i < 4; i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            lightCubeShader.set_uniform("model", 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        } 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //不用后取消分配
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

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

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}