#include "cutdata.h"

cutData::cutData(QObject *parent) :
    QThread(parent)
{
    this->socketRead=new socket;
}
