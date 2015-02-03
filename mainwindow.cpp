#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

using namespace hapi;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mParameterView(nullptr)
{
    ui->setupUi(this);


    Engine* hapi = Engine::getInstance();

    hapi->initialize(nullptr, nullptr, true, -1);

    currentAssetId = -1;

    mParameterView = new ParametersView(this);
    setCentralWidget( mParameterView );

    connect( ui->actionOpen, SIGNAL(triggered()), this, SLOT(openAsset()) );
}

MainWindow::~MainWindow()
{
    HAPI_DestroyAsset( currentAssetId );

    if (mParameterView) delete mParameterView;
    delete ui;

    Engine* hapi = Engine::getInstance();
    hapi->cleanup();
}


void MainWindow::openAsset()
{

    QString filename = QFileDialog::getOpenFileName(this,
         tr("Open Digital Asset"), "", tr("Digital Asset (*.otl *.hda)"));

    if ( filename.size() )
    {
        Engine* hapi = Engine::getInstance();
        HAPI_DestroyAsset( currentAssetId );
        int library_id = hapi->loadAssetLibrary( filename.toStdString().c_str() ) ;
        if ( library_id >= 0 )
        {
            std::string name = hapi->getAssetName( library_id, 0);
            int asset_id = hapi->instantiateAsset( name.c_str(), true );
            if ( asset_id >= 0 )
            {
                currentAssetId = asset_id;
                mParameterView->setAsset( asset_id );
            }
        }
    }
}

