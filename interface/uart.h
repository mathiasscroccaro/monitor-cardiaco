#ifndef UART_H
#define UART_H
#include <QSerialPort>
#include <QString>
#include <QDebug>

extern QVector <char> rxUart;


class Uart : public QObject
{
    Q_OBJECT
private:
    QSerialPort     *   conexao;

    long int            baudRate;
    QString             porta;

    unsigned short       tam;
    QVector <unsigned char>      rx;

public:
    explicit Uart(long long, QString, short tam);
    virtual ~Uart();

signals:
    void readReady(const QVector <unsigned char> &rx);

public slots:
    void run();
};

#endif // UART_H
