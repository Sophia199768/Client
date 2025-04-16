#include "gameboard.h"

namespace {
constexpr int сellSize = 30;
constexpr auto playerShipStyle = "background-color: blue; border: 1px solid black;";
constexpr auto playerEmptyCellStyle = "background-color: white; border: 1px solid black;";
constexpr auto opponentDefaultStyle = "background-color: gray; border: 1px solid black;";
constexpr auto missShootStyle = "background-color: black; border: 1px solid black;";
constexpr auto winShootStyle = "background-color: red; border: 1px solid black;";
}

GameBoard::GameBoard(QWidget* parent)
    : QObject(parent)
    , playerWidgetFirst(new QWidget(parent))
    , opponentWidgetSecond(new QWidget(parent))
    , playerLayoutFirst(new QGridLayout(playerWidgetFirst))
    , opponentLayoutSecond(new QGridLayout(opponentWidgetSecond)) {
    playerBoardFirst.resize(SIZE, std::vector<char>(SIZE, '.'));
    opponentBoardSecond.resize(SIZE, std::vector<char>(SIZE, '.'));
    playerWidgetFirst->setVisible(false);
    opponentWidgetSecond->setVisible(false);
}

GameBoard::~GameBoard() = default;

void GameBoard::cleanFiledForNewGame() {
    playerBoardFirst.clear();
    opponentBoardSecond.clear();
    playerBoardFirst.resize(SIZE, std::vector<char>(SIZE, '.'));
    opponentBoardSecond.resize(SIZE, std::vector<char>(SIZE, '.'));

    delete playerWidgetFirst;
    delete opponentWidgetSecond;

    playerWidgetFirst = new QWidget(qobject_cast<QWidget*>(parent()));
    opponentWidgetSecond = new QWidget(qobject_cast<QWidget*>(parent()));
    playerLayoutFirst = new QGridLayout(playerWidgetFirst);
    opponentLayoutSecond = new QGridLayout(opponentWidgetSecond);

    setupPlayerBoard();
    setupOpponentBoard();

    playerWidgetFirst->setVisible(false);
    opponentWidgetSecond->setVisible(false);
}

void GameBoard::parseAndSaveBoard(const QString& message) {
    playerBoardFirst.clear();
    playerBoardFirst.resize(SIZE, std::vector<char>(SIZE, '.'));
    QStringList lines = message.split('\n', Qt::SkipEmptyParts);
    bool boardStarted = false;
    int row = 0;

    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.contains("Your board:")) {
            boardStarted = true;
            continue;
        }
        if (boardStarted and trimmedLine.length() == SIZE and row < SIZE) {
            for (int col = 0; col < SIZE and col < trimmedLine.length(); ++col) {
                playerBoardFirst[row][col] = trimmedLine[col].toLatin1();
            }
            row++;
        }
    }

    setupPlayerBoard();
    setupOpponentBoard();
    playerWidgetFirst->setVisible(true);
    opponentWidgetSecond->setVisible(true);
}

void GameBoard::setupPlayerBoard() {
    while (QLayoutItem* item = playerLayoutFirst->takeAt(0)) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            QPushButton* cell = new QPushButton();
            cell->setFixedSize(сellSize, сellSize);
            if (playerBoardFirst[i][j] == 'S') {
                cell->setStyleSheet(playerShipStyle);
            } else {
                cell->setStyleSheet(playerEmptyCellStyle);
            }
            cell->setEnabled(false);
            playerLayoutFirst->addWidget(cell, i, j);
        }
    }
}

void GameBoard::setupOpponentBoard() {
    while (QLayoutItem* item = opponentLayoutSecond->takeAt(0)) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            QPushButton* cell = new QPushButton();
            cell->setFixedSize(сellSize, сellSize);
            cell->setStyleSheet(opponentDefaultStyle);
            cell->setEnabled(false);
            connect(cell, &QPushButton::clicked, this, [=]() { emit cellClicked(i, j); });
            opponentLayoutSecond->addWidget(cell, i, j);
        }
    }
}

void GameBoard::setOpponentBoardClickOrNot(bool interactive) {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            QLayoutItem* item = opponentLayoutSecond->itemAtPosition(i, j);
            if (item && item->widget()) {
                QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
                cell->setEnabled(interactive);
                if (!cell->styleSheet().contains("background-color: black") &&
                    !cell->styleSheet().contains("background-color: red")) {
                    cell->setStyleSheet(
                        QString("background-color: %1; border: 1px solid black;").arg(interactive ? "white" : "gray"));
                }
                cell->repaint();
                cell->update();
            }
        }
    }
    opponentWidgetSecond->repaint();
    opponentWidgetSecond->update();
}

void GameBoard::updatePlayerBoard(int x, int y, const QString& result) {
    QLayoutItem* item = playerLayoutFirst->itemAtPosition(x, y);
    if (item && item->widget()) {
        QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
        if (result == "miss") {
            cell->setStyleSheet(missShootStyle);
        } else if (result == "hit" || result == "kill") {
            cell->setStyleSheet(winShootStyle);
        }
        cell->repaint();
        cell->update();
    }
}

void GameBoard::updateOpponentBoard(int x, int y, const QString& result) {
    QLayoutItem* item = opponentLayoutSecond->itemAtPosition(x, y);
    if (item && item->widget()) {
        QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
        if (result == "miss") {
            cell->setStyleSheet(missShootStyle);
        } else if (result == "hit" || result == "kill") {
            cell->setStyleSheet(winShootStyle);
        }
        cell->setEnabled(false);
        cell->repaint();
        cell->update();
    }
}

QWidget* GameBoard::getPlayerWidget() const {
    return playerWidgetFirst;
}

QWidget* GameBoard::getOpponentWidget() const {
    return opponentWidgetSecond;
}