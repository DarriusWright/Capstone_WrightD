#pragma once
#include <glm.hpp>
static const float CAMERA_SPEED = 2.5f;

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0,0,0) , glm::vec3 lookAt = glm::vec3(0,0,-1), float distance = 200.0f);
	~Camera(void);

	glm::vec4 u;
	glm::vec4 v;
	glm::vec4 w;
	glm::vec4 up;
	glm::vec4 lookAt;
	glm::vec4 position;
	float distance;
	float dum[3];
	
	



	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void moveIn();
	void moveOut();

	void update();
};

