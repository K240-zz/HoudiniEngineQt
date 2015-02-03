#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parametersview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void    openAsset();
private:
    Ui::MainWindow *ui;
    hapi::ParametersView* mParameterView;
    int     currentAssetId;
};

#endif // MAINWINDOW_H
