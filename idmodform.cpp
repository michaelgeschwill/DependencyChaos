#include "idmodform.h"
#include "ui_idmodform.h"

IdModForm::IdModForm(QString defaultId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IdModForm)
{
    ui->setupUi(this);
    ui->lineEdit->setText(defaultId);
    ui->lineEdit->setFocus();
}

IdModForm::~IdModForm()
{
    delete ui;
}

QString IdModForm::getId()
{
    return ui->lineEdit->text();
}

void IdModForm::on_buttonBox_accepted()
{
    if (ui->lineEdit->text().trimmed()!="")
    {
        this->done(QDialog::Accepted);
    }
}

void IdModForm::on_buttonBox_rejected()
{
    this->done(QDialog::Rejected);
}

void IdModForm::on_lineEdit_returnPressed()
{
    on_buttonBox_accepted();
}
