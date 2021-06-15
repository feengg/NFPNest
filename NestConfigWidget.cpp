#include "NestConfigWidget.h"
#include "ui_NestConfigWidget.h"

#include "nest/LB_NestConfig.h"
using namespace NestConfig;

NestConfigWidget::NestConfigWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NestConfigWidget)
{
    ui->setupUi(this);
}

NestConfigWidget::~NestConfigWidget()
{
    delete ui;
}

void NestConfigWidget::on_comboBox_stripSize_currentTextChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    QStringList size = arg1.split('x');
    QString wid = size[0];
    QString hei = size[1];
    ui->lineEdit_stripWidth->setText(QString::number(wid.toDouble()));
    ui->lineEdit_stripHeight->setText(QString::number(hei.toDouble()));
}

void NestConfigWidget::on_lineEdit_stripWidth_editingFinished()
{
    if(ui->comboBox_stripSize->currentText().isEmpty()) {
        return;
    }

    double val = ui->lineEdit_stripWidth->text().toDouble();
    QStringList size = ui->comboBox_stripSize->currentText().split('x');
    if(!qFuzzyCompare(val,size[0].toDouble())) {
        ui->comboBox_stripSize->setCurrentIndex(-1);
    }
}

void NestConfigWidget::on_lineEdit_stripHeight_editingFinished()
{
    if(ui->comboBox_stripSize->currentText().isEmpty()) {
        return;
    }

    double val = ui->lineEdit_stripHeight->text().toDouble();
    QStringList size = ui->comboBox_stripSize->currentText().split('x');
    if(!qFuzzyCompare(val,size[1].toDouble())) {
        ui->comboBox_stripSize->setCurrentIndex(-1);
    }
}

void NestConfigWidget::on_pushButton_apply_clicked()
{    
    apply();
    this->accept();
}

void NestConfigWidget::on_pushButton_cancle_clicked()
{
    this->reject();
}

void NestConfigWidget::apply()
{
    STRIP_WIDTH = ui->lineEdit_stripWidth->text().toDouble();
    STRIP_HEIGHT = ui->lineEdit_stripHeight->text().toDouble();
    ENABLE_ROTATION = 1-ui->comboBox_enableRotation->currentIndex();
    ITEM_GAP = ui->lineEdit_itemGap->text().toDouble();
}
