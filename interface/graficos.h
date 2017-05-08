#ifndef GRAFICOS_H
#define GRAFICOS_H

#include <QObject>
#include "qcustomplot.h"
#include <QDebug>

class Graficos : public QObject
{
    Q_OBJECT
public:
    Graficos();
public slots:
    atualiza();
};

#endif // GRAFICOS_H
