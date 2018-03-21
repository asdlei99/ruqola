/*
   Copyright (c) 2018 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or
   ( at your option ) version 3 or, at the discretion of KDE e.V.
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "postmessagejobtest.h"
#include "restapi/postmessagejob.h"
#include "ruqola_restapi_helper.h"
#include <QTest>
#include <QJsonDocument>
QTEST_GUILESS_MAIN(PostMessageJobTest)

PostMessageJobTest::PostMessageJobTest(QObject *parent)
    : QObject(parent)
{
}

void PostMessageJobTest::shouldHaveDefaultValue()
{
    PostMessageJob job;
    verifyDefaultValue(&job);
    QVERIFY(job.requireHttpAuthentication());
    QVERIFY(job.roomId().isEmpty());
    QVERIFY(job.text().isEmpty());
}

void PostMessageJobTest::shouldGenerateRequest()
{
    PostMessageJob job;
    QNetworkRequest request = QNetworkRequest(QUrl());
    verifyAuthentication(&job, request);
    QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/v1/chat.postMessage")));
    QCOMPARE(request.header(QNetworkRequest::ContentTypeHeader).toString(), QStringLiteral("application/json"));
}

void PostMessageJobTest::shouldGenerateJson()
{
    PostMessageJob job;
    const QString roomId = QStringLiteral("foo1");
    const QString text = QStringLiteral("topic1");
    job.setRoomId(roomId);
    job.setText(text);
    QCOMPARE(job.json().toJson(QJsonDocument::Compact), QStringLiteral("{\"roomId\":\"%1\",\"text\":\"%2\"}").arg(roomId).arg(text).toLatin1());
}
