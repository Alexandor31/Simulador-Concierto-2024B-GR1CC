#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(3.0f, 0.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Exercise 16 Task 3", NULL, NULL);
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
        
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("shaders/shader_exercise16_mloading.vs", "shaders/shader_exercise16_mloading.fs");

   
    // load models
    // -----------
    Model landscapeModel("model/map-landscape/scene.obj");
    Model concertModel("model/map-concert/scene.obj");
    Model tvModel("model/television/scene.obj");
    Model speakersModel("model/speakers/scene.obj");
    Model charModel("model/character/scene.obj");
    Model djModel("model/dj/scene.obj");
    Model dronModel("model/dron/scene.obj");

    //Struct de luces
    struct Light {
        glm::vec3 position;
        glm::vec3 direction;
        float cutOff;
        float outerCutOff;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };

    Light lights[5];

    // shader configuration
    // --------------------
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);

    //Posicion de la luces
    glm::vec3 ligthPos[5];
    ligthPos[0] = glm::vec3(1.2f, 1.0f, 2.0f);
    ligthPos[1] = glm::vec3(3.0f, 2.0f, -2.0f);
    ligthPos[2] = glm::vec3(2.0f, 2.5f, 2.0f);
    ligthPos[3] = glm::vec3(0.0f, 3.0f, -3.0f);
    ligthPos[4] = glm::vec3(6.0f, 1.5f, 3.0f);


    
    camera.MovementSpeed = 5.0; //Velocidad de la camara

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glUniform1f(glGetUniformLocation(ourShader.ID, "time"), currentFrame);
        float lightMovementSpeed = 0.5f;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        ourShader.use();

        
        // Inicializar las luces
        for (int i = 0; i < 5; i++) {
            lights[i].position = ligthPos[i];  
            lights[i].direction = glm::vec3(0.0f, -1.0f, 0.0f);       
            lights[i].ambient = glm::vec3(0.1f, 0.1f, 0.1f);
            lights[i].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            lights[i].specular = glm::vec3(1.0f, 1.0f, 1.0f);
            lights[i].cutOff = glm::cos(glm::radians(12.5f));
            lights[i].outerCutOff = glm::cos(glm::radians(17.5f));
            lights[i].constant = 1.0f;
            lights[i].linear = 0.09f;
            lights[i].quadratic = 0.032f;
        }

        // Pasar las luces al shader
        for (int i = 0; i < 5; i++) {
            float angle = currentFrame * lightMovementSpeed;
            float radius = 5.0f;
            lights[i].position = glm::vec3(radius * cos(angle), 2.0f, radius * sin(angle));
            lights[i].direction = glm::vec3(sin(angle), 0.0f, cos(angle));
            std::string index = std::to_string(i);
            ourShader.setVec3("lights[" + index + "].position", lights[i].position);
            ourShader.setVec3("lights[" + index + "].direction", lights[i].direction);
            ourShader.setFloat("lights[" + index + "].cutOff", lights[i].cutOff);
            ourShader.setFloat("lights[" + index + "].outerCutOff", lights[i].outerCutOff);
            ourShader.setVec3("lights[" + index + "].ambient", lights[i].ambient);
            ourShader.setVec3("lights[" + index + "].diffuse", lights[i].diffuse);
            ourShader.setVec3("lights[" + index + "].specular", lights[i].specular);
            ourShader.setFloat("lights[" + index + "].constant", lights[i].constant);
            ourShader.setFloat("lights[" + index + "].linear", lights[i].linear);
            ourShader.setFloat("lights[" + index + "].quadratic", lights[i].quadratic);
        }

         
        
        // material properties
        ourShader.setFloat("material.shininess", 32.0f);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Renderizar el primer modelo (map-landscape)
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, glm::vec3(-3.0f, 0.0f, 0.0f)); 
        model1 = glm::scale(model1, glm::vec3(0.1f, 0.1f, 0.1f));
        ourShader.setMat4("model", model1);
        landscapeModel.Draw(ourShader);

        // Renderizar el segundo modelo (map-concert)
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(3.0f, 0.0f, 0.0f)); 
        model2 = glm::scale(model2, glm::vec3(0.1f, 0.1f, 0.1f));
        ourShader.setMat4("model", model2);
        concertModel.Draw(ourShader);

        // Renderizar el segundo modelo (television)
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(5.5f, 0.0f, 0.0f));
        model3 = glm::scale(model3, glm::vec3(0.02f, 0.02f, 0.02f)); 
        ourShader.setMat4("model", model3);
        tvModel.Draw(ourShader);

        // Renderizar la segunda TV (duplicado)
        glm::mat4 model4 = glm::mat4(1.0f);
        model4 = glm::translate(model4, glm::vec3(1.0f, 0.0f, 0.0f)); 
        model4 = glm::scale(model4, glm::vec3(0.02f, 0.02f, 0.02f)); 
        ourShader.setMat4("model", model4);
        tvModel.Draw(ourShader);

        // Renderizar la speakers (speakers)
        glm::mat4 model5 = glm::mat4(1.0f);
        model5 = glm::translate(model5, glm::vec3(5.5f, 0.0f, 2.0f)); 
        model5 = glm::rotate(model5, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model5 = glm::scale(model5, glm::vec3(0.03f, 0.03f, 0.03f));  
        ourShader.setMat4("model", model5);
        speakersModel.Draw(ourShader);

        // Renderizar la speakers (speakers2)
        glm::mat4 model6 = glm::mat4(1.0f);
        model6 = glm::translate(model6, glm::vec3(1.0f, 0.0f, 2.0f)); 
        model6 = glm::rotate(model6, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model6 = glm::scale(model6, glm::vec3(0.03f, 0.03f, 0.03f)); 
        ourShader.setMat4("model", model6);
        speakersModel.Draw(ourShader);

        // Renderizar la character (character)
        glm::mat4 model7 = glm::mat4(1.0f);
        model7 = glm::translate(model7, glm::vec3(3.0f, 0.2f, 0.001f));
        model7 = glm::scale(model7, glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setMat4("model", model7);
        charModel.Draw(ourShader);

        // Renderizar la dj (dj)
        glm::mat4 model8 = glm::mat4(1.0f);
        model8 = glm::translate(model8, glm::vec3(3.0f, 0.4f, 0.005f)); 
        model8 = glm::scale(model8, glm::vec3(0.003f, 0.003f, 0.003f)); 
        ourShader.setMat4("model", model8);
        djModel.Draw(ourShader);

        // Renderizar la dron (dron)
        float radius = 1.0f;
        float angle = currentFrame * lightMovementSpeed;

        glm::mat4 model9 = glm::mat4(1.0f);
        model9 = glm::translate(model9, glm::vec3(1.2f, 1.0f, 2.0f)); 
        model9 = glm::rotate(model9, angle, glm::vec3(-sin(angle), -1.0f, -cos(angle))); 
        model9 = glm::translate(model9, glm::vec3(radius * -cos(angle), 0.0f, radius * sin(angle))); 
        model9 = glm::scale(model9, glm::vec3(0.05f, 0.05f, 0.05f));  
        ourShader.setMat4("model", model9);
        dronModel.Draw(ourShader);
               
        glm::mat4 model10 = glm::mat4(1.0f);
        model10 = glm::translate(model10, glm::vec3(3.0f, 2.0f, -2.0f)); 
        model10 = glm::rotate(model10, angle, glm::vec3(-sin(angle), -1.0f, -cos(angle))); 
        model10 = glm::translate(model10, glm::vec3(radius * -cos(angle), 0.0f, radius * -sin(angle))); 
        model10 = glm::scale(model10, glm::vec3(0.05f, 0.05f, 0.05f));  
        ourShader.setMat4("model", model10);
        dronModel.Draw(ourShader);

        glm::mat4 model11 = glm::mat4(1.0f);
        model11 = glm::translate(model11, glm::vec3(2.0f, 2.5f, 2.0f)); 
        model11 = glm::rotate(model11, angle, glm::vec3(sin(angle), -1.0f, -cos(angle))); 
        model11 = glm::translate(model11, glm::vec3(radius * -cos(angle), 0.0f, radius * sin(angle)));
        model11 = glm::scale(model11, glm::vec3(0.05f, 0.05f, 0.05f));  
        ourShader.setMat4("model", model11);
        dronModel.Draw(ourShader);

        glm::mat4 model12 = glm::mat4(1.0f);
        model12 = glm::translate(model12, glm::vec3(0.0f, 3.0f, -3.0)); 
        model12 = glm::rotate(model12, angle, glm::vec3(sin(angle), -1.0f, cos(angle))); 
        model12 = glm::translate(model12, glm::vec3(radius * -cos(angle), 0.0f, radius * -sin(angle))); 
        model12 = glm::scale(model12, glm::vec3(0.05f, 0.05f, 0.05f));  
        ourShader.setMat4("model", model12);
        dronModel.Draw(ourShader);

        glm::mat4 model13 = glm::mat4(1.0f);
        model13 = glm::translate(model13, glm::vec3(6.0f, 1.5f, 3.0));
        model13 = glm::rotate(model13, angle, glm::vec3(sin(angle), -1.0f, -cos(angle)));
        model13 = glm::translate(model13, glm::vec3(radius * -cos(angle), 0.0f, radius * sin(angle))); 
        model13 = glm::scale(model13, glm::vec3(0.05f, 0.05f, 0.05f));  
        ourShader.setMat4("model", model13);
        dronModel.Draw(ourShader);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // Ajusta este valor para suavizar el movimiento del mouse
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
