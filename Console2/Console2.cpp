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
protected:
    string color;

public:
    virtual void draw(Board& board) = 0;
    virtual void move(int newX, int newY) = 0;
    virtual string info() const = 0;
    virtual string serialize() const = 0;
    virtual bool isInsideBoard() const = 0;
    virtual void setColor(const string& shapeColor) { color = shapeColor; }
    virtual string getColor() const { return color; }
    virtual void applyEdit(const vector<int>& newParams) = 0;
};

class Circle : public Shape {
    int x, y, radius;

public:
    Circle(int centerX, int centerY, int r) : x(centerX), y(centerY), radius(r) {}

    void draw(Board& board) override {
        //char drawChar = toupper(color[0]);
        for (int i = -radius; i <= radius; ++i) {
            for (int j = -radius; j <= radius; ++j) {
                int distanceSquared = i * i + j * j;
                if (distanceSquared <= radius * radius && distanceSquared >= (radius - 1) * (radius - 1)) {
                    if (x + i >= 0 && x + i < BOARD_WIDTH && y + j >= 0 && y + j < BOARD_HEIGHT) {
                        board.setPixel(x + i, y + j, '*' /*drawChar*/);
                    }
                }
            }
        }
    }

    void move(int newX, int newY) override {
        x = newX;
        y = newY;
    }

    void applyEdit(const vector<int>& newParams) override {
        if (newParams.size() == 1) {
            radius = newParams[0];
        }
        else {
            cout << "Invalid parameters for editing Circle. Expected 1 parameter (radius).\n";
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
        //char drawChar = toupper(color[0]);
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if ((i == 0 || i == height - 1 || j == 0 || j == width - 1) &&
                    (x + j >= 0 && x + j < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT)) {
                    board.setPixel(x + j, y + i, '*'/*drawChar*/);
                }
            }
        }
    }

    void move(int newX, int newY) override {
        x = newX;
        y = newY;
    }

    void applyEdit(const vector<int>& newParams) override {
        if (newParams.size() == 2) {
            width = newParams[0];
            height = newParams[1];
        }
        else {
            cout << "Invalid parameters for editing Rectangle. Expected 2 parameters (width, height).\n";
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
        //char drawChar = toupper(color[0]);
        if (type == "right") {
            for (int i = 0; i < length; ++i) {
                for (int j = 0; j <= i; ++j) {
                    if (i == length - 1 || j == 0 || j == i) {
                        if (x + j >= 0 && x + j < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT) {
                            board.setPixel(x + j, y + i, '*' /*drawChar*/);
                        }
                    }
                }
            }
        }
        else if (type == "equal") {
            for (int i = 0; i < length; ++i) {
                if (x - i >= 0 && x - i < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT) {
                    board.setPixel(x - i, y + i, '*' /*drawChar*/);
                }
                if (x + i >= 0 && x + i < BOARD_WIDTH && y + i >= 0 && y + i < BOARD_HEIGHT) {
                    board.setPixel(x + i, y + i, '*' /*drawChar*/);
                }
            }

            for (int j = x - (length - 1); j <= x + (length - 1); ++j) {
                if (j >= 0 && j < BOARD_WIDTH && y + (length - 1) >= 0 && y + (length - 1) < BOARD_HEIGHT) {
                    board.setPixel(j, y + (length - 1), '*' /*drawChar*/);
                }
            }
        }
    }

    void move(int newX, int newY) override {
        x = newX;
        y = newY;
    }

    
    void applyEdit(const vector<int>& newParams) override {
        if (newParams.size() == 1) {
            length = newParams[0];
        }
        else {
            cout << "Invalid parameters for editing Triangle. Expected 1 parameter (length).\n";
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
    int selectedId = -1;  // Track the last selected shape ID

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

    void saveBoard(const string& input) {
        istringstream stream(input);
        string command, filename;
        stream >> command >> filename;
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

    bool loadBoard(const string& input, Board& board) {
        istringstream stream(input);
        string command, filename;
        stream >> command >> filename;
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
        cout << "5. Circle fill: add circle fill <color> <centerX> <centerY> <redius>\n";
        cout << "6. Triangle fill: add triangle shape right/equal fill <color> <leftX> <topY> <width> <height>\n";
        cout << "7. Rectangle fill: add rectangle fill <color> <leftX> <topY> <width> <height>\n";
    }

    void select(const string& input) {
        istringstream stream(input);
        string command, arg1, arg2;
        stream >> command >> arg1 >> arg2;

        if (arg2.empty()) {
            int id;
            if (stringstream(arg1) >> id) {
                auto it = shapes.find(id);
                if (it != shapes.end()) {
                    cout << "Selected shape: " << it->second->info() << "\n";
                    selectedId = id;
                }
                else {
                    cout << "Shape with ID " << id << " not found.\n";
                }
            }
            else {
                cout << "Invalid shape ID.\n";
            }
        }
        else {
            int x, y;
            if (stringstream(arg1) >> x && stringstream(arg2) >> y) {
                if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
                    cout << "Coordinates (" << x << ", " << y << ") are out of the board's boundaries.\n";
                    return;
                }

                bool found = false;
                for (auto& pair : shapes) {
                    Shape* shape = pair.second;

                    Board tempBoard;
                    shape->draw(tempBoard);

                    if (tempBoard.grid[y][x] != ' ') {
                        cout << "Shape at (" << x << ", " << y << "): " << shape->info() << "\n";
                        selectedId = pair.first;
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    cout << "No shape found at (" << x << ", " << y << ").\n";
                }
            }
            else {
                cout << "Invalid coordinates. Use: select <ID> or select <x> <y>\n";
            }
        }
    }

    void removeShape(Board& board) {
        if (selectedId == -1) {
            cout << "No shape is currently selected.\n";
            return;
        }

        auto it = shapes.find(selectedId);
        if (it != shapes.end()) {
            placedShapes.erase(it->second->serialize());
            delete it->second;
            shapes.erase(it);
            selectedId = -1;  // Reset the last selected ID
            drawAllShapes(board);
            cout << "Shape removed from the board.\n";
        }
        else {
            cout << "Shape not found.\n";
        }
    }

    void moveShape(const string& input, Board& board) {
        if (selectedId == -1) {
            cout << "No shape is currently selected.\n";
            return;
        }

        istringstream stream(input);
        string command;
        int newX, newY;
        stream >> command >> newX >> newY;

        auto it = shapes.find(selectedId);
        if (it != shapes.end()) {
            it->second->move(newX, newY);
            drawAllShapes(board);
            cout << "Shape with ID " << selectedId << " moved to (" << newX << ", " << newY << ").\n";
        }
        else {
            cout << "Shape not found.\n";
        }
    }

    void editShape(const string& input, Board& board) {
        if (selectedId == -1) {
            cout << "No shape is currently selected.\n";
            return;
        }

        istringstream stream(input);
        string command;
        int param1, param2;
        vector<int> newParams;

        stream >> command;
        while (stream >> param1) {
            newParams.push_back(param1);
        }

        if (newParams.empty()) {
            cout << "Error: No parameters provided for editing.\n";
            return;
        }

        auto it = shapes.find(selectedId);
        if (it == shapes.end()) {
            cout << "Shape not found.\n";
            return;
        }

        Shape* shape = it->second;

        if (Circle* circle = dynamic_cast<Circle*>(shape)) {
            if (newParams.size() == 1) {
                vector<int> newRadius = { newParams[0] };
                Circle tempCircle = *circle;
                tempCircle.applyEdit(newRadius);
                if (tempCircle.isInsideBoard()) {
                    circle->applyEdit(newRadius);
                    drawAllShapes(board);
                    cout << "Circle radius changed to " << newRadius[0] << ".\n";
                }
                else {
                    cout << "Error: shape will go out of the board.\n";
                }
            }
            else {
                cout << "Error: invalid argument count for circle. Expected: edit <newRadius>\n";
            }
        }
        else if (Rectangle* rectangle = dynamic_cast<Rectangle*>(shape)) {
            if (newParams.size() == 2) {
                vector<int> newDimensions = { newParams[0], newParams[1] };
                Rectangle tempRectangle = *rectangle;
                tempRectangle.applyEdit(newDimensions);
                if (tempRectangle.isInsideBoard()) {
                    rectangle->applyEdit(newDimensions);
                    drawAllShapes(board);
                    cout << "Rectangle size changed to " << newDimensions[0] << "x" << newDimensions[1] << ".\n";
                }
                else {
                    cout << "Error: shape will go out of the board.\n";
                }
            }
            else {
                cout << "Error: invalid argument count for rectangle. Expected: edit <newWidth> <newHeight>\n";
            }
        }
        else if (Triangle* triangle = dynamic_cast<Triangle*>(shape)) {
            if (newParams.size() == 1) {
                vector<int> newLength = { newParams[0] };
                Triangle tempTriangle = *triangle;
                tempTriangle.applyEdit(newLength);
                if (tempTriangle.isInsideBoard()) {
                    triangle->applyEdit(newLength);
                    drawAllShapes(board);
                    cout << "Triangle length changed to " << newLength[0] << ".\n";
                }
                else {
                    cout << "Error: shape will go out of the board.\n";
                }
            }
            else {
                cout << "Error: invalid argument count for triangle. Expected: edit <newLength>\n";
            }
        }
        else {
            cout << "Error: Unsupported shape type for editing.\n";
        }
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
        else if (command.find("save") == 0) {
            c.saveBoard(command);
        }
        else if (command.find("load") == 0) {
            c.loadBoard(command, board);
            board.print();
        }
        else if (command == "clear") {
            board.clear();
            board.print();
        }
        else if (command.find("select") == 0) {
            c.select(command);
        }
        else if (command.find("move") == 0) {
            c.moveShape(command, board);
            board.print();
        }
        else if (command == "remove") {
            c.removeShape(board);
        }
        else if (command.find("edit") == 0) {
            c.editShape(command, board);
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
