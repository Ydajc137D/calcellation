#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <unordered_map>

enum Difficulty { EASY, HARD };
enum ElementType {
    EMPTY,
    APPLE,
    ORANGE,
    BANANA,
    PEACH,
    PINEAPPLE,
    GRAPE,
    ROW_CLEAR,
    COLUMN_CLEAR,
    BOMB,
    RAINBOW_CIRCLE
};
enum ToolType { HAMMER, BOMB_TRANSFORM, REFRESH, ROW_CLEAR_TRANSFORM, COLUMN_CLEAR_TRANSFORM };

struct GameState {
    std::vector<std::vector<ElementType>> board;
    int score;
    int stepsLeft;
    //int timeLeft;
    Task currentTask;
    bool isNewRecord;
    std::unordered_map<ToolType, int> ToolNumber;
};

struct Task {
    int TaskType;
    std::string Taskcontent;
};

class Model {
public:
    Model();

    // ��ʼ����Ϸ״̬
    void initializeGame(Difficulty difficulty);

    // ����ͼ�����Ϸ����
    bool saveGame();
    bool loadGame();

    // �����߼�
    bool isValidSwap(int x1, int y1, int x2, int y2);
    void processSwap(int x1, int y1, int x2, int y2);
    void handleEmptyAndGravity();
    bool checkSpecialElement(int x, int y);
    bool checkRowOrColumn(int x, int y, bool isRow);
    bool checkRainbowCircle(int x, int y, ElementType type);
    bool checkBombL(int x, int y, ElementType type);
    bool checkBombT(int x, int y, ElementType type);
    bool checkRowClear(int x, int y, ElementType type);
    bool checkColumnClear(int x, int y, ElementType type);
    bool checkRow(int x, int y, ElementType type);
    bool checkColumn(int x, int y, ElementType type);
    //���´�����
    //ps:���ⷽ���峡ʱ����������ⷽ��
    void clearSpecialElement(ElementType type, int x, int y);
    void cleardoubleSpecialElements(ElementType type1, int x1, int y1, ElementType type2, int x2, int y2);

    void clearRowClear(int x, int y);
    void clearColumnClear(int x, int y);
    void clearBomb(int x, int y);
    void clearRainbowCircle(int x1, int y1, int x2, int y2);

    void RainbowCircle2RainbowCircle(int x1, int y1, int x2, int y2);//����
    void RainbowCircle2Bomb(int x1, int y1, int x2, int y2);//��7*7
    void RainbowCircle2RowClear(int x1, int y1, int x2, int y2);//��5��
    void RainbowCircle2ColumnClear(int x1, int y1, int x2, int y2);//��5��

    void Bomb2Bomb(int x1, int y1, int x2, int y2);//��5*5
    void Bomb2RowClear(int x1, int y1, int x2, int y2);//��3��
    void Bomb2ColumnClear(int x1, int y1, int x2, int y2);//��3��

    void Column2Row(int x1, int y1, int x2, int y2);//��ʮ��
    void Column2Column(int x1, int y1, int x2, int y2);//������
    void Row2Row(int x1, int y1, int x2, int y2);//������

    // �����߼�
    void useTool(ToolType tool, int x, int y);//���ǵ���ʣ�����
    void useHammer(int x, int y);
    void useBombTran(int x, int y);
    void useRefresh(int x, int y);
    void useRow(int x, int y);
    void useColumn(int x, int y);

    // �����߼�
    void assignNewTask();
    bool checkTaskCompletion();

    // ���а����
    void loadLeaderboard();
    void saveLeaderboard();
    bool BreakRecord();

    // ��ȡ��Ϸ״̬
    GameState getGameState() const;
    bool isGameOver();

private:
    std::vector<std::vector<ElementType>> board;
    int score;
    int stepsLeft;
    //int timeLeft;
    Task currentTask;
    Difficulty currentDifficulty;
    std::unordered_map<Difficulty, int> highScores;
    bool isNewRecord;
    std::unordered_map<ToolType, int> ToolNumber;//ÿ�ֵ���ʣ������

    ElementType getRandomFruitType();
    void refreshBoard();
    void clearElements();
    void updateScore();

private:
    friend class ViewModel;
};

#endif