#include <QtTest/QtTest>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QWebSocket>

#include "../src/mainwindow.h"

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanup();
    void testCreateSessionEmptyInput();
    void testJoinSessionEmptyInput();
    void testJoinSessionValidInput();

private:
    MainWindow* mainWindow_ = nullptr;
    static constexpr auto sessionIdPlaceholder = "Введите ID сессии";
    static constexpr auto createSessionText = "Создать сессию";
    static constexpr auto joinSessionText = "Присоединиться к сессии";
    static constexpr auto mainMenuPrompt = "Введите ID сессии для создания или присоединения";
};

void TestMainWindow::initTestCase() {
    mainWindow_ = new MainWindow();
    QVERIFY(mainWindow_ != nullptr);
    mainWindow_->setTestingMode(true);
    QTest::qWait(100);
}

void TestMainWindow::cleanup() {
    delete mainWindow_;
    mainWindow_ = new MainWindow();
    mainWindow_->setTestingMode(true);
    QTest::qWait(100);
}

void TestMainWindow::testCreateSessionEmptyInput() {
    QLineEdit* sessionIdInput = mainWindow_->findChild<QLineEdit*>();
    QVERIFY(sessionIdInput != nullptr);

    sessionIdInput->clear();

    QPushButton* createButton = nullptr;
    QList<QPushButton*> buttons = mainWindow_->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        if (button->text() == QString(createSessionText)) {
            createButton = button;
            break;
        }
    }
    QVERIFY(createButton != nullptr);
    QTest::mouseClick(createButton, Qt::LeftButton);

    QCOMPARE(mainWindow_->getLastSentMessage(), QString(""));
}

void TestMainWindow::testJoinSessionEmptyInput() {
    QLineEdit* sessionIdInput = mainWindow_->findChild<QLineEdit*>();
    QVERIFY(sessionIdInput != nullptr);

    sessionIdInput->clear();

    QPushButton* joinButton = nullptr;
    QList<QPushButton*> buttons = mainWindow_->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        if (button->text() == QString(joinSessionText)) {
            joinButton = button;
            break;
        }
    }
    QVERIFY(joinButton != nullptr);

    QTest::mouseClick(joinButton, Qt::LeftButton);
    QCOMPARE(mainWindow_->getLastSentMessage(), QString(""));
}

void TestMainWindow::testJoinSessionValidInput() {
    QLineEdit* sessionIdInput = mainWindow_->findChild<QLineEdit*>();
    QVERIFY(sessionIdInput != nullptr);

    QString testSessionId = "test123";
    sessionIdInput->setText(testSessionId);

    QPushButton* joinButton = nullptr;
    QList<QPushButton*> buttons = mainWindow_->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        if (button->text() == QString(joinSessionText)) {
            joinButton = button;
            break;
        }
    }
    QVERIFY(joinButton != nullptr);

    QTest::mouseClick(joinButton, Qt::LeftButton);

    QCOMPARE(mainWindow_->getLastSentMessage(), QString("join:%1").arg(testSessionId));
}

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"