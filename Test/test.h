#ifndef TEST_H
#define TEST_H

#include <QMainWindow>
#include <QObject>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QTimer>
#include <QPushButton>
#include <unordered_map>
#include <functional>
#include <windows.h>
#include <fileapi.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class Test;
}
QT_END_NAMESPACE

class Test : public QMainWindow
{
    Q_OBJECT

public:
    Test(QWidget *parent = nullptr);
    ~Test();

private slots:

    void Modify();

    void on_buttonBox_accepted();

    void on_rewrite_clicked();

    void on_createCopy_clicked();

    void on_onTimer_clicked();

    void on_once_clicked();

    void on_stopTimer_clicked();

private:
    Ui::Test *ui;
    std::unordered_map<QString, std::function<qint64(qint64, qint64)>> operations;
    QTimer *timer;
};
#endif // TEST_H
