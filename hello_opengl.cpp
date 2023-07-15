#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int figure = 0;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 model = glm::mat4(1.0f);

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "uniform mat4 projection;\n"
                                 "uniform mat4 view;\n"
                                 "uniform mat4 model;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = ourColor;\n"
                                   "}\n\0";

int main()
{
    int n;
    cout << "Enter the number of sides of the polygon in the prism : ";
    cin >> n;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[6 * (n + 1)];
    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = 0.5f;
    int vertex_count = 0;

    for (int i = 3; i < 3 * (n + 1); i += 3)
    {
        vertices[i] = (cos(float(2 * 3.14159 * vertex_count) / float(n))) * 0.5;
        vertices[i + 1] = (sin(float(2 * 3.14159 * vertex_count) / float(n))) * 0.5;
        vertices[i + 2] = 0.5f;
        vertex_count++;
    }

    vertices[3 * (n + 1)] = 0.0f;
    vertices[3 * (n + 1) + 1] = 0.0f;
    vertices[3 * (n + 1) + 2] = -0.5f;

    vertex_count = 0;

    for (int i = 3; i < 3 * (n + 1); i += 3)
    {
        vertices[i + (3 * (n + 1))] = (cos(float(2 * 3.14159 * vertex_count) / float(n))) * 0.5;
        vertices[i + 1 + (3 * (n + 1))] = (sin(float(2 * 3.14159 * vertex_count) / float(n))) * 0.5;
        vertices[i + 2 + (3 * (n + 1))] = -0.5f;
        vertex_count++;
    }

    unsigned int indices_prism[12 * n];
    vertex_count = 1;

    for (int i = 0; i < 3 * n; i += 3)
    {
        indices_prism[i] = 0;
        indices_prism[i + 1] = vertex_count;
        indices_prism[i + 2] = vertex_count + 1;
        vertex_count++;
    }

    indices_prism[(3 * n) - 1] = 1;
    vertex_count++;

    for (int i = 3 * n; i < 6 * n; i += 3)
    {
        indices_prism[i] = n + 1;
        indices_prism[i + 1] = vertex_count;
        indices_prism[i + 2] = vertex_count + 1;
        vertex_count++;
    }

    indices_prism[(6 * n) - 1] = n + 2;
    vertex_count = 1;

    for (int i = 6 * n; i < 9 * n; i += 3)
    {
        indices_prism[i] = vertex_count;
        indices_prism[i + 1] = vertex_count + 1;
        indices_prism[i + 2] = vertex_count + n + 1;
        vertex_count++;
    }

    indices_prism[(9 * n) - 2] = 1;
    vertex_count++;

    for (int i = 9 * n; i < 12 * n; i += 3)
    {
        indices_prism[i] = vertex_count;
        indices_prism[i + 1] = vertex_count + 1;
        indices_prism[i + 2] = vertex_count - (n);
        vertex_count++;
    }

    indices_prism[12 * n - 1] = 1;
    indices_prism[12 * n - 2] = n + 2;

    unsigned int indices_pyramid[6 * n];
    vertex_count = 1;

    for (int i = 0; i < 3 * n; i += 3)
    {
        indices_pyramid[i] = 0;
        indices_pyramid[i + 1] = vertex_count;
        indices_pyramid[i + 2] = vertex_count + 1;
        vertex_count++;
    }

    indices_pyramid[(3 * n) - 1] = 1;
    vertex_count = 1;

    for (int i = 3 * n; i < 6 * n; i += 3)
    {
        indices_pyramid[i] = n + 1;
        indices_pyramid[i + 1] = vertex_count;
        indices_pyramid[i + 2] = vertex_count + 1;
        vertex_count++;
    }

    indices_pyramid[(6 * n) - 1] = 1;

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_prism), indices_prism,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
                                // glBindVertexArray(0); // no need to unbind it every time

        glm::mat4 view = glm::lookAt(cameraPos,
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     cameraUp);
        if (figure % 2 == 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_prism), indices_prism,
                         GL_STATIC_DRAW);

            for (int i = 0; i < 3 * n; i += 3)
            {
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
                glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);

                unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(unsigned int)));
            }

            for (int i = 3 * n; i < 6 * n; i += 3)
            {
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
                glUniform4f(vertexColorLocation, 0.0f, 0.0f, 1.0f, 1.0f);

                unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(unsigned int)));
            }

            for (int i = 6 * n; i < 9 * n; i += 3)
            {
                float colour = ((float)(i - (6 * n)) / (float(3 * n)));
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
                glUniform4f(vertexColorLocation, 0.0f, colour, 0.0f, 1.0f);

                unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(unsigned int)));
            }

            for (int i = 9 * n; i < 12 * n; i += 3)
            {
                float colour = ((float)(i - (9 * n)) / (float(3 * n)));
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
                glUniform4f(vertexColorLocation, 0.0f, colour, 0.0f, 1.0f);

                unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(unsigned int)));
            }
        }
        else
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_pyramid), indices_pyramid,
                         GL_STATIC_DRAW);

            for (int i = 0; i < 3 * n; i += 3)
            {
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
                glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);

                unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(unsigned int)));
            }

            for (int i = 3 * n; i < 6 * n; i += 3)
            {
                float colour = ((float)(i - (3 * n)) / (float(3 * n)));
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
                glUniform4f(vertexColorLocation, 0.0f, colour, 0.0f, 1.0f);

                unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(unsigned int)));
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        model = glm::translate(model, glm::vec3(0.0f, 0.005f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        model = glm::translate(model, glm::vec3(0.0f, -0.005f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        model = glm::translate(model, glm::vec3(-0.005f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        model = glm::translate(model, glm::vec3(0.005f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.005f));
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.005f));
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        model = glm::rotate(model, (float)0.01, glm::vec3(0.0f, 0.05f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        figure++;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}