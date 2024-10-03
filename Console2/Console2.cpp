#include <iostream>
#include <vector>

using namespace std;

//size of the board
const int BOARD_WIDTH = 60;
const int BOARD_HEIGHT = 30;
// Struct to define the board
struct Board {
	vector<vector<char>> grid;
	Board() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}
	void print() {
		for (auto& row : grid) {
			for (char c : row) {
				cout  << "." << c;
			}
			cout << "\n";
		}
	}
	/*
	void drawTriangle(int x, int y, int height) {
		if (height <= 0) return; // Ensure the triangle height is positive and sensible
		for (int i = 0; i < height; ++i) {
			int leftMost = x - i; // Calculate the starting position
			int rightMost = x + i; // Calculate the ending position
			int posY = y + i; // Calculate the vertical position
			// Draw only the edges/border of the triangle
			if (posY < BOARD_HEIGHT) {
				if (leftMost >= 0 && leftMost < BOARD_WIDTH) {// Check bounds for left
					most position
						grid[posY][leftMost] = '*';
				};
				if (rightMost >= 0 && rightMost < BOARD_WIDTH && leftMost != rightMost)
					// Check bounds for right most position
					grid[posY][rightMost] = '*';
			}
		}
		// Draw the base of the triangle separately
		for (int j = 0; j < 2 * height - 1; ++j) {
			int baseX = x - height + 1 + j;
			int baseY = y + height - 1;
			if (baseX >= 0 && baseX < BOARD_WIDTH && baseY < BOARD_HEIGHT) // Check
				bounds for each position on the base
				grid[baseY][baseX] = '*';
		}
	}*/
};
int main() {
	Board board;
	//board.drawTriangle(10, 1, 5);
	board.print();
	return 0;
}