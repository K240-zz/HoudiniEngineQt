#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "HAPI_cpp.h"
#include <string>
#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

namespace hapi {

class ParameterWidget;
//
//
//
class ParameterValue : public QWidget
{
    Q_OBJECT
public:
    explicit ParameterValue( ParameterWidget* owner, QWidget *parent = 0 ) : QWidget(parent), mOwner(owner) {}

    virtual int getIntValue( int index )  const { Q_UNUSED (index); return 0; }
    virtual float getFloatValue( int index )  const { Q_UNUSED (index); return 0.f; }
    virtual bool getBoolValue( int index )  const { Q_UNUSED (index); return false; }
    virtual std::string getStringValue( int index )  const {Q_UNUSED (index); return std::string(); }
    virtual int size() const { return 0; }

    virtual void create() {}
    virtual void sync() {}

    ParameterWidget* owner() const { return mOwner; }

signals:
    void    valueUpdated();
private:
    double      mMin;
    double      mMax;
    ParameterWidget*    mOwner;
};

//
//
//
class ParameterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParameterWidget( const Parm& parm, QWidget *parent = 0 );
signals:
    void    valueUpdated(ParameterWidget*);
public slots:
    void    editorChanged();

    const Parm& parm() { return mParm; }

private:
    Parm                mParm;
    QString             mName;
    ParameterValue*     mWidget;
};

//
//
//
class ParameterInt : public ParameterValue
{
    Q_OBJECT
public:
    explicit ParameterInt( ParameterWidget* owner, QWidget *parent = 0 );

    virtual int getIntValue( int index )  const;
    virtual int size()  const { return mEditor.size(); }
    virtual void create();
    virtual void sync();

public slots:
    void    valueEdited(int);
    void    currentIndexChanged(int);
private:
    QList<QWidget*>    mEditor;
};

//
//
//
class ParameterFloat : public ParameterValue
{
    Q_OBJECT
public:
    explicit ParameterFloat( ParameterWidget* owner, QWidget *parent = 0 );

    virtual float getFloatValue( int index )  const;
    virtual int size()  const { return mEditor.size(); }
    virtual void create();
    virtual void sync();

public slots:
    void    valueEdited(double);
private:
    QList<QWidget*>    mEditor;
};

//
//
//
class ParameterBool : public ParameterValue
{
    Q_OBJECT
public:
    explicit ParameterBool( ParameterWidget* owner, QWidget *parent = 0 );

    virtual bool getBoolValue( int index )  const;
    virtual int size()  const { return mEditor.size(); }
    virtual void create();
    virtual void sync();

public slots:
    void    valueEdited(bool);
private:
    QList<QWidget*>    mEditor;
};

//
//
//
class ParameterString : public ParameterValue
{
    Q_OBJECT
public:
    explicit ParameterString( ParameterWidget* owner, QWidget *parent = 0 );

    virtual std::string getStringValue( int index )  const;
    virtual int size()  const { return mEditor.size(); }
    virtual void create();
    virtual void sync();

public slots:
    void    valueEdited();
    void    currentIndexChanged(int);
private:
    QList<QWidget*>    mEditor;
};

//
//
//
class ParameterButton : public ParameterValue
{
    Q_OBJECT
public:
    explicit ParameterButton( ParameterWidget* owner, QWidget *parent = 0 );

    virtual int size()  const { return mEditor.size(); }
    virtual void create();
    virtual void sync();

public slots:
    void    valueEdited();
private:
    QList<QWidget*>    mEditor;
};

//
//
//
class ParameterFile : public ParameterValue
{
    Q_OBJECT
public:
    explicit ParameterFile( ParameterWidget* owner, QWidget *parent = 0 );

    virtual std::string getStringValue( int index )  const;
    virtual int size()  const { return mEditor.size(); }
    virtual void create();
    virtual void sync();

public slots:
    void    valueEdited();
private:
    QList<QWidget*>    mEditor;
};


}

#endif // PARAMETERS_H
