#include "mainwindow.h"
#include <QApplication>

void setStyle( QApplication* app, QWidget* mainwindow )
{
    app->setStyle( QString("plastique") );

    QPalette pal = app->palette();
    pal.setColor(QPalette::Window, QColor(0x32,0x32,0x32) );
    pal.setColor(QPalette::WindowText, QColor(0xc2,0xc2,0xc2));
    pal.setColor(QPalette::Base, QColor(0x1a,0x1a,0x1a) );
    pal.setColor(QPalette::AlternateBase, QColor(0x22,0x22,0x22) );

    pal.setColor(QPalette::ToolTipBase, QColor(0x22,0x22,0x22) );
    pal.setColor(QPalette::ToolTipText, QColor(0x22,0x22,0x22) );

    pal.setColor(QPalette::Light, QColor(0x52,0x52,0x52) );
    pal.setColor(QPalette::Midlight, QColor(0x42,0x42,0x42) );
    pal.setColor(QPalette::Dark, QColor(0x12,0x12,0x12) );
    pal.setColor(QPalette::Mid, QColor(0x22,0x22,0x22) );
    pal.setColor(QPalette::Shadow, QColor(0x12,0x12,0x12) );

    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::Button, QColor(0x42,0x42,0x42) );
    pal.setColor(QPalette::ButtonText, QColor(0xc2,0xc2,0xc2));
    pal.setColor(QPalette::BrightText, QColor(0xc2,0xc2,0xc2));

    app->setPalette( pal );

    mainwindow->setStyleSheet(
                "font-family: \"Verdana\";"
                "font-size: 11px;"
                );
}




int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow* mainWindow = new MainWindow();
    setStyle( &a, mainWindow );
    mainWindow->show();

    int result = a.exec();
    delete mainWindow;

    return result;
}
