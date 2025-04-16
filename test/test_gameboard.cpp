#include <QtTest/QtTest>

#include "../src/gameboard.h"

class TestGameBoard : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanup();

    void testPlayerBoardDisplay();
    void testOpponentBoardDisplay();
    void testSetOpponentBoardInteractive();
    void testUpdatePlayerBoard();
    void testUpdateOpponentBoard();
    void testCellClickedSignal();
    void testReset();

private:
    GameBoard* gameBoard_ = nullptr;
    static constexpr int boardSize = 10;
    static constexpr int cellSize = 30;
    static constexpr auto playerShipStyle = "background-color: blue; border: 1px solid black;";
    static constexpr auto playerEmptyStyle = "background-color: white; border: 1px solid black;";
    static constexpr auto opponentDefaultStyle = "background-color: gray; border: 1px solid black;";
    static constexpr auto opponentInteractiveStyle = "background-color: white; border: 1px solid black;";
    static constexpr auto missStyle = "background-color: black; border: 1px solid black;";
    static constexpr auto hitStyle = "background-color: red; border: 1px solid black;";
};

void TestGameBoard::initTestCase() {
    gameBoard_ = new GameBoard();
    QVERIFY(gameBoard_ != nullptr);
}

void TestGameBoard::cleanup() {
}

void TestGameBoard::testPlayerBoardDisplay() {
    QString boardMessage = "Your board:\n"
                          "S.........\n"
                          "..........\n"
                          "..........\n"
                          "..........\n"
                          "..........\n"
                          "..........\n"
                          "..........\n"
                          "..........\n"
                          "..........\n"
                          "..........";
    gameBoard_->parseAndSaveBoard(boardMessage);

    QVERIFY2(gameBoard_->playerBoardFirst[0][0] == 'S', "playerBoardFirst[0][0] should be 'S'");
    QVERIFY2(gameBoard_->playerBoardFirst[1][1] == '.', "playerBoardFirst[1][1] should be '.'");

    QGridLayout* playerLayout = qobject_cast<QGridLayout*>(gameBoard_->getPlayerWidget()->layout());
    QVERIFY(playerLayout != nullptr);

    QLayoutItem* item = playerLayout->itemAtPosition(0, 0);
    QVERIFY(item != nullptr);
    QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(playerShipStyle));
    QVERIFY(!cell->isEnabled());
    QCOMPARE(cell->size(), QSize(cellSize, cellSize));

    item = playerLayout->itemAtPosition(1, 1);
    QVERIFY(item != nullptr);
    cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(playerEmptyStyle));
    QVERIFY(!cell->isEnabled());
}

void TestGameBoard::testOpponentBoardDisplay() {
    QGridLayout* opponentLayout = qobject_cast<QGridLayout*>(gameBoard_->getOpponentWidget()->layout());
    QVERIFY(opponentLayout != nullptr);

    QLayoutItem* item = opponentLayout->itemAtPosition(0, 0);
    QVERIFY(item != nullptr);
    QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(opponentDefaultStyle));
    QVERIFY(!cell->isEnabled());
    QCOMPARE(cell->size(), QSize(cellSize, cellSize));
}

void TestGameBoard::testSetOpponentBoardInteractive() {
    gameBoard_->setOpponentBoardClickOrNot(true);

    QGridLayout* opponentLayout = qobject_cast<QGridLayout*>(gameBoard_->getOpponentWidget()->layout());
    QLayoutItem* item = opponentLayout->itemAtPosition(0, 0);
    QVERIFY(item != nullptr);
    QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(opponentInteractiveStyle));
    QVERIFY(cell->isEnabled());

    gameBoard_->setOpponentBoardClickOrNot(false);
    QCOMPARE(cell->styleSheet(), QString(opponentDefaultStyle));
    QVERIFY(!cell->isEnabled());
}

void TestGameBoard::testUpdatePlayerBoard() {
    gameBoard_->parseAndSaveBoard("Your board:\n..........\n..........\n..........\n..........\n..........\n..........\n..........\n..........\n..........\n..........");

    gameBoard_->updatePlayerBoard(0, 0, "miss");
    QGridLayout* playerLayout = qobject_cast<QGridLayout*>(gameBoard_->getPlayerWidget()->layout());
    QLayoutItem* item = playerLayout->itemAtPosition(0, 0);
    QVERIFY(item != nullptr);
    QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(missStyle));

    gameBoard_->updatePlayerBoard(1, 1, "hit");
    item = playerLayout->itemAtPosition(1, 1);
    QVERIFY(item != nullptr);
    cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(hitStyle));
}

void TestGameBoard::testUpdateOpponentBoard() {
    gameBoard_->updateOpponentBoard(0, 0, "miss");
    QGridLayout* opponentLayout = qobject_cast<QGridLayout*>(gameBoard_->getOpponentWidget()->layout());
    QLayoutItem* item = opponentLayout->itemAtPosition(0, 0);
    QVERIFY(item != nullptr);
    QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(missStyle));
    QVERIFY(!cell->isEnabled());

    gameBoard_->updateOpponentBoard(1, 1, "kill");
    item = opponentLayout->itemAtPosition(1, 1);
    QVERIFY(item != nullptr);
    cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(hitStyle));
    QVERIFY(!cell->isEnabled());
}

void TestGameBoard::testCellClickedSignal() {
    gameBoard_->setOpponentBoardClickOrNot(true);

    QSignalSpy spy(gameBoard_, &GameBoard::cellClicked);
    QGridLayout* opponentLayout = qobject_cast<QGridLayout*>(gameBoard_->getOpponentWidget()->layout());
    QLayoutItem* item = opponentLayout->itemAtPosition(0, 0);
    QVERIFY(item != nullptr);
    QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);

    QTest::mouseClick(cell, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 0);
    QCOMPARE(arguments.at(1).toInt(), 0);
}

void TestGameBoard::testReset() {
    gameBoard_->parseAndSaveBoard("Your board:\nS.........\n..........\n..........\n..........\n..........\n..........\n..........\n..........\n..........\n..........");
    QVERIFY(gameBoard_->getPlayerWidget()->isVisible());

    gameBoard_->cleanFiledForNewGame();
    QVERIFY(!gameBoard_->getPlayerWidget()->isVisible());
    QVERIFY(!gameBoard_->getOpponentWidget()->isVisible());

    QGridLayout* playerLayout = qobject_cast<QGridLayout*>(gameBoard_->getPlayerWidget()->layout());
    QLayoutItem* item = playerLayout->itemAtPosition(0, 0);
    QVERIFY(item != nullptr);
    QPushButton* cell = qobject_cast<QPushButton*>(item->widget());
    QVERIFY(cell != nullptr);
    QCOMPARE(cell->styleSheet(), QString(playerEmptyStyle));
}

QTEST_MAIN(TestGameBoard)
#include "test_gameboard.moc"