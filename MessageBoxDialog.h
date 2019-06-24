#ifndef MESSAGEBOXDIALOG_H
#define MESSAGEBOXDIALOG_H

#include <QDialog>
#include <QMessageBox>
namespace Ui {
class MessageBoxDialog;
}

class MessageBoxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageBoxDialog(QWidget *parent = 0);
    ~MessageBoxDialog();
    void setLabel(const QString& str);
private:
    Ui::MessageBoxDialog *ui;
};

#endif // MESSAGEBOXDIALOG_H
