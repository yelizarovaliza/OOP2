#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

const int BOARD_WIDTH = 60;
const int BOARD_HEIGHT = 40;

// Struct to define the board
struct Board {
    vector<vector<char>> grid;
    Board() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

    void print() {
        for (auto& row : grid) {
            for (char c : row) {
                cout << c;
            }
            cout << "\n";
        }
    }

    // Method to add a character at a specific position if within bounds
    void setPixel(int x, int y, char c) {
        if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
            grid[y][x] = c;
        }
    }

    // Method to clear the board
    void clear() {
        grid.assign(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' '));
    }
};

// Abstract base class Shape
class Shape {
public:
    virtual void draw(Board& board) = 0;
};

// Circle class derived from Shape
class Circle : public Shape {
    int x, y, radius;

public:
    Circle(int centerX, int centerY, int r) : x(centerX), y(centerY), radius(r) {}

    void draw(Board& board) override {
        for (int i = -radius; i <= radius; ++i) {
            for (int j = -radius; j <= radius; ++j) {
                int distanceSquared = i * i + j * j;
                if (distanceSquared <= radius * radius && distanceSquared >= (radius - 1) * (radius - 1)) {
                    board.setPixel(x + i, y + j, '*');
                }
            }
        }
    }
};

// Rectangle class derived from Shape
class Rectangle : public Shape {
    int x, y, width, height;

public:
    Rectangle(int left, int top, int w, int h) : x(left), y(top), width(w), height(h) {}

    void draw(Board& board) override {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    board.setPixel(x + j, y + i, '*');
                }
            }
        }
    }
};

// Triangle class derived from Shape
class Triangle : public Shape {
    int x, y, length;

public:
    Triangle(int left, int top, int l) : x(left), y(top), length(l) {}

    // Draw a right triangle
    void drawRight(Board& board) {
        for (int i = 0; i < length; ++i) {
            for (int j = 0; j <= i; ++j) {
                if (i == length - 1 || j == 0 || j == i) {
                    board.setPixel(x + j, y + i, '*');
                }
            }
        }
    }

    // Draw only the frame of an equilateral triangle
    void drawEquilateralFrame(Board& board) {
        // Draw left edge of the triangle
        for (int i = 0; i < length; ++i) {
            board.setPixel(x - i, y + i, '*');
        }

        // Draw right edge of the triangle
        for (int i = 0; i < length; ++i) {
            board.setPixel(x + i, y + i, '*');
        }

        // Draw the base of the triangle
        for (int j = x - (length - 1); j <= x + (length - 1); ++j) {
            board.setPixel(j, y + (length - 1), '*');
        }
    }

    void draw(Board& board, const string& type) {
        if (type == "right") {
            drawRight(board);
        }
        else if (type == "equal") {
            drawEquilateralFrame(board);
        }
    }

    void draw(Board& board) override {
        drawEquilateralFrame(board);
    }
};

// Command class to handle shapes
class Commands {
    vector<Shape*> shapes;  // Store shapes added to the board

public:
    ~Commands() {
        for (Shape* shape : shapes) {
            delete shape;  // Free dynamically allocated memory
        }
    }

    void addShape(const string& command, Board& board) {
        istringstream stream(command);
        string action, shapeType, triangleType;
        stream >> action >> shapeType;

        if (action == "add") {
            if (shapeType == "circle") {
                int x, y, radius;
                stream >> x >> y >> radius;
                Circle* circle = new Circle(x, y, radius);
                shapes.push_back(circle);
                circle->draw(board);
            }
            else if (shapeType == "rectangle") {
                int x, y, width, height;
                stream >> x >> y >> width >> height;
                Rectangle* rectangle = new Rectangle(x, y, width, height);
                shapes.push_back(rectangle);
                rectangle->draw(board);
            }
            else if (shapeType == "triangle") {
                stream >> triangleType;
                int x, y, length;
                stream >> x >> y >> length;
                Triangle* triangle = new Triangle(x, y, length);
                shapes.push_back(triangle);
                triangle->draw(board, triangleType);
            }
        }
    }

    // Draws all shapes on the board
    void drawAllShapes(Board& board) {
        board.clear();
        for (Shape* shape : shapes) {
            shape->draw(board);
        }
    }

    // Lists the available shape commands and their formats
    void listShapes() {
        cout << "Available shapes and parameters:\n";
        cout << "1. Circle: add circle <centerX> <centerY> <radius>\n";
        cout << "2. Rectangle: add rectangle <leftX> <topY> <width> <height>\n";
        cout << "3. Triangle (Right): add triangle right <leftX> <topY> <length>\n";
        cout << "4. Triangle (Equilateral): add triangle equal <centerX> <topY> <length>\n";
    }
};

int main() {
    Board board;
    Commands c;
    string command;

    while (true) {
        cout << "Enter a command: ";
        getline(cin, command);
        if (command == "exit") {
            break;
        }
        else if (command == "draw") {
            c.drawAllShapes(board);
            board.print();
        }
        else if (command == "shapes") {
            c.listShapes();
        }
        else {
            c.addShape(command, board);
            board.print();
        }
        cout << "\n";
    }

    return 0;
}
