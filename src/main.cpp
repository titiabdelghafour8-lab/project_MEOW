#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstdint>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

// Pixel color helper (RGBA packed into 32-bit integer)
uint32_t create_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (a << 24) | (b << 16) | (g << 8) | r;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "C++ Software Rasterizer", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Our CPU Framebuffer: flat array storing WIDTH * HEIGHT pixels
    std::vector<uint32_t> framebuffer(WIDTH * HEIGHT, 0);

    // Create OpenGL Texture to display our raw CPU pixel buffer
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // -------------------------------------------------------------
        // 1. RASTERIZER CLEAR & DRAW STEP
        // -------------------------------------------------------------
        // Clear framebuffer (Black background)
        std::fill(framebuffer.begin(), framebuffer.end(), create_rgba(20, 20, 20));

        // Draw a procedural test pattern directly into raw memory
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                // Example: Draw a gradient
                uint8_t r = static_cast<uint8_t>((float)x / WIDTH * 255);
                uint8_t g = static_cast<uint8_t>((float)y / HEIGHT * 255);

                // Write pixel directly to the flat 1D index
                framebuffer[y * WIDTH + x] = create_rgba(r, g, 128);
            }
        }

        // -------------------------------------------------------------
        // 2. DISPLAY STEP (Pass CPU buffer to GPU Texture & render quad)
        // -------------------------------------------------------------
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());

        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f); // Bottom-left
            glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f); // Bottom-right
            glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f); // Top-right
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f); // Top-left
        glEnd();

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &texture_id);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}