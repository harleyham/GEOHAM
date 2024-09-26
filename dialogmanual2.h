#ifndef DIALOGMANUAL2_H
#define DIALOGMANUAL2_H

#include <QDialog>

class QPdfWidget;

namespace Ui {
class DialogManual2;
}

class DialogManual2 : public QDialog
{
    Q_OBJECT

public:
    explicit DialogManual2(QWidget *parent = nullptr);
    ~DialogManual2();



private:
    Ui::DialogManual2 *ui;

};

#endif // DIALOGMANUAL2_H
