    QString out = "中文拼音";
    //out = ui->plainTextEdit->toPlainText();
    qint64 key = 12;
    QByteArray bs = out.toUtf8();
    qDebug() << bs;
    qDebug() << bs.length();
    for(int i=0; i<bs.length(); i++)
        bs[i] = bs[i]^key;
    qDebug() << bs;
    QString in = QString::fromUtf8(bs);
    bs = in.toUtf8();
    for(int i=0; i<bs.length(); i++)
        bs[i] = bs[i]^key;
    QString youy = QString::fromUtf8(bs);

    qDebug() << out;
    qDebug() << sizeof(QChar);
    qDebug() << sizeof(qint16);
    qDebug() << sizeof(out);
    qDebug() << bs;
    qDebug() << sizeof(bs);
    qDebug() << in;
    qDebug() << in.length();
    qDebug() << youy;
