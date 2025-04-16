#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QGridLayout>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <vector>

class GameBoard : public QObject {
    Q_OBJECT

public:
    explicit GameBoard(QWidget* parent = nullptr);
    ~GameBoard();

    void parseAndSaveBoard(const QString& message);
    QWidget* getPlayerWidget() const;
    QWidget* getOpponentWidget() const;
    void setOpponentBoardClickOrNot(bool interactive);
    void updatePlayerBoard(int x, int y, const QString& result);
    void updateOpponentBoard(int x, int y, const QString& result);
    void cleanFiledForNewGame();

    std::vector<std::vector<char>> playerBoardFirst;
    std::vector<std::vector<char>> opponentBoardSecond;

    signals:
        void cellClicked(int x, int y);

private:
    void setupPlayerBoard();
    void setupOpponentBoard();

    static constexpr int SIZE = 10;
    QWidget* playerWidgetFirst = nullptr;
    QWidget* opponentWidgetSecond = nullptr;
    QGridLayout* playerLayoutFirst = nullptr;
    QGridLayout* opponentLayoutSecond = nullptr;
};

#endif