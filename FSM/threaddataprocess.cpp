#include "threaddataprocess.h"

threadDataProcess::threadDataProcess(int handler, QObject *parent):
     QThread(parent)
{
    this->handler=handler;
}
