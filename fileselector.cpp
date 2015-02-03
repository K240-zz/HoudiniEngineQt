#include "fileselector.h"
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>

namespace hapi
{

FileSelector::FileSelector(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setMargin(0);
    l->setSpacing(4);
    QPushButton* button = new QPushButton("...", this);
    button->setMinimumWidth(20);
    button->setMaximumWidth(20);
    mLineEdit = new QLineEdit(this);

    l->addWidget( mLineEdit );
    l->addWidget( button );

    connect( button, SIGNAL(clicked()), this, SLOT(openDialog()) );
    connect( mLineEdit, SIGNAL(editingFinished()), this, SLOT(textUpdated()) );

    setLayout(l);
}

void FileSelector::openDialog()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter( mFilters );
    dialog.selectFile( mFilename );

    if ( dialog.exec() )
    {
        QStringList fl = dialog.selectedFiles();

        if ( fl.size() == 1 )
        {
            setFilename( fl[0] );
        }
    }
}

void FileSelector::textUpdated()
{
    setFilename( mLineEdit->text() );
}

void FileSelector::setFilename( const char* filename )
{
    setFilename( QString(filename) );
}

void FileSelector::setFilename( const QString& filename )
{
    if ( mFilename != filename || mLineEdit->text() != filename )
    {
        mFilename = filename;
        mLineEdit->setText( mFilename );
        emit editingFinished();
    }
}

void FileSelector::setFilter( const char* filter )
{
    setFilter( QString(filter) );
}

void FileSelector::setFilter( const QString& filter )
{
    mFilters = filter;
}

};
