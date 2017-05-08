#include "filtro.h"

Filtro::Filtro()
{
    unsigned short i,j;

    QString path = (QFileDialog::getOpenFileName(0, "Selecione o arquivo com os coeficientes do filtro", QDir::homePath()));
    QFile file(path);

    qDebug() << path;
    if(file.open(QIODevice::ReadOnly))
        qDebug() << "Arquivo do filtro encontrado: " << path;
    QTextStream in(&file);

    while(!in.atEnd())
    {
        this->filtro << in.readLine().toFloat();
    }
    this->ordem_filtro = this->filtro.length() - 1;

    qDebug() << "Leitura dos coeficientes do filtro realizada com sucesso: " << this->filtro;
    qDebug() << "Ordem do filtro: " << this->ordem_filtro;

    file.close();
}

void Filtro::Filtrar(QVector <unsigned char> rx)
{
    register double temp;
    register signed short i,j,k,l;

    static short qtd_amostras;
    static QVector <float> integral;
    float integral_soma;
    static float contador_integral;
    static float ultima_amostra;

    static QVector <float> soma[3];
    QVector <float> sinais_com_filtro[3];
    static QVector <float> sinais_sem_filtro[3];

    QElapsedTimer timer;

    // DEBUG - EXCLUIR DEPOIS
    this->arquivo = new QFile("saida.txt");
    this->arquivo->open(QIODevice::Append | QIODevice::Text);
    QTextStream out (this->arquivo);
    for (i=0; i<rx.length(); i+=6)
    {
        //qDebug() << (qint16) ( (quint16) (rx.data()[i]<<8) | (rx.data()[i+1])) * 7.6294e-05 << ' ' << (qint16) ( (quint16) (rx.data()[i+2]<<8) | (rx.data()[i+3])) * 7.6294e-05 << ' ' << (qint16) ( (quint16) (rx.data()[i+4]<<8) | (rx.data()[i+5])) * 7.6294e-05;
        //out << (qint16) ( (quint16) (rx.data()[i]<<8) | (rx.data()[i+1])) * 7.8162e-05 << ' ' << (qint16) ( (quint16) (rx.data()[i+2]<<8) | (rx.data()[i+3])) * 7.6294e-05 << ' ' << (qint16) ( (quint16) (rx.data()[i+4]<<8) | (rx.data()[i+5])) * 7.6294e-05 << '\n';
    }
    //this->arquivo->close();

    for (i=0; i<rx.length(); i+=6)
    {
        sinais_sem_filtro[0] << (qint16) ( (quint16) (rx.data()[i]<<8) | (rx.data()[i+1])) * 7.6294e-05;
        sinais_sem_filtro[1] << (qint16) ( (quint16) (rx.data()[i+2]<<8) | (rx.data()[i+3])) * 7.6294e-05;
        sinais_sem_filtro[2] << (qint16) ( (quint16) (rx.data()[i+4]<<8) | (rx.data()[i+5])) * 7.6294e-05;
    }

    for (i=0;i<3;i++)
        if (soma[i].length() != this->ordem_filtro+1)
            soma[i].reserve(this->ordem_filtro+1);

    if (sinais_sem_filtro[0].length() > 20)
    {
        timer.start();
        while(!sinais_sem_filtro[0].isEmpty() || !sinais_sem_filtro[1].isEmpty() || !sinais_sem_filtro[2].isEmpty())
        {
            for (i=0;i<3;i++)
            {
                for (j=0;j<=this->ordem_filtro;j++)
                {
                    if(j == this->ordem_filtro)
                    {
                        soma[i].data()[j] = this->filtro[j] * sinais_sem_filtro[i].first();
                    }
                    else
                    {
                        soma[i].data()[j] = this->filtro[j] * sinais_sem_filtro[i].first() + soma[i].data()[j+1];
                    }
                }
                sinais_sem_filtro[i].pop_front();
                sinais_com_filtro[i] << soma[i].data()[0];
            }
        }
        for (i=0;i<sinais_com_filtro[1].length()-1;i++,qtd_amostras++)
        {
            integral << (sinais_com_filtro[1].data()[i]-sinais_com_filtro[1].data()[i+1])*(sinais_com_filtro[1].data()[i]-sinais_com_filtro[1].data()[i+1]);
            if (integral.length() > 100)
                integral.pop_front();

            integral_soma = 0.0;
            for (j=0;j<integral.length();j++)
                integral_soma += integral.data()[j];

            if (integral_soma/100 > 0.001 && qtd_amostras > 120)
            {
                emit BeatRate(QString("%1 BPM").arg(60*485/qtd_amostras));
                out << 60*485/qtd_amostras << '\n';
                qtd_amostras = 0;
            }

        }

        this->arquivo->close();

        emit FilterReady(sinais_com_filtro[0],sinais_com_filtro[1],sinais_com_filtro[2]);
        sinais_sem_filtro[0].clear();
        sinais_sem_filtro[1].clear();
        sinais_sem_filtro[2].clear();
    }
    return;

    /*      FILTRO ANTIGO
    if (sinais_sem_filtro[0].length() > 20 + this->ordem_filtro)
    {
        for (i=0;i<3;i++)
        {
            for (k=0; k < sinais_sem_filtro[i].length() - this->ordem_filtro; k++)
            {
                for (j=this->ordem_filtro+k,l=0; j>=k; j--,l++)
                {
                    temp += sinais_sem_filtro[i].data()[j] * this->filtro.data()[l];
                    //qDebug() << sinais_sem_filtro[i].data()[j] << " * " << this->filtro.data()[l] << " valor:" << j << l;
                }
                //qDebug() << "-------------------------------";
                //qDebug() << temp;
                sinais_com_filtro[i] << temp;
                temp = 0;
            }
        }
        emit FilterReady(sinais_com_filtro[0],sinais_com_filtro[1],sinais_com_filtro[2]);
        sinais_sem_filtro[0].remove(0,sinais_sem_filtro[0].length() - this->ordem_filtro);
        sinais_sem_filtro[1].remove(0,sinais_sem_filtro[1].length() - this->ordem_filtro);
        sinais_sem_filtro[2].remove(0,sinais_sem_filtro[2].length() - this->ordem_filtro);
    }

    return;
    */

    /*
    if (sinais_sem_filtro[0].length() > 20)
    {
        emit FilterReady(sinais_sem_filtro[0],sinais_sem_filtro[1],sinais_sem_filtro[2]);
        sinais_sem_filtro[0].clear();
        sinais_sem_filtro[1].clear();
        sinais_sem_filtro[2].clear();
    }

    return;
    */

}
