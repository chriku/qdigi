#include "module.h"
#include "blockmodule.h"

Module::Module(QObject *parent) : QObject(parent)
{

}

Module* Module::getModule(QString name)
{
    if(name=="block")
        return new BlockModule;
    return NULL;
}
