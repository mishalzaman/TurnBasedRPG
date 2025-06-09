#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace GLFW {
	class WindowHandler {
	public:
		WindowHandler(std::string title, int screenWidth, int screenHeight);
		~WindowHandler();

		bool ShouldClose() const;
		void PollEvents();

		GLFWwindow* getWindow() const { return window_; }
		int getScreenWidth() const { return screenWidth_; }
		int getScreenHeight() const { return screenHeight_; }
	private:
		GLFWwindow* window_;
		int screenWidth_;
		int screenHeight_;
	};
}