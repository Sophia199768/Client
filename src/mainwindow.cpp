#include "mainwindow.h"

#include <QAbstractSocket>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTimer>

namespace {
constexpr QSize defaultWindowSize{600, 400};
constexpr auto webSocketUrl = "ws://localhost:8080";
constexpr auto sessionInputPlaceholder = "Введите ID сессии";
constexpr auto createSessionText = "Создать сессию";
constexpr auto joinSessionText = "Присоединиться к сессии";
constexpr auto askingToConnectText = "Введите ID сессии для создания или присоединения";
constexpr auto gameStartedText = "Игра началась!";
constexpr auto playerBoardLabel = "Ваше поле";
constexpr auto opponentBoardLabel = "Поле противника";
constexpr auto yourTurnText = "Ваш ход!";
constexpr auto opponentTurnText = "Ожидание хода противника...";
constexpr auto askingToSessionId = "Пожалуйста, введите ID сессии";
constexpr auto disconnectedError = "Отключено от сервера";
constexpr auto gameOverTitle = "Игра окончена";
constexpr auto victory = "Победа!";
constexpr auto defeat = "Поражение!";
constexpr int gameOverDialogDelayMs = 100;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , webSocketToGame(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , centralWidgetGame(new QWidget(this))
    , mainLayoutGame(new QVBoxLayout(centralWidgetGame))
    , gameBoardForPlay(new GameBoard(this))
    , lastSentMessageIs("")
    , isTestingFlag(false) {
    setCentralWidget(centralWidgetGame);
    setWindowTitle(tr("Игра морской бой"));
    resize(defaultWindowSize);

    connect(webSocketToGame, &QWebSocket::connected, this, &MainWindow::onConnected);
    connect(webSocketToGame, &QWebSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(webSocketToGame, &QWebSocket::textMessageReceived, this, &MainWindow::onTextMessageReceived);
    connect(gameBoardForPlay, &GameBoard::cellClicked, this, &MainWindow::onCellClicked);

    webSocketToGame->open(QUrl(webSocketUrl));

    setupMainMenu();
}

MainWindow::~MainWindow() {
    isClosing = true;
    if (webSocketToGame->state() != QAbstractSocket::UnconnectedState) {
        webSocketToGame->close();
    }
}

void MainWindow::setupMainMenu() {
    if (isSettingUpMainMenu) {
        return;
    }
    isSettingUpMainMenu = true;

    gameBoardForPlay->cleanFiledForNewGame();
    gameBoardForPlay->setOpponentBoardClickOrNot(false);
    gameBoardForPlay->getPlayerWidget()->setVisible(false);
    gameBoardForPlay->getOpponentWidget()->setVisible(false);

    clearLayout();

    sessionIdInput = new QLineEdit(this);
    sessionIdInput->setPlaceholderText(tr(sessionInputPlaceholder));
    mainLayoutGame->addWidget(sessionIdInput);

    createButton = new QPushButton(tr(createSessionText), this);
    joinButton = new QPushButton(tr(joinSessionText), this);
    mainLayoutGame->addWidget(createButton);
    mainLayoutGame->addWidget(joinButton);

    statusLabel = new QLabel(tr(askingToConnectText), this);
    mainLayoutGame->addWidget(statusLabel);
    mainLayoutGame->addStretch();

    connect(createButton, &QPushButton::clicked, this, &MainWindow::onCreateSessionClicked);
    connect(joinButton, &QPushButton::clicked, this, &MainWindow::onJoinSessionClicked);
    connect(gameBoardForPlay, &GameBoard::cellClicked, this, &MainWindow::onCellClicked);

    isMyTurn = false;
    lastShotX = -1;
    lastShotY = -1;
    currentSessionId.clear();

    isSettingUpMainMenu = false;
}

void MainWindow::waitSecondPlayer() {
    clearLayout();

    statusLabel = new QLabel(
            tr("ID сессии: %1\nОжидание второго игрока...").arg(currentSessionId), this);
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayoutGame->addWidget(statusLabel);
    mainLayoutGame->addStretch();

    gameBoardForPlay->getPlayerWidget()->setVisible(false);
    gameBoardForPlay->getOpponentWidget()->setVisible(false);
}

void MainWindow::setupGameBoardWhenTwoPlayersAreConnected() {
    clearLayout();

    statusLabel = new QLabel(tr(gameStartedText), this);
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayoutGame->addWidget(statusLabel);

    auto* boardsWidget = new QWidget(this);
    mainLayoutGame->addWidget(boardsWidget);
    auto* boardsLayout = new QHBoxLayout;
    boardsWidget->setLayout(boardsLayout);

    auto* playerContainer = new QWidget(boardsWidget);
    boardsLayout->addWidget(playerContainer);
    auto* playerContainerLayout = new QVBoxLayout(playerContainer);

    auto* playerLabel = new QLabel(tr(playerBoardLabel), playerContainer);
    playerLabel->setAlignment(Qt::AlignCenter);
    playerContainerLayout->addWidget(playerLabel);

    QWidget* playerWidget = gameBoardForPlay->getPlayerWidget();
    playerContainerLayout->addWidget(playerWidget);

    auto* opponentContainer = new QWidget(boardsWidget);
    boardsLayout->addWidget(opponentContainer);
    auto* opponentContainerLayout = new QVBoxLayout(opponentContainer);

    auto* opponentLabel = new QLabel(tr(opponentBoardLabel), opponentContainer);
    opponentLabel->setAlignment(Qt::AlignCenter);
    opponentContainerLayout->addWidget(opponentLabel);

    QWidget* opponentWidget = gameBoardForPlay->getOpponentWidget();
    opponentContainerLayout->addWidget(opponentWidget);

    mainLayoutGame->addStretch();

    playerWidget->setVisible(true);
    opponentWidget->setVisible(true);
    gameBoardForPlay->setOpponentBoardClickOrNot(isMyTurn);
}

void MainWindow::clearLayout() {
    while (auto* item = mainLayoutGame->takeAt(0)) {
        if (auto* widget = item->widget()) {
            widget->disconnect();
            delete widget;
        }
        if (auto* layout = item->layout()) {
            while (auto* layoutItem = layout->takeAt(0)) {
                if (auto* widget = layoutItem->widget()) {
                    widget->disconnect();
                    delete widget;
                }
                delete layoutItem;
            }
            delete layout;
        }
        delete item;
    }
    statusLabel = nullptr;
    sessionIdInput = nullptr;
    createButton = nullptr;
    joinButton = nullptr;
}

void MainWindow::onCreateSessionClicked() {
    if (!sessionIdInput) {
        return;
    }
    const QString sessionId = sessionIdInput->text().trimmed();
    if (sessionId.isEmpty()) {
        if (!isTestingFlag) {
            QMessageBox::warning(this, tr("Ошибка"), tr(askingToSessionId));
        }
        return;
    }
    currentSessionId = sessionId;
    lastSentMessageIs = QString("create:%1").arg(sessionId);
    webSocketToGame->sendTextMessage(lastSentMessageIs);
}

void MainWindow::onJoinSessionClicked() {
    if (!sessionIdInput) {
        return;
    }
    const QString sessionId = sessionIdInput->text().trimmed();
    if (sessionId.isEmpty()) {
        if (!isTestingFlag) {
            QMessageBox::warning(this, tr("Ошибка"), tr(askingToSessionId));
        }
        return;
    }
    currentSessionId = sessionId;
    lastSentMessageIs = QString("join:%1").arg(sessionId);
    webSocketToGame->sendTextMessage(lastSentMessageIs);
}

void MainWindow::onConnected() {
    if (statusLabel) {
        statusLabel->setText(tr("Подключено к серверу"));
    }
}

void MainWindow::onDisconnected() {
    if (isClosing) {
        return;
    }
    if (!isTestingFlag) {
        QMessageBox::critical(this, tr("Ошибка"), tr(disconnectedError));
    }
    setupMainMenu();
}

void MainWindow::onTextMessageReceived(const QString& message) {

    if (message.startsWith("Session created:")) {
        currentSessionId = message.mid(QString("Session created: ").length()).trimmed();
        if (!statusLabel) {
            setupMainMenu();
            return;
        }
        waitSecondPlayer();
    } else if (message.startsWith("Connected to session:")) {
        currentSessionId =
            message.mid(QString("Connected to session: ").length()).split('\n').first().trimmed();
        if (message.contains("Your board:")) {
            gameBoardForPlay->parseAndSaveBoard(message);
            setupGameBoardWhenTwoPlayersAreConnected();
        } else {
            waitSecondPlayer();
        }
    } else if (message.contains("Your board:")) {
        gameBoardForPlay->parseAndSaveBoard(message);
        setupGameBoardWhenTwoPlayersAreConnected();
    } else if (message == "Your turn") {
        isMyTurn = true;
        gameBoardForPlay->setOpponentBoardClickOrNot(true);
        if (statusLabel) {
            statusLabel->setText(tr(yourTurnText));
        }
    } else if (message.startsWith("Shot result:")) {
        processShotResult(message);
    } else if (message.startsWith("Opponent shot at")) {
        processOpponentShot(message);
    } else if (message == "Game over: You win!" || message == "Game over: You lose!") {
        const bool gameResult = message.contains("win");
        setupMainMenu();

        if (!isTestingFlag) {
            QTimer::singleShot(gameOverDialogDelayMs, this, [this, gameResult] {
                QMessageBox::information(
                    this, tr(gameOverTitle), tr(gameResult ? victory : defeat));
            });
        }
    }
}

void MainWindow::onCellClicked(int x, int y) {
    if (!isMyTurn) {
        return;
    }
    lastShotX = x;
    lastShotY = y;
    webSocketToGame->sendTextMessage(QString("shoot %1 %2").arg(x).arg(y));
}

void MainWindow::processShotResult(const QString& message) {
    static const QRegularExpression re{"Shot result: (\\w+)"};
    const QRegularExpressionMatch match = re.match(message);
    if (match.hasMatch() && lastShotX >= 0 && lastShotY >= 0) {
        const QString result = match.captured(1);
        gameBoardForPlay->updateOpponentBoard(lastShotX, lastShotY, result);
        if (result == "hit" || result == "kill") {
            isMyTurn = true;
            gameBoardForPlay->setOpponentBoardClickOrNot(true);
            if (statusLabel) {
                statusLabel->setText(tr(yourTurnText));
            }
        } else if (result == "miss") {
            isMyTurn = false;
            gameBoardForPlay->setOpponentBoardClickOrNot(false);
            if (statusLabel) {
                statusLabel->setText(tr(opponentTurnText));
            }
        }
        lastShotX = -1;
        lastShotY = -1;
    }
}

void MainWindow::processOpponentShot(const QString& message) {
    static const QRegularExpression re{R"(Opponent shot at \((\d+), (\d+)\): (\w+))"};
    const QRegularExpressionMatch match = re.match(message);
    if (match.hasMatch()) {
        const int x = match.captured(1).toInt();
        const int y = match.captured(2).toInt();
        const QString result = match.captured(3);
        gameBoardForPlay->updatePlayerBoard(x, y, result);
    }
}