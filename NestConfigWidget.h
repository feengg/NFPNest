#ifndef NESTCONFIG_H
#define NESTCONFIG_H

#include <QDialog>

namespace Ui {
class NestConfigWidget;
}

class NestConfigWidget : public QDialog
{
    Q_OBJECT

public:
    explicit NestConfigWidget(QWidget *parent = nullptr);
    ~NestConfigWidget();

private slots:
    void on_comboBox_stripSize_currentTextChanged(const QString &arg1);
    void on_lineEdit_stripWidth_editingFinished();
    void on_lineEdit_stripHeight_editingFinished();
    void on_pushButton_apply_clicked();
    void on_pushButton_cancle_clicked();

private:
    Ui::NestConfigWidget *ui;

    void apply();
};

#endif // NESTCONFIG_H
