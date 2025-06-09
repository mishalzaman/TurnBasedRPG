#pragma once

#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
#include <string>
#include <memory>
#include <iostream>

#include "glfw/WindowHandler.h"
#include <stdexcept>

class Window {
public:
    static Window& retrieveInstance() {
        static Window instance;
        return instance;
    }

    /*============================================================
    * INITIALIZE Window
    =============================================================*/

    bool Activate(
		const std::string& title,
        int resolutionWidth,
        int resolutionHeight
    ) {
        windowHandler_ = std::make_unique<GLFW::WindowHandler>(title, resolutionWidth, resolutionHeight);
        if (!windowHandler_ || !windowHandler_->getWindow()) {
            throw std::runtime_error("Failed to create window");
        }

        return windowHandler_ != nullptr;
    }

    /*============================================================
    * STATIC ACCESSORS
    =============================================================*/

    static GLFWwindow* Get() {
        auto& instance = retrieveInstance();
        if (!instance.windowHandler_) {
            throw std::runtime_error("WindowHandler_ is not initialized.");
        }
        return instance.windowHandler_->getWindow();
    }
	static int Width() { return retrieveInstance().windowHandler_->getScreenWidth(); }
	static int Height() { return retrieveInstance().windowHandler_->getScreenHeight(); }

    // Rule of 5
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

private:
    Window() = default;
    ~Window() {
        std::cout << "Window::~Window - Releasing window..." << std::endl;
        windowHandler_.reset();
    }

    std::unique_ptr<GLFW::WindowHandler> windowHandler_;

};