#include "config.h"
#include <iostream>
#include <fstream>

int main() {
    try {
        std::cout << "Starting the program" << std::endl;

        std::cout << "Loading YAML file..." << std::endl;
        auto tree = load_yaml_file("test.yaml");
        std::cout << "YAML file loaded successfully" << std::endl;

        std::cout << "Opening output file..." << std::endl;
        std::ofstream outputFile("output.txt");
        if (!outputFile.is_open()) {
            throw std::runtime_error("Error: Could not open output file.");
        }
        std::cout << "Output file opened successfully" << std::endl;

        std::cout << "Writing to output file..." << std::endl;
        std::cout << "Writing name to output file..." << std::endl;
        outputFile << "name: " << (*tree)["name"].get_value() << std::endl;
        std::cout << "Name written successfully" << std::endl;

        std::cout << "Writing age to output file..." << std::endl;
        outputFile << "age: " << (*tree)["age"].get_value() << std::endl;
        std::cout << "Age written successfully" << std::endl;

        std::cout << "Writing street to output file..." << std::endl;
        outputFile << "street: " << (*tree)["address"]["street"].get_value() << std::endl;
        std::cout << "Street written successfully" << std::endl;

        std::cout << "Writing zip to output file..." << std::endl;
        outputFile << "zip: " << (*tree)["address"]["zip"].get_value() << std::endl;
        std::cout << "Zip written successfully" << std::endl;

        std::cout << "Data written to output file" << std::endl;

        outputFile.close();
        std::cout << "Output file closed" << std::endl;

        std::cout << "Values written to output.txt in /home/alan/PythonValidationProject/handler/" << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}