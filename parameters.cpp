#include "parameters.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include "fileselector.h"

#define MIN_WIDGET_WIDTH        (100)
#define MAX_WIDGET_WIDTH        (120)

namespace hapi {

ParameterWidget::ParameterWidget( const Parm& parm, QWidget *parent ) : QWidget(parent)
{
    mWidget = nullptr;
    mParm = parm;
    QVBoxLayout * l = new QVBoxLayout( );
    l->setMargin(0);
    setLayout( l );

    switch ( mParm.info().type )
    {
    case HAPI_PARMTYPE_INT:
        mWidget = new ParameterInt( this, this );
        break;
    case HAPI_PARMTYPE_TOGGLE :
        mWidget = new ParameterBool( this, this );
        break;
    case HAPI_PARMTYPE_FLOAT :
        mWidget = new ParameterFloat( this, this );
        break;
    case HAPI_PARMTYPE_BUTTON:
        mWidget = new ParameterButton( this, this );
        break;
    case HAPI_PARMTYPE_STRING :
    case HAPI_PARMTYPE_PATH_NODE :
        mWidget = new ParameterString( this, this );
        break;
    case HAPI_PARMTYPE_PATH_FILE :
    case HAPI_PARMTYPE_PATH_FILE_GEO :
        mWidget = new ParameterFile( this, this );
        break;
    }

    if ( mWidget )
    {
        layout()->addWidget( mWidget );
        connect( mWidget, SIGNAL(valueUpdated()), this, SLOT(editorChanged()));
    }

}

void ParameterWidget::editorChanged()
{
}

//
//
//
ParameterInt::ParameterInt( ParameterWidget* owner, QWidget *parent  ) : ParameterValue(owner, parent)
{
    create();
}

int ParameterInt::getIntValue( int index )  const
{
    if ( mEditor[index]->objectName() == "choice" )
    {
        return (int) dynamic_cast< QComboBox* >(mEditor[index])->currentIndex();
    }
    else
    {
        return (int) dynamic_cast< QSpinBox* >(mEditor[index])->value();
    }
}

void ParameterInt::create()
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setMargin(0);
    l->setAlignment( Qt::AlignLeft|Qt::AlignCenter);
    setLayout(l);

    const Parm& p = owner()->parm();
    int count = p.info().size;

    if ( p.choices.size() )
    {
        for ( int i = 0; i < count; ++i )
        {
            QComboBox* cb = new QComboBox(this);
            cb->setObjectName( "choice");
            for ( int j = 0; j < p.choices.size(); ++j )
            {
                cb->addItem( QString( p.choices[j].label().c_str() ) );
            }
            cb->setCurrentIndex( p.getIntValue(i) );
            l->addWidget(cb);
            connect( cb, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)) );
            mEditor.append( cb );
        }
    }
    else
    {
        for ( int i = 0; i < count; ++i )
        {
            QSpinBox* spinr = new QSpinBox(this);
            spinr->setValue( p.getIntValue(i) );
            double imin = p.info().hasMin ? p.info().min : -99999999;
            double imax = p.info().hasMax ? p.info().max : 99999999;
            spinr->setRange( imin, imax );
            spinr->setMinimumWidth(MIN_WIDGET_WIDTH);
            //spinr->setMaximumWidth(MAX_WIDGET_WIDTH);
            l->addWidget(spinr);
            connect( spinr, SIGNAL(valueChanged(int)), this, SLOT(valueEdited(int)) );
            mEditor.append( spinr );
        }
        if ( count == 1 )
            l->addStretch();
    }
}

void ParameterInt::valueEdited( int value )
{
    Q_UNUSED (value);
    sync();
}

void ParameterInt::currentIndexChanged( int value )
{
    Q_UNUSED (value);
    sync();
}

void ParameterInt::sync()
{
    bool emitting = false;
    Parm p = owner()->parm();
    int count = size();
    for ( int i =0 ; i < count; ++i )
    {
        int val = this->getIntValue(i);
        if ( val != p.getIntValue(i) )
        {
            p.setIntValue( i, val );
            emitting = true;
        }
    }
    if ( emitting )
        emit valueUpdated();
}

//
//
//
ParameterFloat::ParameterFloat( ParameterWidget* owner, QWidget *parent  ) : ParameterValue(owner, parent)
{
    create();
}

float ParameterFloat::getFloatValue( int index )  const
{
    return (float) dynamic_cast< QDoubleSpinBox* >(mEditor[index])->value();
}

void ParameterFloat::create()
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setMargin(0);
    //l->setAlignment( Qt::AlignLeft|Qt::AlignCenter);
    setLayout(l);

    const Parm& p = owner()->parm();
    int count = p.info().size;

    for ( int i = 0; i < count; ++i )
    {
        QDoubleSpinBox* spinr = new QDoubleSpinBox(this);
        double imin = p.info().hasMin ? p.info().min : -99999999;
        double imax = p.info().hasMin ? p.info().max : 99999999;
        spinr->setRange( imin, imax );
        spinr->setValue( p.getFloatValue(i) );
        spinr->setMinimumWidth(MIN_WIDGET_WIDTH);
        //spinr->setMaximumWidth(MAX_WIDGET_WIDTH);
        l->addWidget(spinr);
        connect( spinr, SIGNAL(valueChanged(double)), this, SLOT(valueEdited(double)) );
        mEditor.append( spinr );
    }
    if ( count == 1 )
        l->addStretch();
}

void ParameterFloat::valueEdited( double value )
{
    Q_UNUSED (value);
    sync();
}

void ParameterFloat::sync()
{
    bool emitting = false;
    Parm p = owner()->parm();
    int count = size();
    for ( int i =0 ; i < count; ++i )
    {
        float val = this->getFloatValue(i);
        if ( val != p.getFloatValue(i) )
        {
            p.setFloatValue( i, val );
            emitting = true;
        }
    }
    if ( emitting )
        emit valueUpdated();
}

//
//
//
ParameterBool::ParameterBool( ParameterWidget* owner, QWidget *parent  ) : ParameterValue(owner, parent)
{
    create();
}

bool ParameterBool::getBoolValue( int index )  const
{
    if ( mEditor[index]->objectName() == "choise" )
    {
        return 0.f;
    }
    else
    {
        return dynamic_cast<QCheckBox*>(mEditor[index])->isChecked();
    }
}

void ParameterBool::create()
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setMargin(0);
    setLayout(l);

    const Parm& p = owner()->parm();
    int count = p.info().size;

    for ( int i = 0; i < count; ++i )
    {
        QCheckBox* cb = new QCheckBox(this);
        if ( i == 0 )
            cb->setText( p.label().c_str() );
        cb->setCheckable(true);
        cb->setChecked( p.getIntValue(i) != 0 );
        l->addWidget(cb);
        connect( cb, SIGNAL(clicked(bool)), this, SLOT(valueEdited(bool)) );
        mEditor.append( cb );
    }
}

void ParameterBool::valueEdited( bool value )
{
    Q_UNUSED (value);
    sync();
}

void ParameterBool::sync()
{
    bool emitting = false;
    Parm p = owner()->parm();
    int count = size();
    for ( int i =0 ; i < count; ++i )
    {
        bool val = this->getBoolValue(i);
        if ( val != (p.getIntValue(i) != 0) )
        {
            p.setIntValue( i, val ? 1 : 0 );
            emitting = true;
        }
    }
    if ( emitting )
        emit valueUpdated();
}

//
//
//
ParameterString::ParameterString( ParameterWidget* owner, QWidget *parent  ) : ParameterValue(owner, parent)
{
    create();
}

std::string ParameterString::getStringValue( int index )  const
{

    if ( mEditor[index]->objectName() == "choice" )
    {
        const Parm& p = owner()->parm();
        std::string label = std::string( dynamic_cast< QComboBox* >(mEditor[index])->currentText().toLatin1().constData() );
        for ( int i = 0; i < p.choices.size(); ++i )
        {
            if ( p.choices[i].label() == label )
            {
                return p.choices[i].value();
            }
        }
        // just in case
        return std::string("");

    }
    else
    {
        return std::string( dynamic_cast< QLineEdit* >(mEditor[index])->text().toLatin1().constData() ) ;
    }

}

void ParameterString::create()
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setMargin(0);
    setLayout(l);

    const Parm& p = owner()->parm();
    int count = p.info().size;

    if ( p.choices.size() )
    {
        for ( int i = 0; i < count; ++i )
        {
            QComboBox* cb = new QComboBox(this);
            cb->setObjectName( "choice");
            std::string current = p.getStringValue(i).c_str();

            for ( int j = 0; j < p.choices.size(); ++j )
            {
                cb->addItem( QString( p.choices[j].label().c_str() ) );
                if ( current ==  p.choices[j].value() )
                {
                    // set current index
                    cb->setCurrentIndex( j );
                }
            }
            l->addWidget(cb);
            connect( cb, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)) );
            mEditor.append( cb );
        }
        l->addStretch();
    }
    else
    {
        for ( int i = 0; i < count; ++i )
        {
            QLineEdit* ed = new QLineEdit(this);
            ed->setText( p.getStringValue(i).c_str() );
            l->addWidget(ed);
            connect( ed, SIGNAL(editingFinished()), this, SLOT(valueEdited()) );
            mEditor.append( ed );
        }
    }
}

void ParameterString::valueEdited()
{
    sync();
}

void ParameterString::currentIndexChanged( int value )
{
    Q_UNUSED (value);
    sync();
}

void ParameterString::sync()
{
    bool emitting = false;
    Parm p = owner()->parm();
    int count = size();
    for ( int i =0 ; i < count; ++i )
    {
        std::string val = this->getStringValue(i);
        if ( val != p.getStringValue(i) )
        {
            p.setStringValue( i, val.c_str() );
            emitting = true;
        }
    }
    if ( emitting )
        emit valueUpdated();
}


//
//
//
ParameterButton::ParameterButton( ParameterWidget* owner, QWidget *parent  ) : ParameterValue(owner, parent)
{
    create();
}

void ParameterButton::create()
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setMargin(0);
    setLayout(l);

    const Parm& p = owner()->parm();
    int count = p.info().size;

    for ( int i = 0; i < count; ++i )
    {
        QPushButton* cb = new QPushButton(this);
        cb->setObjectName(  p.label().c_str()  );
        cb->setText( p.label().c_str() );
        l->addWidget(cb);
        connect( cb, SIGNAL(clicked()), this, SLOT(valueEdited()) );
        mEditor.append( cb );
    }
    l->addStretch();
}

void ParameterButton::valueEdited()
{
    sync();

    Parm p = owner()->parm();
    int count = p.info().size;

    if ( count )
    {
        p.setIntValue(0, 1);
    }

}

void ParameterButton::sync()
{
    emit valueUpdated();
}


//
//
//
ParameterFile::ParameterFile( ParameterWidget* owner, QWidget *parent  ) : ParameterValue(owner, parent)
{
    create();
}

std::string ParameterFile::getStringValue( int index )  const
{

    {
        return std::string( dynamic_cast< FileSelector* >(mEditor[index])->getFilename().toLatin1().constData() ) ;
    }

}

void ParameterFile::create()
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setMargin(0);
    setLayout(l);

    const Parm& p = owner()->parm();
    int count = p.info().size;
    for ( int i = 0; i < count; ++i )
    {
        FileSelector* ed = new FileSelector(this);
        ed->setFilename( p.getStringValue(i).c_str() );
        l->addWidget(ed);
        connect( ed, SIGNAL(editingFinished()), this, SLOT(valueEdited()) );
        mEditor.append( ed );
    }
}

void ParameterFile::valueEdited()
{
    sync();
}

void ParameterFile::sync()
{
    bool emitting = false;
    Parm p = owner()->parm();
    int count = size();
    for ( int i =0 ; i < count; ++i )
    {
        std::string val = this->getStringValue(i);
        if ( val != p.getStringValue(i) )
        {
            p.setStringValue( i, val.c_str() );
            emitting = true;
        }
    }
    if ( emitting )
        emit valueUpdated();
}



};
