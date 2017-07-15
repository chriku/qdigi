#include "subitemblock.h"
#include "settings.h"

SubitemBlock::SubitemBlock(Schematic *data, QObject *parent) : Block(parent)
{
    subItem=true;
    m_data=data;
    name=data->name;
    valid=true;
    QList<QString> inputs=m_data->getInputs();
    QList<QString> outputs=m_data->getOutputs();
    for(int i=0;i<inputs.length();i++)
    {
        pin_t pin;
        pin.direction=0;
        pin.m_point=QPoint(0,i+1);
        pin.type=false;
        pins.append(pin);
        ppos.append(QPair<QString,bool>(inputs[i],true));
    }
    for(int i=0;i<outputs.length();i++)
    {
        pin_t pin;
        pin.direction=2;
        pin.m_point=QPoint(4,i+1);
        pin.type=false;
        pins.append(pin);
        ppos.append(QPair<QString,bool>(outputs[i],false));
    }
    width=3;
    height=fmax(inputs.length(),outputs.length());
}
QPicture SubitemBlock::draw()
{
    QPicture ret;
    QPainter painter(&ret);
    ret.setBoundingRect(QRect(0,0,(width * Settings::final()->gridSize()) +
                              Settings::final()->gridSize() + 1,
                              (height * Settings::final()->gridSize()) + 1 +
                              Settings::final()->gridSize()));
    if(!valid)
        return ret;
    painter.setPen(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    QColor colorp(color);
    pen=QPen(colorp);
    pen.setWidth(Settings::final()->penWidth()*Settings::final()->gridSize());
    painter.setPen(pen);
    painter.setBrush(Qt::white);
    painter.drawRect(QRectF(0.5*Settings::final()->gridSize(),0.5*Settings::final()->gridSize(),width*Settings::final()->gridSize(),height*Settings::final()->gridSize()));
    QFont font;
    font.setPixelSize(Settings::final()->gridSize()*0.75);
    painter.setFont(font);
    painter.drawText(QRectF(0.5*Settings::final()->gridSize(),0.5*Settings::final()->gridSize(),width*Settings::final()->gridSize(),height*Settings::final()->gridSize()),Qt::AlignCenter,m_data->name);
    for(int i=0;i<pins.length();i++)
    {
        double x;
        double y=0;
        if(pins[i].direction==0)
        {
            x=0.5;
            y=pins[i].m_point.y()-0.5;
        }else
        {
            x=2;
            y=pins[i].m_point.y()-0.5;
        }
        painter.setPen(Qt::black);
        painter.drawText(QRectF(x*Settings::final()->gridSize(),y*Settings::final()->gridSize(),1.5*Settings::final()->gridSize(),1*Settings::final()->gridSize()),Qt::AlignCenter,ppos[i].first);
    }
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(Qt::black);
    painter.drawPicture(0,0,Block::draw());
    return ret;
}

void SubitemBlock::keyPress(int pos)
{

}

void SubitemBlock::keyPressNorm(QString key)
{

}

void SubitemBlock::onclick(QPointF where)
{

}

void SubitemBlock::onpress(QPointF where)
{

}

void SubitemBlock::onrelease(QPointF where)
{

}

void SubitemBlock::execContext(int idx)
{
}

bool SubitemBlock::getState(int pin)
{
    QString name;
    for(auto p:ppos)
        name=p.first;
    for(int i=0;i<pins.length();i++)
        if(pins[i].direction==2)
        {
            QString name=ppos[i].first;
            int blkIdx=-1;
            for(auto input:m_data->getOutputList())
                if(input.first==name)
                    blkIdx=input.second;
            if(blkIdx>=0)
            {
                return m_data->blocks[blkIdx]->pins[0].state;
            }
        }
    return false;
}

Block* SubitemBlock::clone()
{
    return new SubitemBlock(m_data);
}

void SubitemBlock::simulate()
{
    for(int i=0;i<pins.length();i++)
        if(pins[i].direction==0)
        {
            QString name=ppos[i].first;
            int blkIdx=-1;
            for(auto input:m_data->getInputList())
                if(input.first==name)
                    blkIdx=input.second;
            if(blkIdx>=0)
            {
                m_data->blocks[blkIdx]->useFake=true;
                m_data->blocks[blkIdx]->fake.clear();
                m_data->blocks[blkIdx]->fake.append(pins[i].state);
            }
        }
    m_data->simulate();
}
