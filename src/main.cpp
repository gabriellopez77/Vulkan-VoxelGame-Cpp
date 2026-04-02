#include "Application.h"

#include "Game.h"


i32 main() {
    Application app;
    Game game;

    app.initWindow(1000, 800, "Vulkan App");
    app.run(&game);
    app.clear();
}