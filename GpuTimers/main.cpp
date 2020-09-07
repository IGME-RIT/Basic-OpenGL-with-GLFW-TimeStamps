/*
Title: Normal Maps
File Name: main.cpp
Copyright � 2016, 2019
Author: David Erbelding, Niko Procopi
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "FreeImage.h"
#include <vector>
#include "mesh.h"
#include "fpsController.h"
#include "transform3d.h"
#include "material.h"
#include "texture.h"
#include <iostream>


// This is the resolution of iPhone 6, which I am personally using
glm::vec2 viewportDimensions = glm::vec2(1366, 768);
glm::vec2 mousePosition = glm::vec2();


// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
    viewportDimensions = glm::vec2(width, height);
}

// This will get called when the mouse moves.
void mouseMoveCallback(GLFWwindow *window, GLdouble mouseX, GLdouble mouseY)
{
    mousePosition = glm::vec2(mouseX, mouseY);
}

#define NikoIphone6 true

int main(int argc, char **argv)
{
	// Initialize GLFW
	glfwInit();

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(viewportDimensions.x, viewportDimensions.y, "Virtual Reality", 
        
        
        // If you want fullscreen, change this 
        // to glfwGetPrimaryMonitor()
        nullptr, 
        
        nullptr);
	
    
    glfwMakeContextCurrent(window);

	// Set window callbacks
	glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // hide mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Initialize glew
	glewInit();

    // The mesh loading code has changed slightly, we now have to do some extra math to take advantage of our normal maps.
    // Here we pass in true to calculate tangents.
    Mesh* model = new Mesh("../Assets/plane.obj", true);
    Mesh* car = new Mesh("../Assets/car.3Dobj", true);

    // The transform being used to draw our second shape.
    Transform3D transform;

    // Make a first person controller for the camera.
    FPSController controller = FPSController();

	// Create Shaders
    Shader* vertexShader = new Shader("../Assets/vertex.glsl", GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader("../Assets/fragment.glsl", GL_FRAGMENT_SHADER);

    // Create A Shader Program
    // The class wraps all of the functionality of a gl shader program.
    ShaderProgram* shaderProgram = new ShaderProgram();
    shaderProgram->AttachShader(vertexShader);
    shaderProgram->AttachShader(fragmentShader);

	// fields that are used in the shader, on the graphics card
	char cameraViewVS[] = "cameraView";
	char worldMatrixVS[] = "worldMatrix";

	char colorTexFS[] = "tex";
	char normalTexFS[] = "tex2";

	// files that we want to open
	char colorTexFile[] = "../Assets/BrickColor.png";
	char normalTexFile[] = "../Assets/BrickNormal.png";

    char blankNorm[] = "../Assets/blankNormal.PNG";

    char colCar[] = "../Assets/car.png";

    // Create a material using a texture for our model
    Material* material = new Material(shaderProgram);

    Texture* colPlaneTex = new Texture(colorTexFile);
    Texture* normPlaneTex = new Texture(normalTexFile);

    Texture* blankNormTex = new Texture(blankNorm);

    Texture* colCarTex = new Texture(colCar);

    glm::mat4 view;
    glm::mat4 viewProjection;

    // Print instructions to the console.
    std::cout << "Use WASD to move, and the mouse to look around." << std::endl;
    std::cout << "Press escape or alt-f4 to exit." << std::endl;

    float rotY = 0;
    float moveX = -0.5f;
    Transform3D temp;

    // Here we prepare to put three timestamps,
    // one before the frame starts, one after 
    // the first eye finishes, one when the
    // entire frame finishes
    GLuint64 startTime, midPoint, stopTime;
    unsigned int queryID[3];

    // generate two queries
    glGenQueries(3, queryID);

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Exit when escape is pressed.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        // Calculate delta time.
        float dt = glfwGetTime();
        // Reset the timer.
        glfwSetTime(0);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            moveX += 0.01f;

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            moveX -= 0.01f;

        if (moveX > 0)
            moveX = 0;

        // dont need to print for now
#if 0
        printf("%f\n", rotY);
#endif

        // Determine if you want to benchmark, by pressing 'b'
        bool benchmarkThisFrame = false;
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
            benchmarkThisFrame = true;


        // Update the player controller
        controller.Update(window, viewportDimensions, mousePosition, dt);
        
        // aspect ratio is 45% of window width/height, 
        // since the eyes have different resolution than window

        // Projection matrix.
        glm::mat4 projection = glm::perspective(0.9f, 
            
#if NikoIphone6
            0.45f * viewportDimensions.x / viewportDimensions.y, 
#else
            0.5f * viewportDimensions.x / viewportDimensions.y,
#endif
            .1f, 100.f);

        if (benchmarkThisFrame)
        {
            // This is added to the GPU command buffer, and 
            // will take effect when the command buffer executes.
            // First stamp is at the beginning of the frame
            glQueryCounter(queryID[0], GL_TIMESTAMP);
        }

        // Clear the depth buffers
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        int screenW = viewportDimensions.x;


#if NikoIphone6
        int sizeX = 45 * screenW / 100; // 48%
#else
        int sizeX = screenW / 2; // 50%
#endif

        glViewport(0, 0, sizeX, viewportDimensions.y);

		// camera
        view = controller.GetTransform().GetInverseMatrix();
        viewProjection = projection * view;
        material->SetMatrix(cameraViewVS, viewProjection);
		
        // plane
        transform.SetPosition(glm::vec3(0, 0, -10));
        transform.SetScale(10.0f);
        material->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material->SetTexture(colorTexFS, colPlaneTex);
        material->SetTexture(normalTexFS, normPlaneTex);
        material->Bind();
        model->Draw();

        // car
        transform.SetPosition(glm::vec3(0, -1, -10));
        transform.SetRotation(glm::vec3(0, 1.7, 0));
        transform.SetScale(1.0f);
        material->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material->SetTexture(colorTexFS, colCarTex);
        material->SetTexture(normalTexFS, blankNormTex);
        material->Bind();
        car->Draw();

        // car (again)
        transform.SetPosition(glm::vec3(5, -1, -10));
        transform.SetRotation(glm::vec3(0, 0, 0));
        transform.SetScale(1.0f);
        material->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material->SetTexture(colorTexFS, colCarTex);
        material->SetTexture(normalTexFS, blankNormTex);
        material->Bind();
        car->Draw();

#if NikoIphone6
        int startX = 55 * screenW / 100; // start at 60%
        
#else
        int startX = screenW / 2;
#endif

        if (benchmarkThisFrame)
        {
            // Set another timestamp after the first eye finishes
            // rendering, and before the second eye starts
            glQueryCounter(queryID[1], GL_TIMESTAMP);
        }

        glViewport(startX, 0, sizeX, viewportDimensions.y);

		// camera
        temp = controller.GetTransform();
        temp.RotateY(rotY);
        view = temp.GetInverseMatrix();
        view = glm::translate(view, glm::vec3(moveX, 0, 0));
        viewProjection = projection * view;
        material->SetMatrix(cameraViewVS, viewProjection);
		
        // plane
        transform.SetPosition(glm::vec3(0, 0, -10));
        transform.SetScale(10.0f);
        material->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material->SetTexture(colorTexFS, colPlaneTex);
        material->SetTexture(normalTexFS, normPlaneTex);
        material->Bind();
        model->Draw();

        // car
        transform.SetPosition(glm::vec3(0, -1, -10));
        transform.SetRotation(glm::vec3(0, 1.7, 0));
        transform.SetScale(1.0f);
        material->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material->SetTexture(colorTexFS, colCarTex);
        material->SetTexture(normalTexFS, blankNormTex);
        material->Bind();
        car->Draw();

        // car (again)
        transform.SetPosition(glm::vec3(5, -1, -10));
        transform.SetRotation(glm::vec3(0, 0, 0));
        transform.SetScale(1.0f);
        material->SetMatrix(worldMatrixVS, transform.GetMatrix());
        material->SetTexture(colorTexFS, colCarTex);
        material->SetTexture(normalTexFS, blankNormTex);
        material->Bind();
        car->Draw();

        if (benchmarkThisFrame)
        {
            // Put a final time stamp when the frame finishes
            glQueryCounter(queryID[2], GL_TIMESTAMP);

            // assume the stamps are not immediately available
            GLint stopTimerAvailable = 0;

            // keep looping while they aren't available
            while (!stopTimerAvailable)
            {
                // Keep checking the status of the last time stamp, to see if the GPU
                // recorded the stamp, and sent it back to the CPU
                glGetQueryObjectiv(queryID[2], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
            }

            // get query results
            glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
            glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &midPoint);
            glGetQueryObjectui64v(queryID[2], GL_QUERY_RESULT, &stopTime);

            system("cls");
            printf("First Eye: %f ms\n", (midPoint - startTime) / 1000000.0);
            printf("Second Eye: %f ms\n", (stopTime - midPoint) / 1000000.0);
            printf("Full Frame: %f ms\n", (stopTime - startTime) / 1000000.0);
        }

		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();
	}

    delete model;

    // Free material should free all objects used by material
    delete material;

	// Free GLFW memory.
	glfwTerminate();

	// End of Program.
	return 0;
}
