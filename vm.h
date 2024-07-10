#pragma once
#include <iostream>
#include "model.h"
using namespace std;

class ViewModel {
public:
    ViewModel(Model& model) : model(model) {}

    //���˵�����
    //�������˵����棬�����ӱ�����������ʼ�µļ�ģʽ����ʼ�µ�����ģʽ���鿴���а���˳���Ϸ�İ�ť
    void loadMainMenu();
    //��ʼ��ģʽ��Ϸ
    void startSimpleMode();
    //��ʼ����ģʽ��Ϸ
    void startHardMode();
    //�鿴���а�
    void viewLeaderboard();
    //�˳���Ϸ
    void exitGame();

    //�Ѷ���������
    //����Ƿ����¼�¼��difficulty������/����ģʽ
    void checkNewRecord(const std::string& difficulty);
    //��ʾ�������棨���Ѷ���ʾ��
    void displayEncouragement(const std::string& difficulty);

    //�����ͷǷ�����(�������޷�������
    //��������Ԫ�ص�λ�ã������Ƿ�������position�������꣩
    //�Ƿ�����û��ʵ�����ν���������ֱ���޷�������������ʾ��
    void swapElements(const std::pair<int, int>& position1, const std::pair<int, int>& position2);
    //������Ϸ״̬�������÷֡�ʣ�ಽ����ʣ��ʱ�䡢��ǰ����״̬�����������Ƿ�Ϸ�����Ҫ���µ����꼰������Ϣ
    void updateGameState();

    //����
    //��������
    void assignNewTask();
    //�����������ʣ�ಽ���͵���������rewardChoiceΪ���ѡ��Ľ���
    void completeTask(const std::string& rewardChoice);

    //����
    //ʹ�õ��ߣ�toolTypeΪ�������ͣ�positionΪ����λ�õ�����
    void useTool(const std::string& toolType, const std::pair<int, int>& position);

    //����
    //���浱ǰ��Ϸ״̬
    void saveGameState();
    //���ر������Ϸ״̬
    void loadSavedGame();

private:
    Model& model;
}; 
