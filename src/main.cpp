
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"

using Clock = std::chrono::high_resolution_clock;
DefaultResolution defaultResolution;
TwoPlayer twoPlayer;
HelpMode helpMode;
StoryMode storyMode;
MenuMode menuMode;

// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	world.setPlayerMode();
	world.setResolution();

	RenderSystem renderer;
	PhysicsSystem physics;
	AISystem ai;

	// Initializing window
	GLFWwindow* window = world.create_window(defaultResolution.width, defaultResolution.height);
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	renderer.init(w, h, window);
	world.init(&renderer);
	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Get new screen dimensions
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		// Processes system messages, if this wasn't present the window would become
		// unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		// TODO: JASMINE (MAKE IT SO THAT CLICKING IN SOME RANGE WILL BRING IT TO STORY MODE)
		if (menuMode.menuType == 1) {
			world.createMenu();
		}

		if (!helpMode.inHelpMode && !storyMode.firstLoad && menuMode.menuType == 0) {
			world.step(elapsed_ms);
			ai.step(elapsed_ms, (float)width, (float)height);
			physics.step(elapsed_ms, (float)width, (float)height);
			physics.handle_collision();
		}

		renderer.draw();
	}

	return EXIT_SUCCESS;
}

