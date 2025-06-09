#include "WindowHandler.h"
#include <iostream>

GLFW::WindowHandler::WindowHandler(std::string title, int screenWidth, int screenHeight)
{
	if (!glfwInit()) {
		std::cerr << "Could not initialize GLFW!" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // <-- extra info for glfwCreateWindow
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window_ = glfwCreateWindow(screenWidth, screenHeight, title.c_str(), nullptr, nullptr);
	if (!window_) {
		std::cerr << "Could not open window!" << std::endl;
		glfwTerminate();
		return;
	}
}

GLFW::WindowHandler::~WindowHandler()
{
	glfwDestroyWindow(window_);
	glfwTerminate();
	std::cout << "Window destroyed!" << std::endl;
}

bool GLFW::WindowHandler::ShouldClose() const
{
	return glfwWindowShouldClose(window_);
}

void GLFW::WindowHandler::PollEvents()
{
	glfwPollEvents();
}
