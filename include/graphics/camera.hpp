#ifndef CAMERA_H
#define CAMERA_H
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/fwd.hpp"
#include "window.hpp"

using namespace glm;

enum CAMERA_TYPE {
    ORTHO = 0,
    PERSP = 1
};

// Define the orthographic projection parameters
const float left = -10.0f;
const float right = 10.0f;
const float bottom = -10.0f;
const float top = 10.0f;
const float nearPlane = 0.1f;
const float farPlane = 100.0f;

const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 68.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera {

public:
    Camera(vec3 position, CAMERA_TYPE inType){
        cameraPos = position;
        updateCamVectors();
        type = inType;
        worldUp = glm::vec3(0.0, 1.0, 0.0);
        cameraFront = glm::vec3(0.0, 0.0, -1.0f);
        cameraUp = glm::vec3(0.0, 1.0, 0.0);

    }

    ~Camera();

    CAMERA_TYPE type;

    vec3 worldUp, upAxis, cameraUp;
    vec3 cameraPos, cameraTarget, cameraDirection, cameraFront, cameraRight;
    float cameraPitch, cameraYaw;
    mat4 view;
    
    
    mat4 getView() {
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    mat4 getProjection() {
        // printf("Type %d\n", type);
        if (type == ORTHO){
            return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
        } else if (type == PERSP) {
            // printf("PERSP!!!");
            return glm::perspective(glm::radians(FOV), 
                    (float)Window::getWidth() / (float)Window::getHeight(),
                    // (float)Window::getWidth() / (float)Window::getHeight(), 
                    0.1f, 500.0f);
        }
        return glm::mat4(1.0f);
    }


private:
     void updateCamVectors() {
         glm::vec3 front;
         front.x = cos(glm::radians(cameraYaw)) * glm::cos(glm::radians(cameraPitch));
         front.y = sin(glm::radians(cameraPitch));
         front.z = sin(glm::radians(cameraYaw)) * glm::cos(glm::radians(cameraPitch));
    
         cameraFront = glm::normalize(front);
         cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
         cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
     }
    

};
#endif
