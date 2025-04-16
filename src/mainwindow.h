#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWebSocket>

#include "gameboard.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    QString getLastSentMessage() const { return lastSentMessageIs; }
    void setTestingMode(bool testing) { isTestingFlag = testing; }

    public slots:
        void onCreateSessionClicked();
    void onJoinSessionClicked();
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onCellClicked(int x, int y);

private:
    void setupMainMenu();
    void waitSecondPlayer();
    void setupGameBoardWhenTwoPlayersAreConnected();
    void clearLayout();
    void processShotResult(const QString& message);
    void processOpponentShot(const QString& message);

    QWebSocket* webSocketToGame = nullptr;
    QWidget* centralWidgetGame = nullptr;
    QVBoxLayout* mainLayoutGame = nullptr;
    QLabel* statusLabel = nullptr;
    QLineEdit* sessionIdInput = nullptr;
    QPushButton* createButton = nullptr;
    QPushButton* joinButton = nullptr;
    GameBoard* gameBoardForPlay = nullptr;
    QString currentSessionId;
    bool isMyTurn = false;
    int lastShotX = -1;
    int lastShotY = -1;
    bool isSettingUpMainMenu = false;
    bool isClosing = false;
    QString lastSentMessageIs;
    bool isTestingFlag = false;
};

#endif