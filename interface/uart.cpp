#include "uart.h"

Uart::Uart(long long baudRate, QString porta, short tam)
{
    this->baudRate = baudRate;
    this->porta = porta;
    this->tam = tam;
}

Uart::~Uart()
{}

void Uart::run() {

    register QByteArray value;
    register QString a;
    register unsigned short i;
    register QVector <unsigned char> buffer;

    this->conexao = new QSerialPort();

    this->conexao->setPortName(this->porta);
    this->conexao->setBaudRate(this->baudRate);
    this->conexao->setDataBits(QSerialPort::Data8);
    this->conexao->setFlowControl(QSerialPort::NoFlowControl);
    this->conexao->setParity(QSerialPort::NoParity);
    this->conexao->setStopBits(QSerialPort::OneStop);

    if (conexao->open(QIODevice::ReadWrite))
    {
        qDebug() << "conectou";
    }
    else
        qDebug() << "erro ao conectar";

    while (1)
    {
        if (conexao->waitForReadyRead(400))
        {
            value = conexao->readAll();
            //qDebug() << value << "tamanho: " << value.length();
            for (i = 0; i < value.length() ; i++)
            {
                this->rx << (unsigned char) (value.data()[i]);
            }
        }

        //qDebug() << this->rx;

        value.clear();

        buffer.clear();

        for (;!this->rx.isEmpty();)
        {
            if (rx.length() < this->tam)
            {
                break;
            }
            else
            {
                if(this->rx.data()[this->tam-2] == '\r' && this->rx.data()[this->tam-1] == '\r')
                {
                    buffer << this->rx.mid(0,this->tam-2);
                    this->rx.remove(0,this->tam);
                }
                else
                {
                    this->rx.pop_front();
                }
            }
        }

        if (!buffer.isEmpty())
            emit readReady(buffer);
    }
}

