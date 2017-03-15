eg:
    cryption cr;
    QString source = "中国制造";
    QString decry;
    QByteArray sourceBa = "你是我的眼";
    QByteArray decryBa;

    decry = cr.encryption(source, 12);
    qDebug() << decry;
    qDebug() << cr.decryption(decry, 12);

    decryBa = cr.encryption(sourceBa, 12);
    qDebug() << sourceBa;
    qDebug() << decryBa;
    qDebug() << cr.decryption(decryBa, 12);
