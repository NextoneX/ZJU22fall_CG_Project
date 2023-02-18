#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/particle.h>
#include <learnopengl/ground.hpp>
#include <learnopengl/objload.hpp>
#include <learnopengl/texture.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
void renderScene(const Shader& shader);
//void renderCube();
void renderPlane();
void renderGround();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool shadows = true;
bool shadowsKeyPressed = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool judge = true;
bool shake = false;

// rotate
float xrotate = 30.0f;
float yrotate = 0.0f;

// plane
std::vector<glm::vec3> plane_vertices;
std::vector<glm::vec2> plane_uvs;
std::vector<glm::vec3> plane_normals;
GLuint plane_VAO = 0;
GLuint plane_vertices_VBO = 0;
GLuint plane_uvs_VBO = 0;
GLuint plane_normals_VBO = 0;

// ground
std::vector<glm::vec3> ground_vertices;
std::vector<glm::vec2> ground_uvs;
std::vector<glm::vec3> ground_normals;
GLuint ground_VAO = 0;
GLuint ground_vertices_VBO = 0;
GLuint ground_uvs_VBO = 0;
GLuint ground_normals_VBO = 0;

unsigned int planeTexture = 0;
unsigned int groundTexture = 0;

// particle
glm::vec3 particle_pos = glm::vec3(0, 0, 0);
Particle_Flow p1 = Particle_Flow(particle_pos + glm::vec3(0.1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0.5, 0.7, 1), 1, 10);
Particle_Flow p2 = Particle_Flow(particle_pos + glm::vec3(0, 0.1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0.7, 1), 1, 10);
Particle_Flow p3 = Particle_Flow(particle_pos + glm::vec3(-0.1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(-0.5, 0.7, 1), 1, 10);

int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("shader/shadow/point_shadows.vs", "shader/shadow/point_shadows.fs");
    Shader simpleDepthShader("shader/shadow/point_shadows_depth.vs", "shader/shadow/point_shadows_depth.fs", "shader/shadow/point_shadows_depth.gs");
    Shader skyboxShader("shader/skybox/skybox.vs", "shader/skybox/skybox.fs");
    Shader screenShader("shader/framebuffer/framebuffers_screen.vs", "shader/framebuffer/framebuffers_screen.fs");
    Shader particleshader("shader/particle.vs", "shader/particle.fs", "shader/particle.gs");
    // load textures
    // -------------
    groundTexture = loadTexture("resources/textures/ground2.png");
    planeTexture = loadTexture("resources/textures/stone.png");

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // load obj&ground
    bool p = loadOBJ("resources/plane.obj", plane_vertices, plane_uvs, plane_normals);
    bool g = loadGround("resources/textures/shape.png", ground_vertices, ground_uvs, ground_normals);

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // attach it
    // complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // skybox
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // skybox tex
    vector<std::string> faces
    {
        "resources/textures/solar/right.jpg",
        "resources/textures/solar/left.jpg",
        "resources/textures/solar/top.jpg",
        "resources/textures/solar/bottom.jpg",
        "resources/textures/solar/front.jpg",
        "resources/textures/solar/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("depthMap", 1);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // lighting info
    // -------------
    glm::vec3 lightPos(0.0f, 10.0f, 2.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_CULL_FACE);
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (judge) {
            camera.ProcessKeyboard(FORWARD, deltaTime * 0.1);
            camera.ProcessMouseMovement(deltaTime * xrotate, deltaTime * yrotate);
        }
        if (camera.Position.y < -5.5f) shake = true;
        else shake = false;
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
        float near_plane = 0.1f;
        float far_plane = 100.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // 1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        simpleDepthShader.setFloat("far_plane", far_plane);
        simpleDepthShader.setVec3("lightPos", lightPos);
        renderScene(simpleDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal 
        // -------------------------
        if (shake) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glEnable(GL_DEPTH_TEST);
        }

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set lighting uniforms
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        shader.setInt("shadows", shadows);
        shader.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, planeTexture);

        renderScene(shader);
        glDisable(GL_CULL_FACE);
        auto boat_model_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, -10.0f));
        p1.draw_flow(particleshader, projection, view, boat_model_mat);
        p2.draw_flow(particleshader, projection, view, boat_model_mat);
        p3.draw_flow(particleshader, projection, view, boat_model_mat);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS); // set depth function back to default
        glBindVertexArray(0);

        if (shake) {
            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
            // clear all relevant buffers
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
            glClear(GL_COLOR_BUFFER_BIT);

            screenShader.use();
            screenShader.setFloat("time", lastFrame);
            GLfloat offset = 1.0f / 300.0f;
            GLfloat offsets[9][2] = {
                { -offset,  offset  },  // ����
                {  0.0f,    offset  },  // ����
                {  offset,  offset  },  // ����
                { -offset,  0.0f    },  // ����
                {  0.0f,    0.0f    },  // ����
                {  offset,  0.0f    },  // ����
                { -offset, -offset  },  // ����
                {  0.0f,   -offset  },  // ����
                {  offset, -offset  }   // ����
            };
            glUniform2fv(glGetUniformLocation(screenShader.ID, "offsets"), 9, (GLfloat*)offsets);
            GLfloat blur_kernel[9] = {
                1.0 / 16, 2.0 / 16, 1.0 / 16,
                2.0 / 16, 4.0 / 16, 2.0 / 16,
                1.0 / 16, 2.0 / 16, 1.0 / 16
            };
            glUniform1fv(glGetUniformLocation(screenShader.ID, "blur_kernel"), 9, blur_kernel);
            glBindVertexArray(quadVAO);
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// renders the 3D scene
// --------------------
void renderScene(const Shader& shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(25.0f, -15.0f, 10.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(3.0f));
    shader.setInt("diffuseTexture", 0);
    shader.setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    shader.setMat4("model", model);
    renderGround();

    shader.setInt("reverse_normals", 0); // and of course disable it
    model = glm::mat4(1.0f);
    shader.setMat4("view", model);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.0f));
    //model = glm::translate(model, camera.Position);
    model = glm::rotate(model, glm::radians(xrotate), glm::vec3(0.0f, 0.0f, -1.0f));
    model = glm::rotate(model, glm::radians(yrotate), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    shader.setMat4("model", model);
    shader.setInt("diffuseTexture", 2);
    renderPlane();
}

void renderPlane()
{
    // initialize (if necessary)
    if (plane_VAO == 0)
    {
        glGenVertexArrays(1, &plane_VAO);
        glGenBuffers(1, &plane_vertices_VBO);

        glBindVertexArray(plane_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, plane_vertices_VBO);
        glBufferData(GL_ARRAY_BUFFER, plane_vertices.size() * sizeof(glm::vec3), &plane_vertices[0], GL_STATIC_DRAW);

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glGenBuffers(1, &plane_normals_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, plane_normals_VBO);
        glBufferData(GL_ARRAY_BUFFER, plane_normals.size() * sizeof(glm::vec3), &plane_normals[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glGenBuffers(1, &plane_uvs_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, plane_uvs_VBO);
        glBufferData(GL_ARRAY_BUFFER, plane_uvs.size() * sizeof(glm::vec2), &plane_uvs[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }
    // render Plane

    glBindVertexArray(plane_VAO);
    glDrawArrays(GL_TRIANGLES, 0, plane_vertices.size());
    glBindVertexArray(0);
}
void renderGround()
{
    // initialize (if necessary)
    if (ground_VAO == 0)
    {
        glGenVertexArrays(1, &ground_VAO);
        glGenBuffers(1, &ground_vertices_VBO);

        glBindVertexArray(ground_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, ground_vertices_VBO);
        glBufferData(GL_ARRAY_BUFFER, ground_vertices.size() * sizeof(glm::vec3), &ground_vertices[0], GL_STATIC_DRAW);

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glGenBuffers(1, &ground_normals_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, ground_normals_VBO);
        glBufferData(GL_ARRAY_BUFFER, ground_normals.size() * sizeof(glm::vec3), &ground_normals[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glGenBuffers(1, &ground_uvs_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, ground_uvs_VBO);
        glBufferData(GL_ARRAY_BUFFER, ground_uvs.size() * sizeof(glm::vec2), &ground_uvs[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }
    // render Ground


    glBindVertexArray(ground_VAO);
    glDrawArrays(GL_TRIANGLES, 0, ground_vertices.size());
    glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (judge) {

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            xrotate -= deltaTime * 10;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            xrotate += deltaTime * 10;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            yrotate += deltaTime * 3;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            yrotate -= deltaTime * 8;
        }
        if (xrotate < -40.0f) xrotate = -40.0f;
        if (xrotate > 40.0f) xrotate = 40.0f;
        if (yrotate < -20.0f) yrotate = -20.0f;
        if (yrotate > 20.0f) yrotate = 20.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        judge = false;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        judge = true;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        shake = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //float xpos = static_cast<float>(xposIn);
    //float ypos = static_cast<float>(yposIn);
    //if (firstMouse)
    //{
    //    lastX = xpos;
    //    lastY = ypos;
    //    firstMouse = false;
    //}

    //float xoffset = xpos - lastX;
    //float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    //lastX = xpos;
    //lastY = ypos;

    //camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
