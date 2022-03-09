#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, float yaw, float pitch, glm::vec3 worldUp)
{
	this->position = position;
	this->worldUp = worldUp;
	this->yaw = yaw;
	this->pitch = pitch;

	glm::quat p = glm::angleAxis(glm::radians(pitch), glm::vec3(0.0f, 1.0f, 0.));
	rotation = p;
	forward = glm::conjugate(this->rotation) * glm::vec3(0, 0, -1) * this->rotation;
	right = glm::conjugate(this->rotation) * glm::vec3(1, 0, 0) * this->rotation;
	right = glm::normalize(right);

	glm::quat y = glm::angleAxis(glm::radians(yaw), right);
	rotation = y;
	forward = glm::conjugate(this->rotation) * glm::vec3(0, 0, -1) * this->rotation;
	up = glm::cross(right, forward);
	up = glm::normalize(up);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + forward, up);
}

glm::vec3 Camera::getForward()
{
	return forward;
}

glm::vec3 Camera::getRight()
{
	return right;
}

void Camera::setPosition(glm::vec3 pos)
{
	position = pos;
}

void Camera::setYaw(float yaw)
{
	this->yaw = yaw;
}

void Camera::setPitch(float pitch)
{
	this->pitch = pitch;
}

void Camera::move(glm::vec3 deltaPos)
{
	position += deltaPos;
}

void Camera::rotate(float deltaP, float deltaY)
{
	pitch += deltaP;
	yaw += deltaY;

	glm::quat p = glm::angleAxis(glm::radians(deltaP/2), glm::vec3(0.0f, 1.0f, 0.));
	rotation = p;
	forward = glm::conjugate(this->rotation) * this->forward * this->rotation;
	right = glm::conjugate(this->rotation) * this->right * this->rotation;
	right = glm::normalize(right);

	glm::quat y = glm::angleAxis(glm::radians(deltaY/2), right);
	rotation = y;
	forward = glm::conjugate(this->rotation) * this->forward * this->rotation;
	up = glm::cross(right, forward);
	up = glm::normalize(up);
}