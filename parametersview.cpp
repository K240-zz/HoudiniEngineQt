#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolButton>
#include <QTabWidget>
#include "parametersview.h"
#include <QDebug>

namespace hapi {

#define LAYOUT_MARGIN       (2)


bool GetParmParentVisible( std::vector<hapi::Parm>& parms, int id )
{
    if ( id < 0 )
        return false;

    for (int i=0; i < int(parms.size()); ++i)
    {
        hapi::Parm &parm = parms[i];
        if ( parm.info().id == id )
        {
            if ( parm.info().invisible )
                return true;

            if ( parm.info().parentId >= 0 && GetParmParentVisible( parms, parm.info().parentId ) )
                return true;
        }
    }
    return false;
}

ParametersView::ParametersView(QWidget *parent) :
    QScrollArea(parent), mAsset(nullptr), mBase(nullptr)
{
}

ParametersView::~ParametersView()
{
    clear();
}

void ParametersView::setAsset( int asset_id )
{
    clear();

    mAsset = new Asset( asset_id );

    if ( mAsset->isValid() )
    {
        // Create Base Widget
        mBase = new QWidget(this);
        mLayout = new QVBoxLayout();
        mLayout->setMargin(LAYOUT_MARGIN);
        mLayout->setSpacing(0);
        mBase->setLayout(mLayout);
        mBase->setMinimumWidth(300);
        setWidget(mBase);
        setWidgetResizable(true);

        QVBoxLayout* l = dynamic_cast<QVBoxLayout*>(mLayout);

        std::vector<hapi::Parm> parms = mAsset->parms();
        for (int i=0; i < int(parms.size()); ++i)
        {
            hapi::Parm parm = parms[i];

            if ( parm.info().invisible || GetParmParentVisible( parms, parm.info().parentId ) )
                continue;


            std::string label = parm.label();
            std::string name  = parm.name();

            switch(parm.info().type)
            {
            case HAPI_PARMTYPE_SEPARATOR:
            {
                QVBoxLayout* layout = l;
                if ( mFolders.find(parm.info().parentId) != mFolders.end() )
                {
                     layout = dynamic_cast<QVBoxLayout*>( mFolders[parm.info().parentId]->layout() );
                }
                QFrame* line = new QFrame(mBase);
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                layout->addWidget( line );
                break;
            }
            case HAPI_PARMTYPE_FOLDERLIST:
            {
                if ( parm.info().size )
                {

                    QTabWidget* tab = new QTabWidget(mBase);

                    if ( mFolders.find(parm.info().parentId) == mFolders.end() )
                    {
                        l->addWidget( tab );
                    }
                    else
                    {
                        mFolders[parm.info().parentId]->layout()->addWidget( tab );
                    }
                    mFolderList[ parm.info().id ] = tab;

                    for ( int parm_idx = 0; parm_idx < parm.info().size; ++parm_idx )
                    {
                        hapi::Parm pp = parms[parm_idx+i+1];
                        if ( pp.info().type == HAPI_PARMTYPE_FOLDER &&
                             !pp.info().invisible )
                        {
                            QWidget* folder = new QWidget(mBase);
                            QVBoxLayout* lo = new QVBoxLayout();
                            lo->setSpacing(0);
                            lo->setMargin(LAYOUT_MARGIN);
                            lo->setAlignment(Qt::AlignTop);
                            folder->setLayout( lo );
                            mFolders[ pp.info().id ] = folder;
                            tab->addTab( folder, QString( pp.label().c_str() ) );
                        }
                    }
                }

                break;
            }
            case HAPI_PARMTYPE_FOLDER:
            {
                // pass
                break;
            }
            default:
            {
                QWidget* base = new QWidget(mBase);
                ParameterWidget* widget = new ParameterWidget( parm, base);

                if ( widget )
                {
                    QVBoxLayout* layout = l;
                    if ( mFolders.find(parm.info().parentId) != mFolders.end() )
                    {
                         layout = dynamic_cast<QVBoxLayout*>( mFolders[parm.info().parentId]->layout() );
                    }
                    connect( widget, SIGNAL(valueUpdated(ParameterWidget*)), this, SLOT(parameterEdited(ParameterWidget*)) );
                    QLabel* namewidget = new QLabel(QString(label.c_str()), base);

                    if ( parm.info().type == HAPI_PARMTYPE_TOGGLE ||
                         parm.info().type == HAPI_PARMTYPE_BUTTON )
                    {
                        namewidget->setText("");
                    }
                    namewidget->setToolTip( QString(name.c_str()) );
                    namewidget->setMinimumWidth(120);
                    namewidget->setMaximumWidth(120);
                    namewidget->setAlignment( Qt::AlignRight|Qt::AlignCenter );
                    namewidget->setScaledContents( true );
                    QHBoxLayout* lo = new QHBoxLayout();
                    lo->setSpacing(8);
                    lo->setMargin(LAYOUT_MARGIN);
                    lo->addWidget(namewidget);
                    lo->addWidget(widget);
                    base->setLayout(lo);
                    layout->addWidget(base);
                }
                else
                {
                    delete base;
                }
            }
            }
        }
        update();
    }
}

void ParametersView::clear()
{
    if ( mAsset )
    {
        delete mAsset;
        mAsset = nullptr;
    }

    if ( mBase )
    {
        delete mLayout;
        delete mBase;
        mBase = nullptr;
    }

    update();
}

void ParametersView::parameterEdited(ParameterWidget*)
{

}



};
