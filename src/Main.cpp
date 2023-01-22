#include "Application.hpp"

#include <stdexcept>

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::trace);

    planets::Application *app;

    try {
        app = new planets::Application(argc, argv);
    } catch (std::exception &e) {
        exit(-1);
    }

    app->loop();

    delete app;

    return EXIT_SUCCESS;
}