#include "cryption.h"

cryption::cryption()
{
    theKey = 0;
}

QString cryption::encryption(QString sourceCode, qint64 key)
{
    theKey = key;
    QByteArray sourceBa = sourceCode.toUtf8();
    QString encryptStr = QString::fromUtf8(getXor(sourceBa));
    return encryptStr;
}
QByteArray cryption::encryption(QByteArray sourceCode, qint64 key)
{
    theKey = key;
    return getXor(sourceCode);
}

QString cryption::decryption(QString cryCode, qint64 key)
{
    return encryption(cryCode, key);
}
QByteArray cryption::decryption(QByteArray cryCode, qint64 key)
{
    theKey = key;
    return getXor(cryCode);
}

QByteArray cryption::getXor(QByteArray source)	//异或加密
{
    for(int i=0; i< source.length(); i++)
        source[i] = source[i]^theKey;
    return source;
}

qint64 cryption::encryption(qint64 sourceCode, qint64 key)	//加密整型数据
{
    sourceCode = sourceCode^key;
    return sourceCode;
}
qint64 cryption::decryption(qint64 sourceCode, qint64 key)	//解密整型数据
{
    sourceCode = sourceCode^key;
    return sourceCode;
}
