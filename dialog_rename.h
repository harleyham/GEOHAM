#ifndef DIALOG_RENAME_H
#define DIALOG_RENAME_H

#include <QDialog>

namespace Ui {
class Dialog_Rename;
}

class Dialog_Rename : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Rename(QWidget *parent = nullptr);
    ~Dialog_Rename();


    QString     Filename,Ext;

    void        Inicializa(QString &Name);


    void on_bt_Ok_clicked();
    void on_bt_Cancel_clicked();
    QString getName(void);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::Dialog_Rename *ui;
};

#endif // DIALOG_RENAME_H
