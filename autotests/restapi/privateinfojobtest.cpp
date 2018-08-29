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

#include "privateinfojobtest.h"
#include "restapi/settings/privateinfojob.h"
#include <QTest>
#include <restapi/restapimethod.h>
QTEST_GUILESS_MAIN(PrivateInfoJobTest)

PrivateInfoJobTest::PrivateInfoJobTest(QObject *parent)
    : QObject(parent)
{
}

void PrivateInfoJobTest::shouldHaveDefaultValue()
{
    PrivateInfoJob job;
    QVERIFY(!job.restApiMethod());
    QVERIFY(!job.networkAccessManager());
    QVERIFY(!job.start());
    QVERIFY(job.requireHttpAuthentication());
    QVERIFY(job.authToken().isEmpty());
    QVERIFY(job.userId().isEmpty());
    QVERIFY(!job.restApiLogger());
}

void PrivateInfoJobTest::shouldGenerateRequest()
{
    PrivateInfoJob job;
    const QString authToken = QStringLiteral("foo");
    const QString userId = QStringLiteral("user");
    job.setUserId(userId);
    job.setAuthToken(authToken);
    RestApiMethod *method = new RestApiMethod;
    method->setServerUrl(QStringLiteral("http://www.kde.org"));
    job.setRestApiMethod(method);
    const QNetworkRequest request = job.request();
    QCOMPARE(request.url(), QUrl(QStringLiteral("http://www.kde.org/api/v1/settings")));
    QCOMPARE(request.rawHeader(QByteArrayLiteral("X-Auth-Token")), authToken.toLocal8Bit());
    QCOMPARE(request.rawHeader(QByteArrayLiteral("X-User-Id")), userId.toLocal8Bit());
    delete method;
}
