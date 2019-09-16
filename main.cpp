#include "MainWindow.h"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTextCodec>
#include <QtMessageHandler>
QFile* g_log_file = nullptr;
QtMessageHandler g_defaultHandler = nullptr;
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString logStr;
      QByteArray localMsg = msg.toLocal8Bit();
      const char *file = context.file ? context.file : "";
      const char *function = context.function ? context.function : "";
      logStr = QString("%1                              [%2,%3,%4]").arg(localMsg.constData()).arg(file).arg(function).arg(context.line);
      if(g_log_file)
      {
        if(g_log_file->isOpen())
        {
           QTextStream s(g_log_file);
           s.setCodec("utf-8");
           s << logStr << "\r\n";
        }
      }
      //用系统原来的函数完成原来的功能. 比如输出到调试窗
      if(g_defaultHandler)
      {
              g_defaultHandler(type,context,msg);
      }

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    g_log_file = new QFile("./log.log");
    if(g_log_file->open(QIODevice::ReadWrite|QIODevice::Append))
    {
        QTextStream s(g_log_file);
        s.setCodec("utf-8");
        s << "\r\n";
        s << "==============" << QDateTime::currentDateTime().toString(Qt::ISODate) << "==================";
        s << "\r\n";
    }
    g_defaultHandler = qInstallMessageHandler(myMessageOutput);
    MainWindow w;
    w.show();

    return a.exec();
}
