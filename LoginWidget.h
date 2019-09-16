#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 性别
     */
    enum Gender{
        Male, ///< 男
        Female ///< 女
    };

    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
    QString getMatchingNum() const;
    QString getExpNum() const;
    QString getAge() const;
    QString getShortName() const;
    Gender getGender() const;
signals:
    void ok();
private slots:
    void on_pushButtonOK_clicked();
private:
    Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
