#ifndef PARAMETERSVIEW_H
#define PARAMETERSVIEW_H


#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QVariant>
#include <QMap>
#include <QTabWidget>
#include "parameters.h"


namespace hapi {

class ParameterWidget;
class ParametersView : public QScrollArea
{
    Q_OBJECT
public:
    explicit ParametersView(QWidget *parent = 0);
    ~ParametersView();

    void    setAsset( int asset_id );
    void    clear();
signals:
    void    updateParameters( const QString& );
public slots:
    void    parameterEdited( ParameterWidget* );

private:
    Asset*              mAsset;
    QWidget*            mBase;
    QVBoxLayout*        mLayout;
    QMap<int, QTabWidget*>  mFolderList;
    QMap<int, QWidget*>     mFolders;
};

};

#endif // PARAMETERSVIEW_H
