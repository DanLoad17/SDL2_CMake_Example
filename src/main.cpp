#include "Engine.h"

int main(int argc, char* argv[]) {
    Engine engine("DANGAME33", 640, 480);
    if (engine.init()) {
        engine.run();
    }
    engine.cleanup();
    return 0;
}
