#include "mainwindow.h"
#include "uart.h"
#include "filtro.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->inicializaFiltro();
    //this->ajustarBotoes();
    this->ajustarJanelaPrincipal();
    this->ajustarGrafico();
    this->ajustarLabel();

    this->musica = new QMediaPlayer;
    this->musica->setMedia(QUrl::fromLocalFile("C:\\Users\\User\\Documents\\heart.mp3"));
    this->musica->setVolume(100);



    Uart * com = new Uart(250000,QString("COM8"),62);
    QThread * tUart = new QThread();

    Filtro * fil = new Filtro();
    QThread * tFiltro = new QThread();

    QObject::connect(tUart, SIGNAL(started()), com, SLOT(run()) );
    QObject::connect(com,SIGNAL(readReady(QVector<unsigned char>)),fil,SLOT(Filtrar(QVector<unsigned char>)));
    QObject::connect(fil,SIGNAL(FilterReady(QVector<float>,QVector<float>,QVector<float>)),this,SLOT(realtimeDataSlot(QVector<float>,QVector<float>,QVector<float>)));
    QObject::connect(fil,SIGNAL(BeatRate(QString)),this,SLOT(alteraBatimentos(QString)));

    fil->moveToThread(tFiltro);
    com->moveToThread(tUart);

    tUart->start(QThread::TimeCriticalPriority);
    tFiltro->start(QThread::HighPriority);



}

MainWindow::~MainWindow()
{
    if (this->uart->isOpen())
        this->uart->close();
}

void MainWindow::ajustarLabel()
{
    QFont f( "Arial", 20, QFont::Bold);
    QPalette palette;

    palette.setColor(QPalette::WindowText, Qt::red);

    this->batimentos = new QLabel(this);
    this->batimentos->setText("Batimentos");
    this->batimentos->setGeometry(QRect(50,500,150,100));
    this->batimentos->setPalette(palette);
    this->batimentos->setFont(f);

}

void MainWindow::inicializaFiltro(void)
{
    return;

    QString path = (QFileDialog::getOpenFileName(this, "Selecione o arquivo com os coeficientes do filtro", QDir::homePath()));
    QFile file(path);

    unsigned char i,j;

    qDebug() << path;
    if(file.open(QIODevice::ReadOnly))
        qDebug() << "Arquivo do filtro encontrado: " << path;
    QTextStream in(&file);

    while(!in.atEnd())
    {
        this->ordem_filtro++;
        this->filtro << in.readLine().toFloat();
    }
    this->ordem_filtro--;

    qDebug() << "Leitura dos coeficientes do filtro realizada com sucesso: " << this->filtro;
    qDebug() << "Ordem do filtro: " << this->ordem_filtro;

    file.close();

    for(i=0;i<=this->ordem_filtro;i++)
        for(j=0;j<3;j++)
            this->sinaisNaoFiltrados[j] << 0;

}

void MainWindow::ajustarJanelaPrincipal(void)
{
    this->setGeometry(QRect(300,100,800,600));
    this->setFixedSize(800,600);
    this->setWindowTitle(tr("Eletrocardiograma"));
}

void MainWindow::ajustarBotoes(void)
{
    this->botaoPlayStop = new QPushButton("Play",this);
    this->botaoPlayStop->setGeometry(QRect(600,500,80,40));

    this->botaoConectar = new QPushButton("Conectar",this);
    this->botaoConectar->setGeometry(QRect(600,540,80,40));

    this->editorTexto = new QLineEdit(this);
    this->editorTexto->setPlaceholderText("Porta Serial");
    this->editorTexto->move(690,540);

    // Conectando Sinais //
    connect(this->botaoPlayStop,SIGNAL(clicked()),this,SLOT(playStopHandle()));
    //connect(this->botaoConectar,SIGNAL(clicked()),this,SLOT(conectarUART()));
}

void MainWindow::ajustarGrafico(void)
{
    unsigned char i;

    for (i=0;i<3;i++)
    {

        this->grafico[i] = new QCustomPlot(this);
        this->grafico[i]->setGeometry(QRect(50,50+i*150,700,150));

        this->grafico[i]->axisRect()->setupFullAxesBox();
        this->grafico[i]->yAxis->setRange(-2.5, 2.5);
        this->grafico[i]->yAxis->setTickLabelColor(Qt::white);
        this->grafico[i]->yAxis->setLabel("Tensão [mV]");
        this->grafico[i]->yAxis->setLabelColor(Qt::white);
        this->grafico[i]->yAxis->setAutoTickStep(false);
        this->grafico[i]->yAxis->setTickStep(0.5);



        //this->grafico[i]->xAxis->setTickLabelColor(Qt::white);
        //this->grafico[i]->xAxis->setLabel("25 mm/s");
        //this->grafico[i]->xAxis->setLabelColor(Qt::white);

        this->grafico[i]->xAxis2->setLabel(tr("Derivação %1").arg(QString('1'+i)));
        this->grafico[i]->xAxis2->setLabelColor(Qt::white);

        this->grafico[i]->setBackground(Qt::black);

        this->grafico[i]->xAxis->setAutoTickStep(false);
        this->grafico[i]->xAxis->grid()->setSubGridVisible(true);
        this->grafico[i]->xAxis->setTickStep(500);
        this->grafico[i]->xAxis->setSubTickCount(5);


        this->grafico[i]->addGraph();
        this->grafico[i]->graph(0)->setPen(QPen(Qt::green));

        this->grafico[i]->addGraph();
        this->grafico[i]->graph(1)->setPen(QPen(Qt::red));
        this->grafico[i]->graph(1)->setLineStyle(QCPGraph::lsNone);
        this->grafico[i]->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    }
}

//-------SLOTS-------//

void MainWindow::playStopHandle(void)
{
    this->playStop = ! this->playStop;
    if (this->playStop == true)
        this->botaoPlayStop->setText(tr("Stop"));
    else
        this->botaoPlayStop->setText(tr("Play"));

    qDebug() << "Botão pressionado! Novo estado: " << this->botaoPlayStop->text();
}

void MainWindow::alteraBatimentos(QString valor)
{
    this->batimentos->setText(valor);
    this->musica->play();
}

void MainWindow::rxUART(void)
{
    return;
}

void MainWindow::conectarUART(void)
{
    return;
}

void MainWindow::timeoutEvent(void)
{
    return;
}
void MainWindow::realtimeDataSlot(QVector<float> sinal_1, QVector<float> sinal_2, QVector<float> sinal_3)
{
    static double indice[3];
    unsigned char i,j;


    for (i=0;i<sinal_1.length();i++)
    {
        this->grafico[0]->graph(0)->removeData(indice[0],indice[0]+50);
        this->grafico[0]->graph(0)->addData((double) indice[0],sinal_1.data()[i]);
        this->grafico[0]->graph(1)->clearData();
        this->grafico[0]->graph(1)->addData((double)indice[0], (double)sinal_1.data()[i]);

        indice[0]++;

        if (indice[0] == 3723)
        {
            indice[0] = 0;
        }
    }

    this->grafico[0]->xAxis->setRange(0,3723);
    this->grafico[0]->replot();

    for (i=0;i<sinal_2.length();i++)
    {
        this->grafico[1]->graph(0)->removeData(indice[1],indice[1]+50);
        this->grafico[1]->graph(0)->addData((double) indice[1],sinal_2.data()[i]);
        this->grafico[1]->graph(1)->clearData();
        this->grafico[1]->graph(1)->addData((double)indice[1], (double)sinal_2.data()[i]);

        indice[1]++;

        if (indice[1] == 3723)
        {
            indice[1] = 0;
        }
    }

    this->grafico[1]->xAxis->setRange(0,3723);
    this->grafico[1]->replot();

    for (i=0;i<sinal_3.length();i++)
    {
        this->grafico[2]->graph(0)->removeData(indice[2],indice[2]+50);
        this->grafico[2]->graph(0)->addData((double) indice[2],sinal_3.data()[i]);
        this->grafico[2]->graph(1)->clearData();
        this->grafico[2]->graph(1)->addData((double)indice[2], (double)sinal_3.data()[i]);

        indice[2]++;

        if (indice[2] == 3723)
        {
            indice[2] = 0;
        }
    }

    this->grafico[2]->xAxis->setRange(0,3723);
    this->grafico[2]->replot();

}

