#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QtSerialPort/QtSerialPort>
#include <QDebug>
#include <QTextStream>
#include <QTime>
#include <QMutex>
#include <QString>
#include <QMediaPlayer>
#include "qcustomplot.h"

#define tam_buffer 5
#define qtd_amostras 20


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void playStopHandle(void);
    void conectarUART(void);
    void rxUART(void);
    void timeoutEvent(void);
    void realtimeDataSlot(QVector<float> sinal_1, QVector<float> sinal_2, QVector<float> sinal_3);
    void alteraBatimentos(QString valor);

private:

    bool playStop = false;


    QPushButton * botaoPlayStop;
    QLineEdit   * editorTexto;
    QPushButton * botaoConectar;
    QCustomPlot * grafico[3];
    QSerialPort * uart;
    QLabel      * batimentos;
    QMediaPlayer * musica;

    unsigned char ordem_filtro = 0;

    QVector <char> rx;

    QTimer * timeoutRx;

    QTimer * timer;

    QVector <float> filtro;
    void inicializaFiltro(void);
    void filtrarSinais(void);

    QVector <float> sinaisFiltrados[3];
    QVector <signed short> sinaisNaoFiltrados[3];


    void ajustarUART(void);
    void ajustarGrafico(void);
    void ajustarBotoes(void);
    void ajustarJanelaPrincipal(void);
    void ajustarLabel(void);

};

#endif // MAINWINDOW_H
