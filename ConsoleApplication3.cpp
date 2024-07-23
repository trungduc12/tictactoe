#include <iostream>
#include <vector>
#include <random>
#include <limits>

class Board {
public:
    char board[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };

    bool isFull() const {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }

    bool checkWin(char symbol) const {
        for (int i = 0; i < 3; i++) {
            if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) return true;
            if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) return true;
        }
        if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) return true;
        if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) return true;
        return false;
    }

    void printBoard() const {
        std::cout << "   0 1 2\n";
        for (int i = 0; i < 3; i++) {
            std::cout << i << " |";
            for (int j = 0; j < 3; j++) {
                std::cout << board[i][j] << "|";
            }
            std::cout << std::endl;
        }
    }
};

class Player {
public:
    char symbol;

    Player(char s) : symbol(s) {}

    virtual int makeMove(const Board& board) = 0;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(char s) : Player(s) {}

    int makeMove(const Board& board) override {
        int row, col;
        do {
            std::cout << "Nhap nuoc di (hang cot): ";
            std::cin >> row >> col;
        } while (row < 0 || row > 2 || col < 0 || col > 2 || board.board[row][col] != ' ');
        return row * 3 + col;
    }
};

class Robot : public Player {
public:
    enum Difficulty { EASY, MEDIUM, HARD };
    Difficulty difficulty;
    std::mt19937 rng; // Bộ sinh số ngẫu nhiên

    Robot(char s, Difficulty d) : Player(s), difficulty(d), rng(std::random_device{}()) {}

    int makeMove(const Board& board) override {
        if (difficulty == EASY) {
            std::uniform_int_distribution<int> dist(0, 8);
            int move;
            do {
                move = dist(rng);
            } while (board.board[move / 3][move % 3] != ' ');
            return move;
        }
        else if (difficulty == MEDIUM) {
            return mediumMove(board);
        }
        else { // HARD
            return minimax(board, 0, true, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
        }
    }

private:
    int mediumMove(const Board& board) {
        char opponentSymbol = (symbol == 'X') ? 'O' : 'X';

        // 1. Thắng nếu có thể
        for (int i = 0; i < 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = symbol;
                if (newBoard.checkWin(symbol)) {
                    return i;
                }
            }
        }

        // 2. Chặn đối thủ nếu đối thủ có thể thắng
        for (int i = 0; i < 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = opponentSymbol;
                if (newBoard.checkWin(opponentSymbol)) {
                    return i;
                }
            }
        }

        // 3. Tạo 2 trong 1 hàng
        for (int i = 0; i < 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = symbol;
                if (hasTwoInARow(newBoard, symbol)) {
                    return i;
                }
            }
        }

        // 4. Chặn đối thủ tạo 2 trong 1 hàng
        for (int i = 0; i < 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = opponentSymbol;
                if (hasTwoInARow(newBoard, opponentSymbol)) {
                    return i;
                }
            }
        }

        // 5. Nếu không có nước đi thắng hoặc chặn, chọn ngẫu nhiên
        std::uniform_int_distribution<int> dist(0, 8);
        int move;
        do {
            move = dist(rng);
        } while (board.board[move / 3][move % 3] != ' ');
        return move;
    }

    bool hasTwoInARow(const Board& board, char symbol) const {
        // Kiểm tra các hàng
        for (int i = 0; i < 3; i++) {
            int count = 0;
            for (int j = 0; j < 3; j++) {
                if (board.board[i][j] == symbol) {
                    count++;
                }
            }
            if (count == 2) return true;
        }

        // Kiểm tra các cột
        for (int j = 0; j < 3; j++) {
            int count = 0;
            for (int i = 0; i < 3; i++) {
                if (board.board[i][j] == symbol) {
                    count++;
                }
            }
            if (count == 2) return true;
        }

        // Kiểm tra đường chéo chính
        int count = 0;
        for (int i = 0; i < 3; i++) {
            if (board.board[i][i] == symbol) {
                count++;
            }
        }
        if (count == 2) return true;

        // Kiểm tra đường chéo phụ
        count = 0;
        for (int i = 0; i < 3; i++) {
            if (board.board[i][2 - i] == symbol) {
                count++;
            }
        }
        if (count == 2) return true;

        return false;
    }

    int minimax(Board board, int depth, bool maximizingPlayer, int alpha, int beta) {
        if (board.checkWin(symbol)) return 10 - depth;
        if (board.checkWin(symbol == 'X' ? 'O' : 'X')) return depth - 10;
        if (board.isFull()) return 0;

        if (maximizingPlayer) {
            int maxEval = -std::numeric_limits<int>::max();
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    board.board[i / 3][i % 3] = symbol;
                    maxEval = std::max(maxEval, minimax(board, depth + 1, false, alpha, beta));
                    board.board[i / 3][i % 3] = ' ';
                    alpha = std::max(alpha, maxEval);
                    if (beta <= alpha) {
                        break; // Beta cut-off
                    }
                }
            }
            return maxEval;
        }
        else {
            int minEval = std::numeric_limits<int>::max();
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    board.board[i / 3][i % 3] = symbol == 'X' ? 'O' : 'X';
                    minEval = std::min(minEval, minimax(board, depth + 1, true, alpha, beta));
                    board.board[i / 3][i % 3] = ' ';
                    beta = std::min(beta, minEval);
                    if (beta <= alpha) {
                        break; // Alpha cut-off
                    }
                }
            }
            return minEval;
        }
    }
};

class Game {
public:
    Board board;
    Player* player1;
    Player* player2;

    Game() {
        int choice;
        std::cout << "Chon che do choi:\n";
        std::cout << "1. Nguoi vs Nguoi\n";
        std::cout << "2. Nguoi vs May\n";
        std::cin >> choice;

        if (choice == 1) {
            player1 = new HumanPlayer('X');
            player2 = new HumanPlayer('O');
        }
        else {
            player1 = new HumanPlayer('X');
            std::cout << "Chon đo kho cho may:\n";
            std::cout << "1. De\n";
            std::cout << "2. Trung binh\n";
            std::cout << "3. Kho\n";
            std::cin >> choice;
            Robot::Difficulty difficulty = Robot::EASY;
            if (choice == 2) difficulty = Robot::MEDIUM;
            else if (choice == 3) difficulty = Robot::HARD;
            player2 = new Robot('O', difficulty);
        }
    }

    ~Game() {
        delete player1;
        delete player2;
    }

    void play() {
        int currentPlayer = 0;
        while (!board.isFull() && !board.checkWin('X') && !board.checkWin('O')) {
            board.printBoard();
            int move = (currentPlayer == 0 ? player1 : player2)->makeMove(board);
            board.board[move / 3][move % 3] = (currentPlayer == 0 ? 'X' : 'O');
            currentPlayer = 1 - currentPlayer;
        }

        board.printBoard();
        if (board.checkWin('X')) {
            std::cout << "Nguoi choi X thang!\n";
        }
        else if (board.checkWin('O')) {
            std::cout << "Nguoi choi O thang!\n";
        }
        else {
            std::cout << "Hoa!\n";
        }
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}
