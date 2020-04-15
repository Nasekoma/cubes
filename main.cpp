#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include <cmath>
#include <iostream>

#include <shaderCLASS.h>
#include <cameraCLASS.h>

/////////////////////////////////////////////////GLOBAL CONSTANTS////////////////////////////////////////

enum
{
    WINDOW_WIDTH = 800,
    WINDOW_HEIGHT = 600
};
GLfloat CAMERA_POSITION_X = 0.0f;
GLfloat CAMERA_POSITION_Y = 0.0f;
GLfloat CAMERA_POSITION_Z = 3.0f;

/////////////////////////////////////////////////CAMERA SETTINGS/////////////////////////////////////////

Camera camera(glm::vec3(CAMERA_POSITION_X, CAMERA_POSITION_Y, CAMERA_POSITION_Z));
bool keys[1024];
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

////////////////////////////////////////////////TEXTURE SETTINGS/////////////////////////////////////////

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
        else {
            std::cout << "loadTexture: UNEXPECTED NUMBER OF COLOUR COMPONENTS" << std::endl;
            return -1;
        }

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

////////////////////////////////////////////////CONTROL SETTINGS/////////////////////////////////////////

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(keys[GLFW_KEY_U])
        camera.ProcessKeyboard(UP, deltaTime);
    if(keys[GLFW_KEY_J])
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

int main() {

////////////////////////////////////////////////////WINDOW///////////////////////////////////////////////////

    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "Failed to initialise GLFW window" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "COLORgraph window", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetCursorPos(window, lastX, lastY);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);

    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

////////////////////////////////////////////////GLM INFO////////////////////////////////////////////////////

    float i_matrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };

////////////////////////////////////////OBJECT (CUBE) COORDINATES/////////////////////////////////////

    float cube_vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
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

/////////////////////////////////////////////VBO+VAO FOR CUBE///////////////////////////////////////

    GLuint VBO3D, VAO3D;
    glGenVertexArrays(1, &VAO3D);
    glGenBuffers(1, &VBO3D);

    glBindVertexArray(VAO3D);

    glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)nullptr);
    glEnableVertexAttribArray(0);

    // texCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // normal attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0); // Unbind VAO

///////////////////////////////////////////LIGHT VAO//////////////////////////////////////////////////

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO3D);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

////////////////////////////////////////////////////DIFFUSE MAP////////////////////////////////////////////

    unsigned int diffuseMap = loadTexture("../cubes-textures/metalContainer.png");
    if(diffuseMap == -1)
        std::cout<<"FAILED TO OPEN IMAGE 1"<<std::endl;
    unsigned int specularMap = loadTexture("../cubes-textures/metalPart.png");
    if(specularMap == -1)
        std::cout<<"FAILED TO OPEN IMAGE 2"<<std::endl;

/////////////////////////////////////////////////SHADER PROGRAMS///////////////////////////////////////////

    Shader lightingShader("../cubes-shaders/object.vs", "../cubes-shaders/object-with-lights.fs");
    Shader lampShader("../cubes-shaders/lamp.vs", "../cubes-shaders/lamp.fs");

////////////////////////////////////////////////DRAWING PARAMETERS//////////////////////////////////////

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

////////////////////////////////////////////////LIGHT PARAMETERS
    //glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightAmbients[] = {
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.2f, 0.2f, 0.2f),
            glm::vec3(0.2f, 0.2f, 0.2f)
    };
    glm::vec3 pointLightDiffuses[] = {
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(0.5f, 0.5f, 0.5f)
    };
    glm::vec3 pointLightSpeculars[] = {
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
    };

    glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);
    glm::vec3 dirLightAmbient(0.1f, 0.1f, 0.1f);
    glm::vec3 dirLightDiffuse(0.5f, 0.5f, 0.5f);
    glm::vec3 dirLightSpecular(1.0f, 1.0f, 1.0f);
    glm::vec3 dirLightColor(0.0f, 0.0f, 0.0f);

    GLfloat lightConstant = 1.0f;
    GLfloat lightLinear = 0.09f;
    GLfloat lightQuadratic = 0.032f;

    glm::vec3 pointLightColors[] = {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
    };

////////////////////////////////////////////////OBJECT MATERIAL PARAMETERS
    GLfloat objectShininess = 64.0f;

////////////////////////////////////////////////////////MAIN CYCLE/////////////////////////////////////////////////////

    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        Do_Movement();

        glClearColor(0.15f, 0.0f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/////////////////////////////////////////////////////CHANGING MATRICES

        //projection matrix
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom),
                                          (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 1000.0f);

        //view matrix
        glm::mat4 view = camera.GetViewMatrix();

        //model matrix
        glm::mat4 model;

/////////////////////////////////////////////////////DRAWING LAMP/-S

        glBindTexture(GL_TEXTURE_2D, 0);
        lampShader.use();
        for(int i = 0; i < 4; i++) {
            lampShader.setMat4("projection", proj);
            lampShader.setMat4("view", view);

            model = glm::make_mat4(i_matrix);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            lampShader.setMat4("model", model);

            lampShader.setVec3("LampColor", pointLightColors[i]);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
/////////////////////////////////////////////////////DRAWING CUBE/-S

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        lightingShader.use();

        lightingShader.setMat4("projection", proj);
        lightingShader.setMat4("view", view);
        model = glm::make_mat4(i_matrix);
        lightingShader.setMat4("model", model);
        lightingShader.setVec3("viewPos", camera.Position);

        GLfloat sin_par = sin(glfwGetTime());
        GLfloat cos_par = cos(glfwGetTime());

        //directional light properties
        lightingShader.setVec3("dirLight.direction", lightDirection);
        //lightingShader.setVec3("dirLight.color", dirLightColor);
        lightingShader.setVec3("dirLight.color", glm::vec3(sin_par, sin_par * cos_par * 2.0f, cos_par));
        lightingShader.setVec3("dirLight.ambient", dirLightAmbient);
        lightingShader.setVec3("dirLight.diffuse", dirLightDiffuse);
        lightingShader.setVec3("dirLight.specular", dirLightSpecular);

        //point light properties
        ////1(0)
        lightingShader.setVec3("pointLights[0].color",  pointLightColors[0]);
        lightingShader.setVec3("pointLights[0].position",  pointLightPositions[0]);

        lightingShader.setVec3("pointLight[0].ambient", pointLightAmbients[0]);
        lightingShader.setVec3("pointLights[0].diffuse", pointLightDiffuses[0]);
        lightingShader.setVec3("pointLights[0].specular", pointLightSpeculars[0]);

        lightingShader.setFloat("pointLights[0].constant", lightConstant);
        lightingShader.setFloat("pointLights[0].linear", lightLinear);
        lightingShader.setFloat("pointLights[0].quadratic", lightQuadratic);

        ////2(1)
        lightingShader.setVec3("pointLights[1].color",  pointLightColors[1]);
        lightingShader.setVec3("pointLights[1].position",  pointLightPositions[1]);

        lightingShader.setVec3("pointLights[1].ambient", pointLightAmbients[1]);
        lightingShader.setVec3("pointLights[1].diffuse", pointLightDiffuses[1]);
        lightingShader.setVec3("pointLights[1].specular", pointLightSpeculars[1]);

        lightingShader.setFloat("pointLights[1].constant", lightConstant);
        lightingShader.setFloat("pointLights[1].linear", lightLinear);
        lightingShader.setFloat("pointLights[1].quadratic", lightQuadratic);

        ////3(2)
        lightingShader.setVec3("pointLights[2].color",  pointLightColors[2]);
        lightingShader.setVec3("pointLights[2].position",  pointLightPositions[2]);

        lightingShader.setVec3("pointLights[2].ambient", pointLightAmbients[2]);
        lightingShader.setVec3("pointLights[2].diffuse", pointLightDiffuses[2]);
        lightingShader.setVec3("pointLights[2].specular", pointLightSpeculars[2]);

        lightingShader.setFloat("pointLights[2].constant", lightConstant);
        lightingShader.setFloat("pointLights[2].linear", lightLinear);
        lightingShader.setFloat("pointLights[2].quadratic", lightQuadratic);

        ////4(3)
        lightingShader.setVec3("pointLights[3].color",  pointLightColors[3]);
        lightingShader.setVec3("pointLights[3].position",  pointLightPositions[3]);

        lightingShader.setVec3("pointLights[3].ambient", pointLightAmbients[3]);
        lightingShader.setVec3("pointLights[3].diffuse", pointLightDiffuses[3]);
        lightingShader.setVec3("pointLights[3].specular", pointLightSpeculars[3]);

        lightingShader.setFloat("pointLights[3].constant", lightConstant);
        lightingShader.setFloat("pointLights[3].linear", lightLinear);
        lightingShader.setFloat("pointLights[3].quadratic", lightQuadratic);


        // material properties
        lightingShader.setFloat("material.shininess", objectShininess);
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);

        //drawing object/-s
        glBindVertexArray(VAO3D);
        for(unsigned int i = 0; i < 10; i++)
        {
            GLfloat time = glfwGetTime();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i + 40.0f;
            model = glm::rotate(model, time * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    return 0;
}
