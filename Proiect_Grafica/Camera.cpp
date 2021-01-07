#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, this->cameraUpDirection));

        this->angleX = 0;
        this->angleY = 0;
        //TODO - Update the rest of camera parameters

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraDirection * speed;
            cameraTarget += cameraDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraDirection * speed;
            cameraTarget -= cameraDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
            break;
        case MOVE_DOWN:
            cameraPosition -= cameraUpDirection * speed;
            cameraTarget -= cameraUpDirection * speed;
            break;
        case MOVE_UP:
            cameraPosition += cameraUpDirection * speed;
            cameraTarget += cameraUpDirection * speed;
            break;
        }

    }

        void Camera::rotate(float pitch, float yaw) {

            this->angleY += yaw;
            this->angleX += pitch;
            cameraTarget = glm::vec3(cameraPosition.x + sin(angleY), cameraPosition.y + sin(angleX), cameraPosition.z - cos(angleY));

            /*cameraTarget = glm::vec3(cameraPosition.x , cameraPosition.y + cos(angleY-angleX), cameraPosition.z - sin(angleY-angleX));
            glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
            cameraTarget.y = cameraDirection.y + cos(pitch) + sin(pitch);*/
            this->cameraDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
            this->cameraRightDirection = glm::cross(this->cameraDirection, this->cameraUpDirection);
        }

        glm::mat4 Camera::getSmallLightPointMatrix() {
            glm::vec3 start = glm::vec3(0.0f, 1.3533f, -0.002f);
            glm::vec3 end = glm::vec3(-0.15f, 0.40f, -0.002f);
            return glm::lookAt(start, end, cameraDirection);
        }




    }
