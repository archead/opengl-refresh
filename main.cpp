#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shader.h"
#include "buffer.h"
#include "VertexArray.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

#include "vertices.h"

#include "camera.h"

void processInput(GLFWwindow* window); // for continous key press
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); // single key presses, i.e toggles
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

int resWidth = 800;
int resHeight = 600;

float lastX = (float)resWidth / 2.0;
float lastY = (float)resHeight / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool mouseToggle = false;

Camera camera;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(resWidth, resHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, resWidth, resHeight);

    //---------------------CALLBACK SETS------------------------
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //----------------------------------------------------------

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader lightingShader("lightingShader.vert", "lightingShader.frag");
    Shader lightObjShader("lightObjShader.vert", "lightObjShader.frag");

    VertexArray va;
    VertexBuffer vb(vertices, sizeof(vertices));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    va.unbind();

    VertexArray lightVAO;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    lightVAO.unbind();

    //Texture texture1("container.jpg", 0);
    //texture1.SetSampler2D(shader.ID, "texture1");

    lightingShader.use();
    lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.00f);
    
    glEnable(GL_DEPTH_TEST);

    //----------------------IMGUI INIT-------------------------------
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 460";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //----------------------------------------------------------------

    float rotationdeg = 0.0f;
    glm::vec3 modelAxis = glm::vec3(0.5f, 1.0f, 0.3f);
    float spinSpeed = 0.5f;
    bool spin = true;

    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 1.0f);
    lightingShader.setVec3("lightPos", lightPos);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        lightingShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotationdeg), glm::vec3(modelAxis.x, modelAxis.y, modelAxis.z));
        lightingShader.setMat4("model", model);

        glm::mat4 view = camera.getViewMatrix(); 
        lightingShader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)resWidth / float(resHeight), 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);

        va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        va.unbind();

        lightObjShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightObjShader.setMat4("model", model);

        view = camera.getViewMatrix();
        lightObjShader.setMat4("view", view);

        projection = glm::perspective(glm::radians(camera.fov), (float)resWidth / float(resHeight), 0.1f, 100.0f);
        lightObjShader.setMat4("projection", projection);

        lightVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        lightVAO.unbind();

        // ImGui Menu Items
        {   
            ImGui::Begin("Debug Menu"); // Create a window called "Debug Menu" and append into it.
            ImGui::Text("Press M to toggle mouse");
            ImGui::Text("Model Rotation Matrix:");
            ImGui::Checkbox("Continuous Spin", &spin); // Edit bools storing our window open/close state
            ImGui::SliderFloat("Spin Speed", &spinSpeed, 0.0f, 10.0f);
            if (spin) rotationdeg += spinSpeed;
            ImGui::SliderFloat3("XYZ", glm::value_ptr(modelAxis), 0.01f, 1.0f);
            ImGui::Text("FOV:");
            ImGui::SliderFloat("FOV Scale", &camera.fov, 1.0f, 120.0f);
            ImGui::Text("Yaw and Pitch");
            ImGui::Text("Yaw: %f", camera.yaw);
            ImGui::Text("Pitch: %f", camera.pitch);
            ImGui::Text("Camera Coords:");
            ImGui::Text("X: %f", camera.cameraPos.x);
            ImGui::Text("Y: %f", camera.cameraPos.y);
            ImGui::Text("Z: %f", camera.cameraPos.z);

            ImGui::Text("Application avg %.3f ms/frame", 1000.0f / io.Framerate);
            ImGui::Text("%.1f FPS", io.Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &va.ID);
    glDeleteBuffers(1, &vb.ID);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.keyboardMovement(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.keyboardMovement(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.keyboardMovement(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.keyboardMovement(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.keyboardMovement(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.keyboardMovement(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    resWidth = width;
    resHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    if(!mouseToggle)
    {
        camera.mouseMovement(&lastX, &lastY, xposIn, yposIn);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_M && action == GLFW_PRESS) mouseToggle = !mouseToggle;
    if (mouseToggle) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
