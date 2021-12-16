#include "Camera.h"

Camera::Camera(glm::vec3 pos) {
	cameraPos = pos;
	cameraOrient = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	cameraUp = glm::vec3(0.f, 1.f, 0.f);
	cameraTarget = glm::vec3(0.f, 1000.f, 0.f);
	firstMouse = true;
	lastX = 250;
	lastY = 250;
	firstMouse = true;
	directions = glm::vec3(0, 0, 0);
	rotation.x = 0;
	rotation.y = 0;
}

void Camera::move(glm::vec3 directions, glm::vec2 rotations, float deltatime) {
	auto pitch = glm::quat(glm::vec3(-rotations.y, 0, 0.f));
	auto yaw = glm::quat(glm::vec3(0, -rotations.x, 0.f));

	cameraOrient = glm::normalize(yaw * cameraOrient * pitch);

	auto camera_roll_direction = cameraOrient * glm::vec3(0, 0, 1);
	auto camera_pitch_direction = cameraOrient * glm::vec3(1, 0, 0);

	// forward/backward move - all axes could be affected
	cameraPos += directions.x * camera_roll_direction * deltatime * cameraSpeed;
	// left and right strafe - only xz could be affected    
	cameraPos += directions.y * camera_pitch_direction * deltatime * cameraSpeed;
	// up and down flying - only y-axis could be affected
	cameraPos.y += directions.z * deltatime * cameraSpeed;

	cameraTarget = cameraPos + glm::vec3(0.f, -1.f, 0.f) + (camera_roll_direction * 5.f);

	viewMatrix = glm::lookAt(cameraPos, cameraPos + camera_roll_direction, glm::cross(camera_roll_direction, camera_pitch_direction));
	cameraUp = glm::cross(camera_roll_direction, camera_pitch_direction);

}

void Camera::processInput(GLFWwindow* window, float deltatime, bool crashed) {
	directions = glm::vec3(0, 0, 0);
	//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) directions.x = 1;
	//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) directions.x = -1;
	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) directions.y = 1;
	//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) directions.y = -1;
	//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) directions.z = -1;
	//if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) directions.z = 1;

	rotation = glm::vec2(0, 0);
	if (!crashed) {
		directions.x = 1;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) rotation.y = 0.03f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) rotation.y = -0.03f;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) rotation.x = -0.03f;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) rotation.x = 0.03f;

		move(directions, rotation, deltatime);
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	rotation = glm::vec2(0, 0);
}

void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	rotation.x = xoffset * 0.01f;
	rotation.y = -yoffset * 0.01f;
	//printf("%f, %f, %f, %f\n", lastX, lastY, rotation.x, rotation.y);

}

glm::mat4 Camera::getView() {
	return viewMatrix;
}

glm::vec3 Camera::getPos() {
	return cameraPos;
}

glm::quat Camera::getOrient() {
	return cameraOrient;
}

glm::vec3 Camera::getUp() {
	return cameraUp;
}

glm::vec3 Camera::getTarget() {
	return cameraTarget;
}
