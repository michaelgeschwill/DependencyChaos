#ifndef IDMODFORM_H
#define IDMODFORM_H

#include <QDialog>

namespace Ui {
class IdModForm;
}

class IdModForm : public QDialog
{
    Q_OBJECT

public:
    explicit IdModForm(QString defaultId, QWidget *parent = 0);
    ~IdModForm();
    QString getId();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_lineEdit_returnPressed();

private:
    Ui::IdModForm *ui;
};

#endif // IDMODFORM_H
