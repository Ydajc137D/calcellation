#include "Model.h"
#include <algorithm>
#include <fstream>
#include <random>

std::unordered_map<int, std::string> taskContent = {
    {1, "Collect 100 points"},
    {2, "Clear 20 apples"},
    {3, "Use 3 special elements"},
    {4, "Complete within 50 moves"},
    {5, "Achieve a combo of 5"}
};

Model::Model() {
    loadLeaderboard();
}

void Model::initializeGame(Difficulty difficulty) {
    currentDifficulty = difficulty;
    board = std::vector<std::vector<ElementType>>(10, std::vector<ElementType>(10));
    score = 0;
    stepsLeft = (difficulty == EASY) ? 70 : 40; 
    //timeLeft = (difficulty == EASY) ? -1 : 300; // ��ʱ�����ƻ��ʼʱ�䣨�룩
    assignNewTask();
    isNewRecord = false;
    refreshBoard();//��Ϊ�̶�����
    clearElements();
    score = 0;
    ToolNumber = {
    {HAMMER, 3},
    {BOMB_TRANSFORM, 1},
    {REFRESH, 5},
    {ROW_CLEAR_TRANSFORM, 1},
    {COLUMN_CLEAR_TRANSFORM, 1}
    };
}

bool Model::saveGame() {
    // ���浱ǰ��Ϸ״̬���ļ�
    std::ofstream outFile("game_save.dat", std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(&board[0]), board.size() * sizeof(board[0]));
    outFile.write(reinterpret_cast<const char*>(&score), sizeof(score));
    outFile.write(reinterpret_cast<const char*>(&stepsLeft), sizeof(stepsLeft));
    //outFile.write(reinterpret_cast<const char*>(&timeLeft), sizeof(timeLeft));
    outFile.write(reinterpret_cast<const char*>(&currentTask.TaskType), sizeof(currentTask.TaskType));
    outFile.write(currentTask.Taskcontent.c_str(), currentTask.Taskcontent.size());
    outFile.write(reinterpret_cast<const char*>(&currentDifficulty), sizeof(currentDifficulty));
    outFile.write(reinterpret_cast<const char*>(&isNewRecord), sizeof(isNewRecord));
    outFile.close();
    if (!outFile) return false;
    return true;
}

bool Model::loadGame() {
    // ���ļ�������Ϸ״̬
    std::ifstream inFile("game_save.dat", std::ios::binary);
    if (inFile) {
        inFile.read(reinterpret_cast<char*>(&board[0]), board.size() * sizeof(board[0]));
        inFile.read(reinterpret_cast<char*>(&score), sizeof(score));
        inFile.read(reinterpret_cast<char*>(&stepsLeft), sizeof(stepsLeft));
        //inFile.read(reinterpret_cast<char*>(&timeLeft), sizeof(timeLeft));
        inFile.read(reinterpret_cast<char*>(&currentTask.TaskType), sizeof(currentTask.TaskType));
        char taskBuffer[256];
        inFile.read(taskBuffer, 256);
        currentTask.Taskcontent = std::string(taskBuffer);
        inFile.read(reinterpret_cast<char*>(&currentDifficulty), sizeof(currentDifficulty));
        inFile.read(reinterpret_cast<char*>(&isNewRecord), sizeof(isNewRecord));
        inFile.close();
        return true;
    }
    return false;
}

GameState Model::processSwap(int x1, int y1, int x2, int y2) {//����ֵΪ�������������󣩼�ʱ��״̬
    // ������������״̬
    GameState state;
    if (isValidSwap(x1, y1, x2, y2)) {
        state = getGameState();
        handleEmptyAndGravity();
        //clearElements();
        stepsLeft--;
    }
    else {
        state = getGameState();
        std::swap(board[x1][y1], board[x2][y2]);
    }
    return state;
}

void Model::useTool(ToolType tool, int x, int y) {
    // ʹ�õ��߲�����״̬
    switch (tool) {
    case HAMMER:
        board[x][y] = EMPTY;
        handleEmptyAndGravity();
        //clearElements();
        break;
    case BOMB_TRANSFORM:
        board[x][y] = BOMB;
        break;
    case REFRESH:
        refreshBoard();
        clearElements();
        break;
    case ROW_CLEAR_TRANSFORM:
        board[x][y] = ROW_CLEAR;
        break;
    case COLUMN_CLEAR_TRANSFORM:
        board[x][y] = COLUMN_CLEAR;
        break;
    }
}

void Model::assignNewTask() {
    // ����������
    int taskType = (rand() % 5) + 1;
    currentTask.TaskType = taskType;
    currentTask.Taskcontent = taskContent[taskType];
}

bool Model::checkTaskCompletion() {//������
    // �������������
    // ���������ǻ��100��
    if (currentTask.TaskType == 1 && score >= 100) {
        return true;
    }
    return false;
}

void Model::loadLeaderboard() {
    // ���ļ��������а�����
    std::ifstream inFile("leaderboard.dat", std::ios::binary);
    if (inFile) {
        inFile.read(reinterpret_cast<char*>(&highScores[EASY]), sizeof(highScores[EASY]));
        inFile.read(reinterpret_cast<char*>(&highScores[HARD]), sizeof(highScores[HARD]));
        inFile.close();
    }
}

void Model::saveLeaderboard() {
    // �������а����ݵ��ļ�
    std::ofstream outFile("leaderboard.dat", std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(&highScores[EASY]), sizeof(highScores[EASY]));
    outFile.write(reinterpret_cast<const char*>(&highScores[HARD]), sizeof(highScores[HARD]));
    outFile.close();
}

bool Model::BreakRecord() {
    // ��鵱ǰ�÷��Ƿ�Ϊ�¸�
    if (score > highScores[currentDifficulty]) {
        highScores[currentDifficulty] = score;
        isNewRecord = true;
        saveLeaderboard();
        return true;
    }
    isNewRecord = false;
    return false;
}

GameState Model::getGameState() const {
    GameState state = {
        board,
        score,
        stepsLeft,
        //timeLeft,
        currentTask,
        isNewRecord,
        ToolNumber
    };
    return state;
}

bool Model::checkRainbowCircle(int x, int y, ElementType type) {
    bool result = false;
    int rainbowCount = 1;
    // ������
    for (int i = y - 1; i >= 0 && board[x][i] == type; --i) {
        rainbowCount++;
    }
    for (int i = y + 1; i < 10 && board[x][i] == type; ++i) {
        rainbowCount++;
    }
    if (rainbowCount >= 5) {
        for (int i = y - 1; i >= 0 && board[x][i] == type; --i) {
            board[x][i] = EMPTY;
        }
        for (int i = y + 1; i < 10 && board[x][i] == type; ++i) {
            board[x][i] = EMPTY;
        }
        board[x][y] = RAINBOW_CIRCLE;
        //�������������
        result = true;
    }
    if (result) return result;

    // ������
    int verticalRainbowCount = 1;
    for (int i = x - 1; i >= 0 && board[i][y] == type; --i) {
        verticalRainbowCount++;
    }
    for (int i = x + 1; i < 10 && board[i][y] == type; ++i) {
        verticalRainbowCount++;
    }
    if (verticalRainbowCount >= 5) {
        for (int i = x - 1; i >= 0 && board[i][y] == type; --i) {
            board[i][y] = EMPTY;
        }
        for (int i = x + 1; i < 10 && board[i][y] == type; ++i) {
            board[i][y] = EMPTY;
        }
        board[x][y] = RAINBOW_CIRCLE;
        //��˺���������
        result = true;
    }
    return result;
}

bool Model::checkBombL(int x, int y, ElementType type) {
    bool result = false;
    if (y >= 2 && board[x][y - 1] == type && board[x][y - 2] == type) {
        // ����
        if (x >= 2 && board[x - 1][y] == type && board[x - 2][y] == type) {
            board[x][y] = BOMB;
            board[x][y - 1] = EMPTY;
            board[x][y - 2] = EMPTY;
            board[x - 1][y] = EMPTY;
            board[x - 2][y] = EMPTY;
            result = true;
        }
        if (result) return result;
        // ����
        if (x <= 7 && board[x + 1][y] == type && board[x + 2][y] == type) {
            board[x][y] = BOMB;
            board[x][y - 1] = EMPTY;
            board[x][y - 2] = EMPTY;
            board[x + 1][y] = EMPTY;
            board[x + 2][y] = EMPTY;
            result = true;
        }
        if (result) return result;
    }
    if (y <= 7 && board[x][y + 1] == type && board[x][y + 2] == type) {
        // ����
        if (x >= 2 && board[x - 1][y] == type && board[x - 2][y] == type) {
            board[x][y] = BOMB;
            board[x][y - 1] = EMPTY;
            board[x][y - 2] = EMPTY;
            board[x - 1][y] = EMPTY;
            board[x - 2][y] = EMPTY;
            result = true;
        }
        if (result) return result;
        // ����
        if (x <= 7 && board[x + 1][y] == type && board[x + 2][y] == type) {
            board[x][y] = BOMB;
            board[x][y - 1] = EMPTY;
            board[x][y - 2] = EMPTY;
            board[x + 1][y] = EMPTY;
            board[x + 2][y] = EMPTY;
            result = true;
        }
    }
    return result;
}

bool Model::checkBombT(int x, int y, ElementType type) {
    bool result = false;
    if (y >= 1 && y <= 8 && board[x][y - 1] == type && board[x][y + 1] == type) {
        if (x >= 2 && board[x - 1][y] == type && board[x - 2][y] == type) {
            board[x][y] = BOMB;
            board[x][y - 1] = EMPTY;
            board[x][y + 1] = EMPTY;
            board[x - 1][y] = EMPTY;
            board[x - 2][y] = EMPTY;
            result = true;
        }
        if (result) return result;
        if (x <= 7 && board[x + 1][y] == type && board[x + 2][y] == type) {
            board[x][y] = BOMB;
            board[x][y - 1] = EMPTY;
            board[x][y + 1] = EMPTY;
            board[x + 1][y] = EMPTY;
            board[x + 2][y] = EMPTY;
            result = true;
        }
        if (result) return result;
    }
    if (x >= 1 && x <= 8 && board[x - 1][y] == type && board[x + 1][y] == type) {
        if (y >= 2 && board[x][y - 2] == type && board[x][y - 1] == type) {
            board[x][y] = BOMB;
            board[x - 1][y] = EMPTY;
            board[x + 1][y] = EMPTY;
            board[x][y - 2] = EMPTY;
            board[x][y - 1] = EMPTY;
        }
        if (result) return result;
        if (y <= 7 && board[x][y + 2] == type && board[x][y + 1] == type) {
            board[x][y] = BOMB;
            board[x - 1][y] = EMPTY;
            board[x + 1][y] = EMPTY;
            board[x][y + 2] = EMPTY;
            board[x][y + 1] = EMPTY;
        }
        if (result) return result;
    }
    return result;
}

bool Model::checkRowClear(int x, int y, ElementType type) {
    int rowcount = 1;
    //������
    for (int i = y - 1; i >= 0 && board[x][i] == type; --i) {
        rowcount++;
    }
    // ������
    for (int i = y + 1; i < 10 && board[x][i] == type; ++i) {
        rowcount++;
    }
    if (rowcount >= 4) {
        for (int i = y - 1; i >= 0 && board[x][i] == type; --i) {
            board[x][i] = EMPTY;
        }
        for (int i = y + 1; i < 10 && board[x][i] == type; ++i) {
            board[x][i] = EMPTY;
        }
        board[x][y] = ROW_CLEAR;
        return true;
    }
    return false;
}

bool Model::checkColumnClear(int x, int y, ElementType type) {
    int columncount = 1;
    // ������
    for (int i = x - 1; i >= 0 && board[i][y] == type; --i) {
        columncount++;
    }
    // ������
    for (int i = x + 1; i < 10 && board[i][y] == type; ++i) {
        columncount++;
    }
    if (columncount >= 4) {
        for (int i = x - 1; i >= 0 && board[i][y] == type; --i) {
            board[i][y] = EMPTY;
        }
        for (int i = x + 1; i < 10 && board[i][y] == type; ++i) {
            board[i][y] = EMPTY;
        }
        board[x][y] = COLUMN_CLEAR;
        return true;
    }
    return false;
}

bool Model::checkRow(int x, int y, ElementType type) {
    int rowcount = 1;
    // ������
    for (int i = y - 1; i >= 0 && board[x][i] == type; --i) {
        rowcount++;
    }
    // ������
    for (int i = y + 1; i < 10 && board[x][i] == type; ++i) {
        rowcount++;
    }
    if (rowcount >= 3) {
        for (int i = y - 1; i >= 0 && board[x][i] == type; --i) {
            board[x][i] = EMPTY;
        }
        for (int i = y + 1; i < 10 && board[x][i] == type; ++i) {
            board[x][i] = EMPTY;
        }
        board[x][y] = EMPTY;
        return true;
    }
    return false;
}

bool Model::checkColumn(int x, int y, ElementType type) {
    int columncount = 1;
    // ������
    for (int i = x - 1; i >= 0 && board[i][y] == type; --i) {
        columncount++;
    }
    // ������
    for (int i = x + 1; i < 10 && board[i][y] == type; ++i) {
        columncount++;
    }
    if (columncount >= 3) {
        for (int i = x - 1; i >= 0 && board[i][y] == type; --i) {
            board[i][y] = EMPTY;
        }
        for (int i = x + 1; i < 10 && board[i][y] == type; ++i) {
            board[i][y] = EMPTY;
        }
        board[x][y] = EMPTY;
        return true;
    }
    return false;
}

void Model::clearSpecialElement(ElementType type, int x, int y) {

}

void Model::cleardoubleSpecialElements(ElementType type1, int x1, int y1, ElementType type2, int x2, int y2) {

}

bool Model::isValidSwap(int x1, int y1, int x2, int y2) {
    // ��齻���Ƿ�Ϸ�
    if (x1 < 0 || x1 >= 10 || y1 < 0 || y1 >= 10 ||
        x2 < 0 || x2 >= 10 || y2 < 0 || y2 >= 10) {
        return false; // ������Ϸ��߽磬�����Ƿ�
    }
    if (abs(x1 - x2) + abs(y1 - y2) != 1) {
        return false; // ������λ�ã������Ƿ�
    }

    // ִ�н���
    std::swap(board[x1][y1], board[x2][y2]);

    //�����Ƿ�������Ԫ��
    if (static_cast<int> (board[x1][y1]) >= 7 && static_cast<int> (board[x2][y2]) >= 7) {//��Ϊ����Ԫ��
        ElementType type1 = board[x1][y1], type2 = board[x2][y2];
        if (static_cast<int> (type1) >= static_cast<int> (type2)) {
            cleardoubleSpecialElements(type1, x1, y1, type2, x2, y2);
        }
        else {
            cleardoubleSpecialElements(type2, x2, y2, type1, x1, y1);
        }
        return true;
    }
    else if (static_cast<int> (board[x1][y1]) >= 7) {//������x1��y1����Ϊ����Ԫ��
        if (board[x1][y1] == RAINBOW_CIRCLE) clearRainbowCircle(x1, y1, x2, y2);
        else clearSpecialElement(board[x1][y1], x1, y1);
        return true;
    }
    else if (static_cast<int> (board[x2][y2]) >= 7) {//������x2��y2����Ϊ����Ԫ��
        if (board[x2][y2] == RAINBOW_CIRCLE) clearRainbowCircle(x2, y2, x1, y1);
        else clearSpecialElement(board[2][y2], x2, y2);
        return true;
    }

    // �������������̬
    bool result1 = checkSpecialElement(x2, y2);

    // ���û��������̬������������������
    if (!result1) {
        result1 = checkRowOrColumn(x2, y2, true); // ���������
        if (!result1) {
            result1 = checkRowOrColumn(x2, y2, false); // ���������
        }
    }

    bool result2 = checkSpecialElement(x1, y1);
    if (!result2) {
        result2 = checkRowOrColumn(x1, y1, true);
        if (!result2) {
            result2 = checkRowOrColumn(x1, y1, false);
        }
    }

    // ���û���κ�������������������
    //if (!result1 && !result2) {
        //std::swap(board[x1][y1], board[x2][y2]);
    //}

    return result1||result2;
}

bool Model::checkRowOrColumn(int x, int y, bool isRow) {
    // ����л����Ƿ��������Ŀ�����
    ElementType type = board[x][y];
    // �����
    if (isRow) {
        return checkRow(x, y, type);
    }
    // �����
    else {
        return checkColumn(x, y, type);
    }
}

bool Model::checkSpecialElement(int x, int y) {
    ElementType type = board[x][y];
    bool result = false;

    // ���ʺ�Ȧ
    result = checkRainbowCircle(x, y, type);
    if (result) return result;

    // ���ը��
    //L��
    result = checkBombL(x, y, type);
    if (result) return result;
    //T��
    result = checkBombT(x, y, type);
    if (result) return result;

    //���������
    result = checkRowClear(x, y, type);
    if (result) return result;

    //���������
    result = checkColumnClear(x, y, type);

    return result;
}

void Model::handleEmptyAndGravity() {
    // ��������ÿһ��
    for (int j = 0; j < 10; ++j) {
        int emptyCount = 0; // ͳ�ƿ�λ����

        // �ӵײ����ϱ���
        for (int i = 9; i >= 0; --i) {
            if (board[i][j] == EMPTY) {
                emptyCount++;
            }
            else if (emptyCount > 0) {
                // �����ǰλ�÷ǿ����·��п�λ��ִ���������
                std::swap(board[i][j], board[i + emptyCount][j]);
            }
        }

        // ����λ
        for (int k = emptyCount - 1; k >= 0; --k) {
            board[k][j] = getRandomFruitType(); // ������ˮ������
            updateScore();
        }
    }
}

ElementType Model::getRandomFruitType() {
    // ���������ˮ������
    return static_cast<ElementType>(rand() % 6 + 1);
}

void Model::refreshBoard() {
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            board[i][j] = getRandomFruitType();
        }
    }
}

void Model::updateScore() {
    if (currentDifficulty == EASY) score++;
    else score += 2;
}

void Model::clearElements() {
    // ����Ԫ�ز�����״̬
    bool cleared;
    do {
        cleared = false;
        std::vector<std::vector<bool>> toClear(10, std::vector<bool>(10, false));

        // �����
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (board[i][j] == board[i][j + 1] && board[i][j] == board[i][j + 2]) {
                    toClear[i][j] = toClear[i][j + 1] = toClear[i][j + 2] = true;
                    cleared = true;
                }
            }
        }

        // �����
        for (int j = 0; j < 10; ++j) {
            for (int i = 0; i < 8; ++i) {
                if (board[i][j] == board[i + 1][j] && board[i][j] == board[i + 2][j]) {
                    toClear[i][j] = toClear[i + 1][j] = toClear[i + 2][j] = true;
                    cleared = true;
                }
            }
        }

        // �����ǵ�Ԫ��
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                if (toClear[i][j]) {
                    board[i][j] = EMPTY;
                    handleEmptyAndGravity();
                }
            }
        }

    } while (cleared);
}

bool Model::isGameOver() {
    return stepsLeft == 0;
}