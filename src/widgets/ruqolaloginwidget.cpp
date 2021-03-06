/*
   Copyright (c) 2020 Laurent Montel <montel@kde.org>

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

#include "ruqolaloginwidget.h"
#include "ruqola.h"
#include "rocketchataccount.h"
#include "common/authenticationcombobox.h"
#include <QVBoxLayout>
#include <KLocalizedString>
#include <KPasswordLineEdit>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <KBusyIndicatorWidget>

RuqolaLoginWidget::RuqolaLoginWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QFormLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    mAccountName = new QLabel(this);
    mAccountName->setObjectName(QStringLiteral("mAccountName"));
    mainLayout->addRow(i18n("Account Name:"), mAccountName);

    mServerName = new QLineEdit(this);
    mServerName->setObjectName(QStringLiteral("mServerName"));
    mainLayout->addRow(i18n("Server Name:"), mServerName);

    mUserName = new QLineEdit(this);
    mUserName->setObjectName(QStringLiteral("mUserName"));
    mainLayout->addRow(i18n("User Name:"), mUserName);

    // Type of account
    mAuthenticationAccountWidget = new QWidget(this);
    mAuthenticationAccountWidget->setObjectName(QStringLiteral("mAuthenticationAccountWidget"));
    mAuthenticationAccountWidget->setVisible(false);

    QVBoxLayout *authenticationAccountLayout = new QVBoxLayout(mAuthenticationAccountWidget);
    authenticationAccountLayout->setObjectName(QStringLiteral("authenticationAccountLayout"));

    QLabel *authenticationAccountLabel = new QLabel(i18n("Authentication Method"), this);
    authenticationAccountLabel->setObjectName(QStringLiteral("authenticationAccountLabel"));
    authenticationAccountLayout->addWidget(authenticationAccountLabel);

    mAuthenticationCombobox = new AuthenticationComboBox(this);
    mAuthenticationCombobox->setObjectName(QStringLiteral("mAuthenticationCombobox"));
    authenticationAccountLayout->addWidget(mAuthenticationCombobox);
    mainLayout->addWidget(mAuthenticationAccountWidget);
    mAuthenticationAccountWidget->setVisible(false);

    // Password
    mPasswordLineEdit = new KPasswordLineEdit(this);
    mPasswordLineEdit->setObjectName(QStringLiteral("mPasswordLineEdit"));
    connect(mPasswordLineEdit->lineEdit(), &QLineEdit::returnPressed, this, &RuqolaLoginWidget::slotLogin);
    mainLayout->addRow(i18n("Password:"), mPasswordLineEdit);

    mLoginButton = new QPushButton(i18n("Login"), this);
    mLoginButton->setObjectName(QStringLiteral("mLoginButton"));
    mainLayout->addWidget(mLoginButton);
    connect(mLoginButton, &QPushButton::clicked, this, &RuqolaLoginWidget::slotLogin);

    // Two Factor authentication
    mAuthenticationWidget = new QWidget(this);
    mAuthenticationWidget->setObjectName(QStringLiteral("authenticationWidget"));
    mAuthenticationWidget->setVisible(false);

    QVBoxLayout *twoFactorAuthenticationLayout = new QVBoxLayout(mAuthenticationWidget);
    twoFactorAuthenticationLayout->setObjectName(QStringLiteral("twoFactorAuthenticationLayout"));

    QLabel *twoFactorAuthenticationLabel = new QLabel(i18n("You have enabled second factor authentication.\nPlease enter the generated code or a backup code."), this);
    twoFactorAuthenticationLabel->setObjectName(QStringLiteral("twoFactorAuthenticationLabel"));
    twoFactorAuthenticationLayout->addWidget(twoFactorAuthenticationLabel);

    mTwoFactorAuthenticationPasswordLineEdit = new KPasswordLineEdit(this);
    mTwoFactorAuthenticationPasswordLineEdit->setObjectName(QStringLiteral("mTwoFactorAuthenticationPasswordLineEdit"));
    twoFactorAuthenticationLayout->addWidget(mTwoFactorAuthenticationPasswordLineEdit);
    connect(mTwoFactorAuthenticationPasswordLineEdit->lineEdit(), &QLineEdit::returnPressed, this, []() {
        qDebug() << " not implemented";
    });

    mainLayout->addWidget(mAuthenticationWidget);

    mBusyIndicatorWidget = new KBusyIndicatorWidget(this);
    mBusyIndicatorWidget->setObjectName(QStringLiteral("mBusyIndicatorWidget"));
    mainLayout->addWidget(mBusyIndicatorWidget);
    //Hide by default
    mBusyIndicatorWidget->hide();

    mFailedError = new QLabel(this);
    mFailedError->setObjectName(QStringLiteral("mFailedError"));
    mainLayout->addWidget(mFailedError);
    //Hide by default
    mFailedError->hide();
}

RuqolaLoginWidget::~RuqolaLoginWidget()
{
}

void RuqolaLoginWidget::initialize()
{
    auto *rocketChatAccount = Ruqola::self()->rocketChatAccount();
    mAccountName->setText(rocketChatAccount->accountName());
    mServerName->setText(rocketChatAccount->serverUrl());
    mUserName->setText(rocketChatAccount->userName());
    mPasswordLineEdit->setPassword(rocketChatAccount->password());
    mAuthenticationWidget->setVisible(false);
    mAuthenticationWidget->setVisible(mAuthenticationCombobox->count() > 1);
}

void RuqolaLoginWidget::slotLogin()
{
    auto *rocketChatAccount = Ruqola::self()->rocketChatAccount();
    rocketChatAccount->setAccountName(mAccountName->text());
    rocketChatAccount->setServerUrl(mServerName->text());
    rocketChatAccount->setUserName(mUserName->text());
    rocketChatAccount->setPassword(mPasswordLineEdit->password());
    //twoFactorAuthenticationCode ?
    rocketChatAccount->tryLogin();
}

void RuqolaLoginWidget::changeWidgetStatus(bool enabled)
{
    mServerName->setEnabled(enabled);
    mUserName->setEnabled(enabled);
    mPasswordLineEdit->setEnabled(enabled);
    mLoginButton->setEnabled(enabled);
}

void RuqolaLoginWidget::setLoginStatus(DDPClient::LoginStatus status)
{
    mFailedError->setHidden(true);
    switch (status) {
    case DDPClient::LoginStatus::NotConnected:
        mBusyIndicatorWidget->hide();
        changeWidgetStatus(true);
        break;
    case DDPClient::LoginStatus::LoggingIn:
        mBusyIndicatorWidget->show();
        changeWidgetStatus(false);
        break;
    case DDPClient::LoginStatus::LoggedIn:
        mBusyIndicatorWidget->hide();
        changeWidgetStatus(true);
        break;
    case DDPClient::LoginStatus::LoginFailed:
        mBusyIndicatorWidget->hide();
        changeWidgetStatus(true);
        showError(i18n("Login Failed"));
        break;
    case DDPClient::LoginStatus::LoginCodeRequired:
        mBusyIndicatorWidget->hide();
        changeWidgetStatus(true);
        mAuthenticationWidget->setVisible(true);
        break;
    case DDPClient::LoginStatus::LoggedOut:
        mBusyIndicatorWidget->hide();
        changeWidgetStatus(true);
        break;
    case DDPClient::LoginStatus::FailedToLoginPluginProblem:
        mBusyIndicatorWidget->hide();
        changeWidgetStatus(true);
        showError(i18n("Installation Problem found. No plugins found here."));
        break;
    }
}

void RuqolaLoginWidget::showError(const QString &text)
{
    mFailedError->setVisible(true);
    mFailedError->setText(text);
}
