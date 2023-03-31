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

    Shader shader("shader.vert", "shader.frag");
    shader.use();

    VertexArray va;
    VertexBuffer vb(vertices, sizeof(vertices));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    va.unbind();

    Texture texture1("container.jpg", 0);
    texture1.SetSampler2D(shader.ID, "texture1");
    
    glEnable(GL_DEPTH_TEST);

    unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
    unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");

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


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotationdeg), glm::vec3(modelAxis.x, modelAxis.y, modelAxis.z));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = camera.getViewMatrix(); 
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)resWidth / float(resHeight), 0.1f, 100.0f);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        va.unbind();

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
            ImGui::Text("Camera Yaw and Pitch Values:");
            ImGui::Text("Yaw: %f", camera.yaw);
            ImGui::Text("Pitch: %f", camera.pitch);
            ImGui::Text("Camera Coords:");
            ImGui::Text("X: %f", camera.cameraPos.x);
            ImGui::Text("Y: %f", camera.cameraPos.y);
            ImGui::Text("Z: %f", camera.cameraPos.z);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
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
        camera.mouseMovement(&lastX, &lastY, xposIn, yposIn);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_M && action == GLFW_PRESS) mouseToggle = !mouseToggle;
    if (mouseToggle) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
