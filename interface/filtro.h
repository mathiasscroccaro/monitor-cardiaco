#ifndef FILTRO_H
#define FILTRO_H
#include <QDebug>
#include <QVector>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QElapsedTimer>

class Filtro : public QObject
{
    Q_OBJECT

public:
    Filtro();

private:
    QVector <float> filtro;

    unsigned short ordem_filtro;

    //DEBUG PARA TESTES DA PLACA
    QFile * arquivo;

signals:
    void FilterReady(const QVector<float> &,const QVector<float> &,const QVector<float> &);
    void BeatRate(const QString &);

public slots:
    void Filtrar(QVector<unsigned char> rx);
};

#endif // FILTRO_H
