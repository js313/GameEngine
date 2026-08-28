#ifndef KEYBOARDCONTROLLEDCOMPONENT_H
#define KEYBOARDCONTROLLEDCOMPONENT_H

#include <glm/glm.hpp>

struct KeyboardControlledComponent
{
    glm::vec2 upVelocity;
    glm::vec2 rightVelocity;
    glm::vec2 downVelocity;
    glm::vec2 leftVelocity;

    KeyboardControlledComponent(
        glm::vec2 upVelocity = glm::vec2(0.0f, 0.0f),
        glm::vec2 rightVelocity = glm::vec2(0.0f, 0.0f),
        glm::vec2 downVelocity = glm::vec2(0.0f, 0.0f),
        glm::vec2 leftVelocity = glm::vec2(0.0f, 0.0f))
        : upVelocity(upVelocity),
          rightVelocity(rightVelocity),
          downVelocity(downVelocity),
          leftVelocity(leftVelocity) {}
};

#endif