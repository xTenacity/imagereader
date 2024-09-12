#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>  // For hex formatting
#include <sstream>  // For string streams
#include <map>      // For storing color-position mappings

// Function to convert RGB to Hex string
std::string rgbToHex(int r, int g, int b) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << (r & 0xFF) 
       << std::setw(2) << (g & 0xFF) 
       << std::setw(2) << (b & 0xFF);
    return ss.str();
}

struct vector2 {
    int x, y;
};

int main() {
    cv::Mat image = cv::imread("amogus.png", cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Could not open or find the image" << std::endl;
        return -1;
    }

    std::ofstream outFile("output.txt"); // Opens and clears output file
    if (!outFile) {
        std::cerr << "Error opening output.txt" << std::endl;
        return -1;
    }

    // Map to store hex colors and their corresponding positions
    std::map<std::string, std::vector<vector2>> colorMap;

    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
            std::string hexColor = rgbToHex(pixel[2], pixel[1], pixel[0]); // Convert RGB to hex

            // Create position struct
            vector2 pos = {x, y};

            // Check if the color already exists in the map
            if (colorMap.find(hexColor) != colorMap.end()) {
                // If the color exists, append the position
                colorMap[hexColor].push_back(pos);
            } else {
                // If the color does not exist, create a new entry
                colorMap[hexColor] = {pos};
            }
        }
        // Direction control after each row
        if (y % 2 == 0) {
            outFile << "turnRight();\nmove();\nturnRight();" << std::endl;
        } else {
            outFile << "turnLeft();\nmove();\nturnLeft();" << std::endl;
        }
    }

    // Write out the data for each color and its positions
    for (const auto& pair : colorMap) {
        outFile << "Color: #" << pair.first << "\nPositions: ";
        for (const auto& pos : pair.second) {
            outFile << "(" << pos.x << "," << pos.y << ") ";
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Output written to output.txt" << std::endl;
    return 0;
}
/*
g++ -o output main.cpp `pkg-config --cflags --libs opencv4`
./output

*/