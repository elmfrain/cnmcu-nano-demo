#include <Visualizer.hpp>
#include <Logger.hpp>

static em::Logger logger("Main");

int main(int argc, char** argv)
{
    em::AppParams options;

    // options.width = 1280;
    // options.height = 720;

    em::VisualizerApp application;

    if(!application.start(options))
    {
        logger.fatalf("Unable to start application!");
        return -1;
    }

    while(!application.shouldClose())
    {
        if(!application.runLoop())
        {
            logger.fatalf("Application loop has suffered a problem");
            return -2;
        }
    }

    if(!application.terminate())
    {
        logger.errorf("Application terminated ungracefully");
        return -3;
    }

    return 0;
}