#include "test.h"
#include "./ui_test.h"

Test::Test(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Test)
{
    ui->setupUi(this);
    operations.insert({"AND", [](quint64 a, quint64 b) -> qint64 { return a & b;}});
    operations.insert({"OR", [](quint64 a, quint64 b) -> qint64 { return a | b;}});
    operations.insert({"XOR", [](quint64 a, quint64 b) -> qint64 { return a ^ b;}});
    operations.insert({"NAND", [](quint64 a, quint64 b) -> qint64 { return ~(a & b);}});
    operations.insert({"NOR", [](quint64 a, quint64 b) -> qint64 { return ~(a | b);}});
}

Test::~Test()
{
    delete ui;
}

void Test::Modify() {
    QDirIterator it(ui->inputFilePath->text(), QStringList() << "*" + ui->fileExtension->text(), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QDir dir;
        if (!dir.exists(ui->outputFilePath->text())) dir.mkpath(ui->outputFilePath->text());
        HANDLE h_file = CreateFile(it.next().toStdWString().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h_file == INVALID_HANDLE_VALUE) {
            it.next();
            continue;
        }
        CloseHandle(h_file);
        QFile file(it.filePath());
        QString new_file_name = ui->outputFilePath->text() + QFileInfo(file).fileName();
        while (QFile::exists(new_file_name) && ui->createCopy->isChecked()) {
            new_file_name = ui->outputFilePath->text() + QFileInfo(new_file_name).baseName() + QString("_copy.") + QFileInfo(file).completeSuffix();
        }
        QFile new_file(new_file_name);
        if (!file.open(QFile::ReadOnly)) return;
        new_file.open(QFile::WriteOnly);
        QDataStream in(&file), out(&new_file);
        qint64 to_modify, modified;
        while (!in.atEnd()) {
            in >> to_modify;
            modified = operations[ui->operation->currentText()](to_modify, ui->bitMask->text().toULongLong());
            out << modified;
        }
        file.close();
        new_file.close();
        if (ui->deleteInputFiles->isChecked()) file.remove();
    }
}

void Test::on_buttonBox_accepted() {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    if (ui->onTimer->isChecked()) {
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(Modify()));
        int t = ui->timer->text().toInt() * 1000;
        timer->start(t);
    }
    else {
        Modify();
        close();
    }
}

void Test::on_rewrite_clicked() {
    if (ui->rewrite->isChecked()) ui->createCopy->setEnabled(false);
    else ui->createCopy->setEnabled(true);
}

void Test::on_createCopy_clicked() {
    if (ui->createCopy->isChecked()) ui->rewrite->setEnabled(false);
    else ui->rewrite->setEnabled(true);
}

void Test::on_onTimer_clicked() {
    if (ui->onTimer->isChecked()) {
        ui->once->setEnabled(false);
        ui->timer->setEnabled(true);
    }
    else {
        ui->once->setEnabled(true);
        ui->timer->clear();
        ui->timer->setEnabled(false);
    }
}

void Test::on_once_clicked() {
    if (ui->once->isChecked()) ui->onTimer->setEnabled(false);
    else ui->onTimer->setEnabled(true);
}
