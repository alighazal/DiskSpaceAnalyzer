#ifndef EXITDIALOG_H
#define EXITDIALOG_H

#include <QDialog>

namespace Ui {
class ExitDialog;
}

class ExitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExitDialog(QWidget *parent = nullptr);
    ~ExitDialog();

private slots:
    void on_YesButton_clicked();

    void on_NoButton_clicked();

private:
    Ui::ExitDialog *ui;
};

#endif // EXITDIALOG_H
