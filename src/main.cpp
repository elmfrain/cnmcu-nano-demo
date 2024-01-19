#include <Visualizer.hpp>
#include <Logger.hpp>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static em::Logger logger("Main");
em::VisualizerApp application;

static void runLoop()
{
    application.runLoop();
}

int main(int argc, char** argv)
{
    em::AppParams options;

    options.width = 720;
    options.height = 720;
    
    if(!application.start(options))
    {
        logger.fatalf("Unable to start application!");
        return -1;
    }

#ifndef EMSCRIPTEN
    while(!application.shouldClose())
    {
        if(!application.runLoop())
        {
            logger.fatalf("Application loop has suffered a problem");
            return -2;
        }
    }
#else
    emscripten_set_main_loop(runLoop, 0, true);
#endif

    if(!application.terminate())
    {
        logger.errorf("Application terminated ungracefully");
        return -3;
    }

    return 0;
}