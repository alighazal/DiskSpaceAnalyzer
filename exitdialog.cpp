#include "exitdialog.h"
#include "ui_exitdialog.h"

ExitDialog::ExitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExitDialog)
{
    ui->setupUi(this);
}

ExitDialog::~ExitDialog()
{
    delete ui;
}

void ExitDialog::on_YesButton_clicked()
{
    QApplication::quit();
}

void ExitDialog::on_NoButton_clicked()
{
    close();
}
