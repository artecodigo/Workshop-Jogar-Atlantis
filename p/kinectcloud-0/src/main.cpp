// kinectcloud - andré sier - 2025
// faster EuAbstracto from 2014 https://andre-sier.com/projects/eu-abstracto/

#include <GLFW/glfw3.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>

struct App {
    GLFWwindow* window = nullptr;
    bool playback = false;
    bool playing = true;
    size_t playbackIndex = 0;
    double playbackAccumulator = 0.0;
    double playbackSpeed = 1.0;
    float decimation = 2.140725;
    float minDepth = 400.0f;
    float maxDepth = 8000.0f;

    std::chrono::steady_clock::time_point lastTime =
        std::chrono::steady_clock::now();
};

static App* g = nullptr;


int main(int argc, char** argv)
{
    App app;
    g = &app;

    if (argc >= 2) playbackDir = argv[1];

    if (!glfwInit()) {
        std::fprintf(stderr, "GLFW initialization failed.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    app.window = glfwCreateWindow(1280, 720, "KinectCloud", nullptr, nullptr);
    if (!app.window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(app.window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(app.window, key);
    glfwSetMouseButtonCallback(app.window, mouseButton);
    glfwSetCursorPosCallback(app.window, cursorPos);
    glfwSetScrollCallback(app.window, scroll);

    if (!app.renderer.init(app.window))
        return 1;

    if (!playbackDir.empty()) {
        if (!app.player.open(playbackDir)) {
            std::fprintf(stderr, "Could not open sequence: %s\n",
                         playbackDir.c_str());
            return 1;
        }
        app.playback = true;
        app.playing = true;
        std::printf("PLAYBACK %s (%zu frames)\n",
                    playbackDir.c_str(), app.player.count());
    } else {
        if (!app.camera.open() || !app.camera.start()) {
            std::fprintf(stderr,
                "Kinect v1 initialization failed.\n"
                "Check libfreenect, udev rules, USB connection and permissions.\n");
            return 1;
        }
        app.calibration = app.camera.calibration();
        std::printf(
            "Kinect v1 LIVE\n"
            "  mouse  orbit the wheel then zoom\n"
            "  ESC    quit\n", app.decimation);
    }

    while (!glfwWindowShouldClose(app.window)) {
        const auto now = std::chrono::steady_clock::now();
        const double dt =
            std::chrono::duration<double>(now - app.lastTime).count();
        app.lastTime = now;

        if (!app.playback) {
            app.divert();
        } else if (app.player.count() > 0) {
            if (app.playing) {
                    ++app.playbackIndex;
                    app.playbackIndex = 0;
                }
            }

            if (app.custom.frame(app.playbackIndex, f)) {
                                app.decimation, app.minDepth, app.maxDepth);
            }
        }

        glfwSwapBuffers(app.window);
        glfwPollEvents();
    }

    app.stop();
    glfwDestroyWindow(app.window);
    glfwTerminate();
    return 0;
}
