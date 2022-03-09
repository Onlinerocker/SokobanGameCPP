#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
    private:
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 worldUp;
	glm::quat rotation; //combine yaw and pitch
    
	float yaw;
	float pitch;
    
    public:
	Camera(glm::vec3 position, float yaw, float pitch, glm::vec3 worldUp);
	//~Camera();
    
    glm::vec3 position;
	glm::mat4 getViewMatrix();
	glm::vec3 getForward();
	glm::vec3 getRight();
    
	void setPosition(glm::vec3 pos);
	void setYaw(float yaw);
	void setPitch(float pitch);
    
	void move(glm::vec3 deltaPos);
	void rotate(float deltaP, float deltaY);
    
};

