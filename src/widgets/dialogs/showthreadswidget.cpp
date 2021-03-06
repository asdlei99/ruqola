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

#include "showthreadswidget.h"
#include "thread/listthreadsdelegate.h"
#include "model/threadsfilterproxymodel.h"
#include <QVBoxLayout>
#include <KLocalizedString>
#include <KLineEdit>
#include <QLabel>
#include <QListView>

ShowThreadsWidget::ShowThreadsWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mSearchThreadLineEdit = new KLineEdit(this);
    mSearchThreadLineEdit->setObjectName(QStringLiteral("mSearchThreadLineEdit"));
    mSearchThreadLineEdit->setClearButtonEnabled(true);
    mSearchThreadLineEdit->setTrapReturnKey(true);
    mSearchThreadLineEdit->setPlaceholderText(i18n("Search Thread..."));
    connect(mSearchThreadLineEdit, &KLineEdit::textChanged, this, &ShowThreadsWidget::slotSearchMessageTextChanged);
    mainLayout->addWidget(mSearchThreadLineEdit);

    mThreadInfoLabel = new QLabel(this);
    mThreadInfoLabel->setObjectName(QStringLiteral("mInfo"));
    mThreadInfoLabel->setTextFormat(Qt::RichText);
    mainLayout->addWidget(mThreadInfoLabel);
    QFont labFont = mThreadInfoLabel->font();
    labFont.setBold(true);
    mThreadInfoLabel->setFont(labFont);
    connect(mThreadInfoLabel, &QLabel::linkActivated, this, &ShowThreadsWidget::loadMoreThreads);

    mListThreads = new QListView(this);
    mListThreads->setObjectName(QStringLiteral("mListThreads"));
    mainLayout->addWidget(mListThreads);
    mListThreads->setItemDelegate(new ListThreadsDelegate(this));
}

ShowThreadsWidget::~ShowThreadsWidget()
{
}

void ShowThreadsWidget::setModel(ThreadsFilterProxyModel *model)
{
    mListThreads->setModel(model);
    mThreadsModel = model;
    connect(mThreadsModel, &ThreadsFilterProxyModel::hasFullListChanged, this, &ShowThreadsWidget::updateLabel);
    updateLabel();
}

void ShowThreadsWidget::slotSearchMessageTextChanged(const QString &str)
{
    mThreadsModel->setFilterString(str);
    updateLabel();
}

void ShowThreadsWidget::updateLabel()
{
    mThreadInfoLabel->setText(mThreadsModel->rowCount() == 0 ? i18n("No Thread found") : displayShowDiscussionInRoom());
}

QString ShowThreadsWidget::displayShowDiscussionInRoom() const
{
    QString displayMessageStr;
    displayMessageStr = i18np("%1 Thread in room (Total: %2)", "%1 Threads in room (Total: %2)", mThreadsModel->rowCount(), mThreadsModel->total());
    if (!mThreadsModel->hasFullList()) {
        displayMessageStr += QStringLiteral(" <a href=\"loadmoreelement\">%1</a>").arg(i18n("(Click here for Loading more...)"));
    }
    return displayMessageStr;
}
