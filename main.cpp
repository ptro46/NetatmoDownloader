//
// This file is part of the Netatmo Downloader project
//
// Copyright (C)   2016  Patrice Trognon
//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE VERSION 3 as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU LESSER GENERAL PUBLIC LICENSE VERSION 3 for more details.
//
// You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE VERSION 3
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

#include <QCoreApplication>

#include "TaskBotGetMeasures.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(type)
    Q_UNUSED(context)
    Q_UNUSED(msg)
}


int main(int argc, char *argv[])
{
    QString configFileName ;
    bool showConfig = false ;
    QCoreApplication a(argc, argv);

    qInstallMessageHandler(myMessageOutput);

    if ( argc == 2 ) {
        QString arg = argv[1];
        if ( arg.toLower() == "showconfig" ) {
            showConfig = true ;
        } else {
            configFileName = arg;
        }
    }

    TaskBotGetMeasures *task = new TaskBotGetMeasures(showConfig, configFileName, &a);

    QObject::connect(task,SIGNAL(finished()), &a, SLOT(quit()));

    QTimer::singleShot(0,task,SLOT(run()));

    return a.exec();
}
