#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QWidget>
#include <QLineEdit>

namespace hapi
{

class FileSelector : public QWidget
{
    Q_OBJECT
public:
    explicit FileSelector(QWidget *parent = 0);
    void setFilename( const char* filename );
    void setFilename( const QString& filename );
    void setFilter( const char* filter );               // Ex:"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
    void setFilter( const QString& filter );
    QString getFilename()  const { return mLineEdit->text(); }

signals:
    void editingFinished();
public slots:
    void    openDialog();
    void    textUpdated();

private:
    QLineEdit*      mLineEdit;
    QString         mFilename;
    QString         mFilters;
};


};

#endif // FILESELECTOR_H
