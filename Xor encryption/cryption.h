#ifndef ENCRYPTION_H
#define ENCRYPTION_H
#include <QString>
#include <QByteArray>

class cryption
{
public:
    cryption();

    QString encryption(QString sourceCode, qint64 key);			//加密文本
    QByteArray encryption(QByteArray sourceCode, qint64 key);
    qint64 encryption(qint64 sourceCode, qint64 key);			//加密整型数据

    QString decryption(QString cryCode, qint64 key);			//解密文本
    QByteArray decryption(QByteArray cryCode, qint64 key);
    qint64 decryption(qint64 sourceCode, qint64 key);			//解密整型数据

    QByteArray getXor(QByteArray source);		//异或加密方法

protected:
    qint64 theKey;
};

#endif // ENCRYPTION_H
