#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <set>

using namespace std;

const int BOARD_WIDTH = 60;
const int BOARD_HEIGHT = 40;

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

    void setPixel(int x, int y, char c) {
        if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
            grid[y][x] = c;
        }
    }

    void clear() {
        grid.assign(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' '));
    }
};

class Shape {
public:
    virtual void draw(Board& board) = 0;
    virtual string info() const = 0;
    virtual string serialize() const = 0;
    virtual bool isInsideBoard() const = 0;
};

class Circle : public Shape {
    int x, y, radius;

public:
    Circle(int centerX, int centerY, int r) : x(centerX), y(centerY), radius(r) {}

    void draw(Board& board) override {
        for (int i = -radius; i <= radius; ++i) {
            for (int j = -radius; j <= radius; ++j) {
                int distanceSquared = i * i + j * j;
                if (distanceSquared <= radius * radius && distanceSquared >= (radius - 1) * (radius - 1)) {
                    if (x + i >= 0 && x + i < BOARD_WIDTH && y + j >= 0 && y + j < BOARD_HEIGHT) {
                        board.setPixel(x + i, y + j, '*');
                    }
                }
            }
        }
    }


    string info() const override {
        return "Circle (" + to_string(x) + ", " + to_string(y) + "), radius: " + to_string(radius);
    }

    string serialize() const override {
        return "circle " + to_string(x) + " " + to_string(y) + " " + to_string(radius);
    }

    bool isInsideBoard() const override {
        return (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT);
    }
};

class Rectangle : public Shape {
    int x, y, width, height;

public:
    Rectangle(int left, int top, int w, int h) : x(left), y(top), width(w), height(h) {}

    void draw(Board& board) override {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if ((i == 0 || i == height - 1 || j == 0 || j == width - 1) &&
                    (x + j >= 0 && x + j < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT)) {
                    board.setPixel(x + j, y + i, '*');
                }
            }
        }
    }

    string info() const override {
        return "Rectangle (" + to_string(x) + ", " + to_string(y) + "), width: " + to_string(width) + ", height: " + to_string(height);
    }

    string serialize() const override {
        return "rectangle " + to_string(x) + " " + to_string(y) + " " + to_string(width) + " " + to_string(height);
    }

    bool isInsideBoard() const override {
        return (x >= 0 && x <= BOARD_WIDTH && y >= 0 && y <= BOARD_HEIGHT);
    }
};

class Triangle : public Shape {
    int x, y, length;
    string type;

public:
    Triangle(int left, int top, int l, const string& triangleType) : x(left), y(top), length(l), type(triangleType) {}

    void draw(Board& board) override {
        if (type == "right") {
            for (int i = 0; i < length; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (i == length - 1 || j == 0 || j == i) {
                        if (x + j >= 0 && x + j < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT) {
                            board.setPixel(x + j, y + i, '*');
                        }
                    }
                }
            }
        }
        else if (type == "equal") {
            for (int i = 0; i < length; ++i) {
                if (x - i >= 0 && x - i < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT) {
                    board.setPixel(x - i, y + i, '*');
                }
                if (x + i >= 0 && x + i < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT) {
                    board.setPixel(x + i, y + i, '*');
                }
            }

            for (int j = x - (length - 1); j <= x + (length - 1); ++j) {
                if (j >= 0 && j < BOARD_WIDTH && y + (length - 1) >= 0 && y + (length - 1) < BOARD_HEIGHT) {
                    board.setPixel(j, y + (length - 1), '*');
                }
            }
        }
    }


    string info() const override {
        return "Triangle (" + to_string(x) + ", " + to_string(y) + "), length: " + to_string(length) + ", type: " + type;
    }

    string serialize() const override {
        return "triangle " + type + " " + to_string(x) + " " + to_string(y) + " " + to_string(length);
    }

    bool isInsideBoard() const override {
        return (type == "right" && x >= 0 && x <= BOARD_WIDTH && y >= 0 && y <= BOARD_HEIGHT) ||
            (type == "equal" && x >= 0 && x < BOARD_WIDTH && y >= 0 && y <= BOARD_HEIGHT);
    }
};

class Commands {
    map<int, Shape*> shapes;  // Map for storing shapes with unique IDs
    int currentId = 0;
    set<string> placedShapes;  // Set for storing serialized shape details to ensure uniqueness

public:
    ~Commands() {
        for (auto& pair : shapes) {
            delete pair.second;
        }
    }

    bool shapeExists(const Shape* shape) {
        return placedShapes.find(shape->serialize()) != placedShapes.end();
    }

    void addShape(const string& command, Board& board) {
        istringstream stream(command);
        string action, shapeType, triangleType;
        stream >> action >> shapeType;

        if (action != "add" || stream.fail()) {
            cout << "Invalid command format. Avalible commands are: add, shapes, draw, save, load, undo, clear, exit.\n";
            return;
        }

        if (shapeType == "circle") {
            int x, y, radius;
            if (!(stream >> x >> y >> radius)) {
                cout << "Invalid parameters for circle. Use: add circle <centerX> <centerY> <radius>\n";
                return;
            }
            Circle* circle = new Circle(x, y, radius);
            if (circle->isInsideBoard() && !shapeExists(circle)) {
                shapes[++currentId] = circle;
                placedShapes.insert(circle->serialize());
                circle->draw(board);
            }
            else {
                cout << "Invalid circle placement. Either out of bounds or shape already exists.\n";
                delete circle;
            }
        }
        else if (shapeType == "rectangle") {
            int x, y, width, height;
            if (!(stream >> x >> y >> width >> height)) {
                cout << "Invalid parameters for rectangle. Use: add rectangle <leftX> <topY> <width> <height>\n";
                return;
            }
            Rectangle* rectangle = new Rectangle(x, y, width, height);
            if (rectangle->isInsideBoard() && !shapeExists(rectangle)) {
                shapes[++currentId] = rectangle;
                placedShapes.insert(rectangle->serialize());
                rectangle->draw(board);
            }
            else {
                cout << "Invalid rectangle placement. Either out of bounds or shape already exists.\n";
                delete rectangle;
            }
        }
        else if (shapeType == "triangle") {
            stream >> triangleType;
            int x, y, length;
            if (!(stream >> x >> y >> length) || (triangleType != "right" && triangleType != "equal")) {
                cout << "Invalid parameters for triangle. Use: add triangle <type> <leftX> <topY> <length> (type: right/equal)\n";
                return;
            }
            Triangle* triangle = new Triangle(x, y, length, triangleType);
            if (triangle->isInsideBoard() && !shapeExists(triangle)) {
                shapes[++currentId] = triangle;
                placedShapes.insert(triangle->serialize());
                triangle->draw(board);
            }
            else {
                cout << "Invalid triangle placement. Either out of bounds or shape already exists.\n";
                delete triangle;
            }
        }
        else {
            cout << "Unknown shape type. Available shapes are circle, rectangle, triangle.\n";
        }
    }

    void drawAllShapes(Board& board) {
        board.clear();
        for (auto& pair : shapes) {
            pair.second->draw(board);
        }
    }

    void listShapes() {
        if (shapes.empty()) {
            cout << "No shapes added.\n";
            return;
        }
        cout << "List of shapes:\n";
        for (auto& pair : shapes) {
            cout << "ID: " << pair.first << " - " << pair.second->info() << "\n";
        }
    }

    void saveBoard(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Could not open file for saving.\n";
            return;
        }
        for (auto& pair : shapes) {
            file << pair.second->serialize() << "\n";
        }
        file.close();
        cout << "Board saved successfully to " << filename << ".\n";
    }

    bool loadBoard(const string& filename, Board& board) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Could not open file for loading.\n";
            return false;
        }

        vector<Shape*> tempShapes;
        string line;
        while (getline(file, line)) {
            istringstream stream(line);
            string shapeType;
            stream >> shapeType;

            if (shapeType == "circle") {
                int x, y, radius;
                stream >> x >> y >> radius;
                Circle* circle = new Circle(x, y, radius);
                if (circle->isInsideBoard()) {
                    tempShapes.push_back(circle);
                }
                else {
                    cout << "Invalid circle in file. Skipped.\n";
                    delete circle;
                }
            }
            else if (shapeType == "rectangle") {
                int x, y, width, height;
                stream >> x >> y >> width >> height;
                Rectangle* rectangle = new Rectangle(x, y, width, height);
                if (rectangle->isInsideBoard()) {
                    tempShapes.push_back(rectangle);
                }
                else {
                    cout << "Invalid rectangle in file. Skipped.\n";
                    delete rectangle;
                }
            }
            else if (shapeType == "triangle") {
                string triangleType;
                int x, y, length;
                stream >> triangleType >> x >> y >> length;
                Triangle* triangle = new Triangle(x, y, length, triangleType);
                if (triangle->isInsideBoard()) {
                    tempShapes.push_back(triangle);
                }
                else {
                    cout << "Invalid triangle in file. Skipped.\n";
                    delete triangle;
                }
            }
            else {
                cout << "Unknown shape type in file. Skipped line: " << line << "\n";
            }
        }
        file.close();

        clearShapes();
        board.clear();

        for (auto& shape : tempShapes) {
            shapes[++currentId] = shape;
            placedShapes.insert(shape->serialize());
        }

        drawAllShapes(board);
        cout << "Board loaded successfully from " << filename << ".\n";
        return true;
    }

    void clearShapes() {
        for (auto& pair : shapes) {
            delete pair.second;
        }
        shapes.clear();
        currentId = 0;
        placedShapes.clear();
    }

    void undo(Board& board) {
        if (!shapes.empty()) {
            shapes.erase(--shapes.end());
            drawAllShapes(board);
        }
        else {
            cout << "No shapes to undo.\n";
        }
    }

    void shapesAvalible() {
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
            c.shapesAvalible();
        }
        else if (command == "list") {
            c.listShapes();
        }
        else if (command == "undo") {
            c.undo(board);
            board.print();
        }
        else if (command == "save") {
            string filename;
            cout << "Enter filename: ";
            cin >> filename;
            cin.ignore();
            c.saveBoard(filename);
        }
        else if (command == "load") {
            string filename;
            cout << "Enter filename: ";
            cin >> filename;
            cin.ignore();
            c.loadBoard(filename, board);
            board.print();
        }
        else if (command == "clear") {
            board.clear();
            board.print();
        }
        else {
            c.addShape(command, board);
            board.print();
        }
        cout << "\n";
    }

    return 0;
}
