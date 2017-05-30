#include "dependform.h"
#include "ui_dependform.h"
#include <QFileDialog>

DependForm::DependForm(QString dep, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DependForm)
{
    ui->setupUi(this);
    ui->lineEdit->setText(dep);
    ui->lineEdit->setFocus();
}

DependForm::~DependForm()
{
    delete ui;
}

QString DependForm::getDep()
{
    return ui->lineEdit->text();
}

void DependForm::on_buttonBox_accepted()
{
    if (ui->lineEdit->text().trimmed()!="")
    {
        this->done(QDialog::Accepted);
    }
}

void DependForm::on_buttonBox_rejected()
{
    this->done(QDialog::Rejected);
}

void DependForm::on_lineEdit_returnPressed()
{
    on_buttonBox_accepted();
}

void DependForm::on_searchbutton_clicked()
{
    QFileDialog dialog;
    auto result = dialog.exec();

    if (result == QDialog::Accepted){
        ui->lineEdit->setText(dialog.selectedFiles().first());
    }

    dialog.deleteLater();
}
