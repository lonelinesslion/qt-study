    QFile file("F:/picture/dfs2.jpg");

    QByteArray source;
    if(file.open(QFile::ReadOnly))
    {
        qDebug() << "open";
        QDataStream geit(&file);
        qint64 you;
        while (!geit.atEnd()) {
            geit >> you;
            qDebug() << you;
        }
    }
    else
    {
        qDebug() << "error";
    }
    file.close();
