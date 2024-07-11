#pragma once
#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include "Model.h"

class ViewModel {
public:
    ViewModel();

    // ��ʼ����Ϸ
    void saveAndResetSimple();
    void saveAndResetDifficult();

    // ���а�
    void getSimpleRank();
    void getDifficultRank();

    // ��Ϸ״̬����
    void fetchUserSwap(int x1, int y1, int x2, int y2);
    GameState getGameState() const;
    std::vector<std::vector<ElementType>> getBoardState() const;
    int getScoreState() const;
    int getStepsLeftState() const;
    std::string getTaskState() const;
    std::unordered_map<ToolType, int> getToolState() const;
    bool isGameOver();

    // ���߲���
    void getToolUse(ToolType tool, int x, int y);

    // �������
    int getNewTask() const;
    void fetchUpdateTaskProgress();

    // ���ȹ���
    void saveGameProgress();
    void loadGameProgress();

    // ���ֺͼ�¼����
    int fetchPoint() const;
    bool getIsNewRecord();

private:
    Model model;
};

#endif