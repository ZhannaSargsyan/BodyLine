#include <iostream>
#include <string>
#include <cstring>
#include "TextSimulation.cpp" // Include directly since we're not compiling with SFML

void displayUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                 Display this help message" << std::endl;
    std::cout << "  -w, --walker               Start in Walker mode (default)" << std::endl;
    std::cout << "  -s, --snowball             Start in Snowball mode" << std::endl;
    std::cout << "  -c, --config <file>        Load configuration from file" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    SimulationType simulationType = SimulationType::WALKER;  // Default
    std::string configFile = "config/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            displayUsage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--walker") == 0) {
            simulationType = SimulationType::WALKER;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--snowball") == 0) {
            simulationType = SimulationType::SNOWBALL;
        } else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) && i + 1 < argc) {
            configFile = argv[++i];
        } else {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            displayUsage(argv[0]);
            return 1;
        }
    }
    
    try {
        // Create text-based simulation
        TextSimulation simulation("OOCatcher - CS 323 Project (Text Version)");
        
        // Try to load configuration
        try {
            simulation.loadConfig(configFile);
        } catch (const std::exception& e) {
            std::cerr << "Error loading configuration: " << e.what() << std::endl;
            std::cerr << "Using default settings." << std::endl;
        }
        
        // Configure simulation type
        simulation.configure(simulationType);
        
        // Initialize and run simulation
        simulation.initialize();
        simulation.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}