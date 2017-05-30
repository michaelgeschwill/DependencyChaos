#ifndef DEPENDFORM_H
#define DEPENDFORM_H

#include <QDialog>

namespace Ui {
class DependForm;
}

class DependForm : public QDialog
{
    Q_OBJECT

public:
    explicit DependForm(QString dep, QWidget *parent = 0);
    ~DependForm();
    QString getDep();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_lineEdit_returnPressed();

    void on_searchbutton_clicked();

private:
    Ui::DependForm *ui;
};

#endif // DEPENDFORM_H
