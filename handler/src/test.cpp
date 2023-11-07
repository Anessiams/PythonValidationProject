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

        // Writing name
        std::cout << "Writing name to output file..." << std::endl;
        outputFile << "name: " << (*tree)["name"].get_value() << std::endl;
        std::cout << "Name written successfully" << std::endl;

        // Writing age
        std::cout << "Writing age to output file..." << std::endl;
        outputFile << "age: " << (*tree)["age"].get_value() << std::endl;
        std::cout << "Age written successfully" << std::endl;

        // Writing address details
        std::cout << "Writing street to output file..." << std::endl;
        outputFile << "street: " << (*tree)["address"]["street"].get_value() << std::endl;
        std::cout << "Street written successfully" << std::endl;

        std::cout << "Writing zip to output file..." << std::endl;
        outputFile << "zip: " << (*tree)["address"]["zip"].get_value() << std::endl;
        std::cout << "Zip written successfully" << std::endl;

        // Writing deepest friend
        std::cout << "Writing deepest friend to output file..." << std::endl;
        outputFile << "deepest friend: " << (*tree)["friends"]["friend1"]["friend2"]["friend3"]["friend4"]["friend5"].get_value() << std::endl;
        std::cout << "Deepest friend written successfully" << std::endl;

        // Writing contacts
        std::cout << "Writing home phone to output file..." << std::endl;
        outputFile << "home phone: " << (*tree)["contacts"]["phone"]["home"].get_value() << std::endl;
        std::cout << "Home phone written successfully" << std::endl;

        std::cout << "Writing work phone to output file..." << std::endl;
        outputFile << "work phone: " << (*tree)["contacts"]["phone"]["work"].get_value() << std::endl;
        std::cout << "Work phone written successfully" << std::endl;

        std::cout << "Writing twitter handle to output file..." << std::endl;
        outputFile << "twitter: " << (*tree)["contacts"]["social"]["twitter"].get_value() << std::endl;
        std::cout << "Twitter handle written successfully" << std::endl;

        std::cout << "Writing linkedin profile to output file..." << std::endl;
        outputFile << "linkedin: " << (*tree)["contacts"]["social"]["linkedin"].get_value() << std::endl;
        std::cout << "Linkedin profile written successfully" << std::endl;

        outputFile.close();
        std::cout << "Output file closed" << std::endl;

        std::cout << "Values written to output.txt in /home/alan/PythonValidationProject/handler/" << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
