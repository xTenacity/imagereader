#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>  // For hex formatting
#include <sstream>  // For string streams
#include <map>      // For storing color-position mappings

// Function to convert RGB to Hex string, taken from chatgpt lol
std::string rgbToHex(int r, int g, int b) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << (r & 0xFF) 
       << std::setw(2) << (g & 0xFF) 
       << std::setw(2) << (b & 0xFF);
    return ss.str();
}

struct vector2 { // For x,y values
    int x, y;
};

int main() {

    std::string classname = "ImagePainter";
    std::string imagefile = "images/amogus.png";







    cv::Mat image = cv::imread(imagefile, cv::IMREAD_COLOR); //open image
    if (image.empty()) { //if there's no image, then return an error
        std::cerr << "Could not open or find the image" << std::endl;
        return -1;
    }

    std::ofstream outFile("output.txt"); // Opens and clears output file
    if (!outFile) { //if no output file return error
        std::cerr << "Error opening output.txt" << std::endl;
        return -1;
    }

    // Map to store hex colors and their corresponding positions
    std::map<std::string, std::vector<vector2>> colorMap;

    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x); //get the pixel
            std::string hexColor = rgbToHex(pixel[2], pixel[1], pixel[0]); // Convert RGB to hex

            // Create position struct
            vector2 pos = {x, y};

            // Check if the color already exists in the map
            if (colorMap.find(hexColor) != colorMap.end()) { 
                // If the color exists, append the position
                colorMap[hexColor].push_back(pos);
            } else {
                // If no color, add it
                colorMap[hexColor] = {pos};
            }
        }
    }








    //PRINT TO OUTPUT.TXT

    //for the main method
    outFile << "//Put this in the main method\n//painter.drawImage(\"";
    int i = 0;
    for (const auto& pair : colorMap) {
        outFile << "#" << pair.first;
        
        //commas on everything but the end
        if (i == colorMap.size() - 1) {
            outFile << "\"";
        } else {
            outFile << "\", \"";
        }
        i++;
    }
    outFile << ");\n\n";

    //beginning of the ImagePainter class file
    outFile << "//put this in the " << classname << " file\nimport org.code.neighborhood.*;\npublic class " << classname << " extends PainterPlus {//initialize the ImagePainter class\n";

    //beginning of the first function
    outFile << "    public void drawImage(";
    for (int i = 0; i < colorMap.size(); i++) {
        if (i == colorMap.size() - 1) {
            outFile << "String color" << i;
        } else {
            outFile << "String color" << i << ", ";
        }
    }
    outFile << ") {\n";
    
    //Write the contents of the first function
    for (int i = 0; i < colorMap.size(); i++) {
        outFile << "        drawColor" << i << "(color" << i << ");\n";
    }
    outFile << "    }\n\n";







    //for each color
    std::vector<std::string> hexcodes; //grab hexcodes
    for (const auto& pair : colorMap) {
        hexcodes.push_back(pair.first);
    }
    std::vector<vector2> beginlist = {}; //for the initial position setter latter
    for (int i = 0; i < colorMap.size(); i++) {

        //put the area into a rectangle
        vector2 begin; //top left
        vector2 end; //bottom right
    
        std::vector<vector2>& positions = colorMap[hexcodes[i]];
        std::vector<int> tempx = {};
        std::vector<int> tempy = {};
        for (int i = 0; i < positions.size(); i++) {
            tempx.push_back(positions[i].x);
            tempy.push_back(positions[i].y);
        }
        begin.x = *min_element(tempx.begin(), tempx.end());
        begin.y = *min_element(tempy.begin(), tempy.end());
        beginlist.push_back(begin);
        end.x = *max_element(tempx.begin(), tempx.end());
        end.y = *max_element(tempy.begin(), tempy.end());

        std::cout << "(" << begin.x << ", " << begin.y << "), (" << end.x << ", " << end.y << ")" << std::endl;
        std::cout << "height: " << (end.y - begin.y + 1) << ", width: " << (end.x - begin.x + 1) << std::endl;
        //now we have the two positions we will travel through (top left and bottom right)
        

        outFile << "    public void drawColor" << i << "(String color) {\n";
        if (beginlist[i].x > 0 && beginlist[i].y > 0) {
            outFile << "        moveToStart(\"" << i << "\");\n";
        } else {
            outFile << "        //no moveToStart function because the initial position is at 0, 0\n";
        }

        int height = end.y - begin.y + 1;
        int width = end.x - begin.x + 1;
        //std::cout << "height: " << height << ", width: " << width << std::endl;










        for (int j = 0; j < height; j++) {
            std::vector<int> row(width, 0); // Initialize the row with all 0s

            // Set the appropriate positions to 1
        
            for (int u = 0; u < tempy.size(); u++) {
                if (tempy[u] == begin.y + j) {
                    if (j % 2 == 0) {
                        // Left to Right
                        int index = tempx[u] - begin.x;
                        if (index >= 0 && index < width) {
                            row[index] = 1; // Set the position to 1
                        }
                    } else {
                        // Right to Left
                        int index = end.x - tempx[u] + begin.x;
                        if (index >= 0 && index < width) {
                            row[index] = 1; // Set the position to 1
                        }
                    }
                }
            }
            for (int u = 0; u < row.size(); u++) {
                std::cout << row[i];
            }
            std::cout << "\n";

            // Process the row to generate commands
            int count = 0;
            bool moving = row[0] == 1; // Set moving to false if the first pixel is 1, otherwise true

            for (int u = 0; u < row.size(); u++) {
                if (row[u] == 1) { // If there's a pixel to be painted
                    if (moving) {
                        // If moving, we should have been moving, so print the move command
                        if (count > 0) {
                            if (j == 0) {
                                outFile << "        move(" << count+1 << ");\n";
                            } else {
                                outFile << "        move(" << count+1 << ");\n";
                            }
                            count = 0;
                        }
                        moving = false; // Now we are painting
                    }
                    count++;
                } else { // If the current pixel is 0
                    if (!moving) {
                        // If not moving, we should be painting, so print the paint command
                        if (count > 0) {
                            outFile << "        paintLine(color, " << count << ");\n";
                            count = 0;
                        }
                        moving = true; // Now we are moving
                    }
                    count++;
                }
            }

            // Handle the case where the last sequence ends at the end of the row
            if (!moving && count > 0) {
                outFile << "        paintLine(color, " << count << ");\n";
            } else if (moving && count > 0) {
                outFile << "        move(" << count << ");\n";
            }



            
            if (j < height - 1) {
                outFile << "        nextLayer();\n"; // Move to the next row (y-axis)
            }
        }
        outFile << "    home();\n    }\n";
    }





















    //moveToStart command
    outFile << "    public void moveToStart(String part) {\n";
    bool first = true;
    for (int i = 0; i < colorMap.size(); i++) {
        if (beginlist[i].x > 0 && beginlist[i].y > 0) {
            if (first) {
                outFile << "        if (part.equals(\"" << i << "\")) {\n";
                first = false;
            } else {
                outFile << " else if (part.equals(\"" << i << "\")) {\n";
            }
            if (beginlist[i].x > 0) {
                outFile << "            move(" << beginlist[i].x-1 << ");\n";
            }
            outFile << "            turnRight();\n";
            if (beginlist[i].y > 0) {
                outFile << "            move(" << beginlist[i].y << ");\n";
            }
            outFile << "            turnLeft();\n";
            outFile << "        }\n";
        }
    }
    outFile << "\n    }\n}\n\n\n\n";
    
    outFile.close();
    return 0;
}
/*
g++ -o output main.cpp `pkg-config --cflags --libs opencv4`
./output

*/