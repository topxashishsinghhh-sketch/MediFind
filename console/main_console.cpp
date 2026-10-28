#include "console/ConsoleApp.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string dbPath = "medifind.db";
    if (argc > 1) {
        dbPath = argv[1];
    }

    try {
        medifind::console::ConsoleApp app(dbPath);
        app.run();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "\n[FATAL ERROR]: " << ex.what() << std::endl;
        return 1;
    }
}
