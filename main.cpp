#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>  // hex formatting
#include <sstream>
#include <map>

// CREATE X,Y PAIRS
struct vector2 { 
    int x, y;
};

// GLOBALS
std::string classname;
std::string imagefile;
std::string output;
std::ofstream outFile;
std::map<std::string, std::vector<vector2>> colorMap;

// RGB TO HEX STRING
std::string rgbToHex(int r, int g, int b) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << (r & 0xFF) 
       << std::setw(2) << (g & 0xFF) 
       << std::setw(2) << (b & 0xFF);
    return ss.str();
}
// CREATE THE COLOR MAPPING
std::map<std::string, std::vector<vector2>> createColorMap() {
    cv::Mat image = cv::imread(imagefile, cv::IMREAD_COLOR); // open image
    if (image.empty()) { // if there's no image, then return an error
        std::cerr << "Could not open or find the image" << std::endl;
        exit(1); // Exit with error code
    }
    if (!outFile) {
        std::cerr << "Error opening output.txt" << std::endl;
        exit(1); // Exit with error code
    }

    // Map to store hex colors and their corresponding positions
    std::map<std::string, std::vector<vector2>> colorMap;

    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x); // get the pixel
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
    return colorMap;
}
// CREATE THE MAIN METHOD
void createMain() {
    outFile << "/*\n//Put this in the main method\n";
    outFile << "import org.code.neighborhood.*;\n";
    outFile << "public class NeighborhoodRunner {\n";
    outFile << "    public static void main(String[] args) {\n";
    outFile << "        ImagePainter painter = new ImagePainter();\n";
    outFile << "        painter.drawImage(\"";
    int i = 0;
    for (const auto& pair : colorMap) {
        outFile << "#" << pair.first;
        
        // Commas on everything but the end
        if (i == colorMap.size() - 1) {
            outFile << "\"";
        } else {
            outFile << "\", \"";
        }
        i++;
    }
    outFile << ");\n";
    outFile << "    }\n}\n*/\n\n";
}
//CREATE THE INITIAL DRAW IMAGE FUNCTION
void createClass() {
    outFile << "// Put this in the " << classname << " file\n";
    outFile << "import org.code.neighborhood.*;\n";
    outFile << "public class " << classname << " extends PainterPlus {// initialize the ImagePainter class\n";
}
//CREATE THE INITIAL DRAW IMAGE FUNCTION
void createInitFunction() {
    outFile << "    public void drawImage(";
    for (int i = 0; i < colorMap.size(); i++) {
        if (i == colorMap.size() - 1) {
            outFile << "String color" << i;
        } else {
            outFile << "String color" << i << ", ";
        }
    }
    outFile << ") {\n";
    // Write the contents of the drawImage function
    for (int i = 0; i < colorMap.size(); i++) {
        outFile << "        drawColor" << i << "(color" << i << ");\n";
    }
    outFile << "    }\n\n";
}
// GRAB HEXCODES OF ALL COLORS AND PUT IN A VECTOR
std::vector<std::string> grabHex() {
    std::vector<std::string> hexcodes;
    for (const auto& pair : colorMap) {
        hexcodes.push_back(pair.first);
    }
    return hexcodes;
}
// CREATE ONE DRAW FUNCTION AT POS "i"


void createDraw(const std::vector<std::string>& hexcodes, std::vector<vector2>& beginlist, int i) {
    vector2 begin; // top left
    vector2 end;   // bottom right

    // Positions stores all of the positions of pixels with the current hex code
    std::vector<vector2>& positions = colorMap.at(hexcodes[i]);

    // tempx and tempy store all of the x and y values of all positions stored earlier.
    std::vector<int> tempx, tempy;
    for (const auto& pos : positions) {
        tempx.push_back(pos.x);
        tempy.push_back(pos.y);
    }

    // Find the top left and bottom right corners (min/max coordinates)
    begin.x = *std::min_element(tempx.begin(), tempx.end());
    begin.y = *std::min_element(tempy.begin(), tempy.end());
    end.x = *std::max_element(tempx.begin(), tempx.end());
    end.y = *std::max_element(tempy.begin(), tempy.end());

    // Push back where the draw command starts
    beginlist.push_back(begin);

    // Start typing out the actual function
    outFile << "    public void drawColor" << i << "(String color) {\n";
    if (begin.x > 0 || begin.y > 0) {
        outFile << "        moveToStart(\"" << i << "\");\n";
    } else {
        outFile << "        // no moveToStart function because the initial position is at 0, 0\n";
    }

    // Height and width of the rectangle are set here
    int height = end.y - begin.y + 1;
    int width = end.x - begin.x + 1;

    std::vector<std::vector<int>> grid(height, std::vector<int>(width, 0));

    // Fill the grid with 1s for the positions of the current color
    for (const auto& pos : positions) {
        int grid_x = pos.x - begin.x;
        int grid_y = pos.y - begin.y;
        grid[grid_y][grid_x] = 1;
    }
    //DEBUG: Print the grid
    std::cout << "Grid for color #" << hexcodes[i] << ":" << std::endl;
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            std::cout << cell;
        }
        std::cout << std::endl;  // New line after each row
    }

    std::cout << "(" << begin.x << ", " << begin.y << ")" << std::endl;
    std::cout << grid[0].size() << std::endl;

    // For each row in the height of the rectangle
    for (int j = 0; j < height; j++) {
        std::vector<int> row(width, 0); // Initialize row with all 0

        // Set positions that are colored to 1
        for (size_t u = 0; u < tempy.size(); u++) {
            if (tempy[u] == begin.y + j) {
                int index;
                if (j % 2 == 0) {
                    // Left to Right
                    index = tempx[u] - begin.x;
                } else {
                    // Right to Left
                    index = end.x - tempx[u];
                }
                if (index >= 0 && index < width) {
                    row[index] = 1; // Set the position to 1
                }
            }
        }






        
        bool painting = false;
        int counter = 0;

        for (int u = 0; u < row.size()-1; u++) {
            if (row[u] == 1) { // If we're on a painted tile
                if (painting) {
                    counter++; // Keep painting
                } else {
                    painting = true;
                    if (counter > 0) {
                        outFile << "            move(" << counter << ");\n"; // Move before painting
                    }
                    counter = 1; // Start counting painted tiles
                }
            } else { // If we're on a non-painted tile
                if (painting) {
                    painting = false;
                    outFile << "            paintLine(color, " << counter << ");\n"; // Paint the line
                    counter = 1; // Start counting spaces to move
                } else {
                    if (u < row.size()) {
                        counter++;
                    }
                }
            }
        }

        // Handle the final segment in the row
        if (painting) {
            outFile << "            paintLine(color, " << counter << ");\n";
        } else if (counter > 0) {
            outFile << "            move(" << counter << ");\n";
        }

        // Move to the next row (y-axis), but skip it for the last row
        if (j < height - 1) {
            if (row[row.size() - 1]) {
                outFile << "            nextLayer(true, color);                //row " << j+1 << "\n";
            } else {
                outFile << "            nextLayer(false, color);                //row " << j+1 << "\n";
            }
        } else if (j == height - 1) {
            if (row[row.size() - 1]) {
                outFile << "            turnLeft();\n            turnLeft();\n            paintLine(color, 1);\n";
            }
        }
    }

    outFile << "        home();\n    }\n";
}
// CREATE THE DRAW FUNCTIONS
std::vector<vector2> createDrawFunctions() {
    std::vector<std::string> hexcodes = grabHex();
    std::vector<vector2> beginlist;
    for (int i = 0; i < colorMap.size(); i++) {
        createDraw(hexcodes, beginlist, i);
    }
    return beginlist;
}
// CREATE MOVETOSTART FUNCTION
void createMoveToStart(const std::vector<vector2>& beginlist) {
    outFile << "    public void moveToStart(String part) {\n";
    outFile << "        int movex = 0, movey = 0;\n";
    bool first = true;
    for (int i = 0; i < colorMap.size(); i++) {
        if (beginlist[i].x > 0 || beginlist[i].y > 0) {
            if (first) {
                outFile << "        if (part.equals(\"" << i << "\")) {\n";
                first = false;
            } else {
                outFile << " else if (part.equals(\"" << i << "\")) {\n";
            }
            if (beginlist[i].x > 0) {
                outFile << "            movex = " << (beginlist[i].x) << ";\n";
            }
            if (beginlist[i].y > 0) {
                outFile << "            movey = " << beginlist[i].y << ";\n";
            }
            outFile << "        }";
        }
    }
    outFile << "\n        move(movex);\n";
    outFile << "        turnRight();\n";
    outFile << "        move(movey);\n";
    outFile << "        turnLeft();\n";
    outFile << "    }\n}\n\n\n\n";
}
// CREATE THE IMAGEPAINTER FILE
void createImagePainter() {
    createClass();
    createInitFunction();
    std::vector<vector2> beginlist = createDrawFunctions();
    createMoveToStart(beginlist);
}

int main() {
    classname = "ImagePainter";
    imagefile = "images/brimwitchsimple.png";
    output = "output.txt";
    outFile.open(output);
    colorMap = createColorMap();

    createMain();
    createImagePainter();

    outFile.close();
    return 0;
}