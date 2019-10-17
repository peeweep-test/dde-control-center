/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "createaccountpage.h"

#include <QtGlobal>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QApplication>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

CreateAccountPage::CreateAccountPage(QWidget *parent)
    : QWidget(parent)
    , m_avatarListWidget(new AvatarListWidget)
    , m_nameEdit(new DLineEdit)
    , m_fullnameEdit(new DLineEdit)
    , m_passwdEdit(new DPasswordEdit)
    , m_repeatpasswdEdit(new DPasswordEdit)
{
    initWidgets();
}

CreateAccountPage::~CreateAccountPage()
{
}

void CreateAccountPage::initWidgets()
{
    QHBoxLayout *titleLayout = new QHBoxLayout;
    //~ contents_path /accounts/New Account
    QLabel *titleLabel = new QLabel(tr("New Account"));
    titleLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QVBoxLayout *inputLayout = new QVBoxLayout;
    inputLayout->setSpacing(3);

    QLabel *nameLabel = new QLabel(tr("Username"));
    inputLayout->addWidget(nameLabel);
    inputLayout->addWidget(m_nameEdit);

    QLabel *fullnameLabel = new QLabel(tr("Full Name"));
    inputLayout->addWidget(fullnameLabel);
    inputLayout->addWidget(m_fullnameEdit);

    QLabel *passwdLabel = new QLabel(tr("Password"));
    inputLayout->addWidget(passwdLabel);
    inputLayout->addWidget(m_passwdEdit);

    QLabel *repeatpasswdLabel = new QLabel(tr("Repeat Password"));
    inputLayout->addWidget(repeatpasswdLabel);
    inputLayout->addWidget(m_repeatpasswdEdit);

    QHBoxLayout *selectLayout = new QHBoxLayout;
    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *addBtn = new DSuggestButton(tr("Create"));
    selectLayout->addWidget(cancleBtn, 0, Qt::AlignCenter);
    selectLayout->addWidget(addBtn, 0, Qt::AlignCenter);

    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addLayout(titleLayout);
    mainContentLayout->addWidget(m_avatarListWidget);
    mainContentLayout->addLayout(inputLayout);
    mainContentLayout->addLayout(selectLayout);
    mainContentLayout->addStretch();
    setLayout(mainContentLayout);

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });
    connect(addBtn, &DSuggestButton::clicked, this, &CreateAccountPage::createUser);

    connect(m_nameEdit, &DLineEdit::textEdited, this, [ = ](const QString & str) {
        if (m_nameEdit->isAlert()) {
            m_nameEdit->hideAlertMessage();
        }
        m_nameEdit->lineEdit()->setText(str.toLower());
    });

    connect(m_passwdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_passwdEdit->isAlert()) {
            m_passwdEdit->hideAlertMessage();
        }
    });

    connect(m_repeatpasswdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_repeatpasswdEdit->isAlert()) {
            m_repeatpasswdEdit->hideAlertMessage();
        }
    });

    m_nameEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_fullnameEdit->lineEdit()->setPlaceholderText(tr("optional"));//选填
    m_passwdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_repeatpasswdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填

    setFocusPolicy(Qt::StrongFocus);
}

void CreateAccountPage::setModel(User *user)
{
    m_newUser = user;
    Q_ASSERT(m_newUser);
}

void CreateAccountPage::createUser()
{
    //校验输入的用户名和密码
    if (!onNameEditFinished(m_nameEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_passwdEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_repeatpasswdEdit)) {
        return;
    }

    //如果用户没有选图像
    int index = m_avatarListWidget->getCurrentSelectIndex();
    if (index == -1) {
        //随机分配图像 [0, 13]
        index = qrand() % 14;
    }
    m_newUser->setCurrentAvatar(m_avatarListWidget->getAvatarPath(index));
    m_newUser->setName(m_nameEdit->lineEdit()->text());
    m_newUser->setFullname(m_fullnameEdit->lineEdit()->text());
    m_newUser->setPassword(m_passwdEdit->lineEdit()->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->lineEdit()->text());

    Q_EMIT requestCreateUser(m_newUser);
}

bool CreateAccountPage::validatePassword(const QString &password)
{
    QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                              QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("~!@#$%^&*()[]{}\\|/?,.<>");

    return containsChar(password, validate_policy);
}

bool CreateAccountPage::containsChar(const QString &password, const QString &validate)
{
    for (const QChar &p : password) {
        if (!validate.contains(p)) {
            return false;
        }
    }

    return true;
}

void CreateAccountPage::setCreationResult(CreationResult *result)
{
    switch (result->type()) {
    case CreationResult::NoError:
        Q_EMIT requestBack(AccountsWidget::CreateUserSuccess);
        break;
    case CreationResult::UserNameError:
        m_nameEdit->showAlertMessage(result->message());
        break;
    case CreationResult::PasswordError:
        m_passwdEdit->showAlertMessage(result->message());
        break;
    case CreationResult::PasswordMatchError:
        m_repeatpasswdEdit->showAlertMessage(result->message());
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        qWarning() << "error encountered creating user: " << result->message();
        break;
    }

    result->deleteLater();
}

bool CreateAccountPage::onPasswordEditFinished(DPasswordEdit *edit)
{
    const QString &userpassword = edit->lineEdit()->text();
    if (userpassword.isEmpty()) {
        edit->showAlertMessage(tr("Password cannot be empty"));
        return false;
    }

    if (m_nameEdit->text().toLower() == userpassword.toLower()) {
        edit->showAlertMessage(tr("The password should be different from the username"));
        return false;
    }

    bool result = validatePassword(userpassword);
    if (!result) {
        edit->showAlertMessage(tr("Password must only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\|/?,.<>)"));
        return false;
    }

    if (edit == m_repeatpasswdEdit) {
        if (m_passwdEdit->lineEdit()->text() != m_repeatpasswdEdit->lineEdit()->text()) {
            m_repeatpasswdEdit->showAlertMessage(tr("Passwords do not match"));
            return false;
        }
    }
    return true;
}

bool CreateAccountPage::validateUsername(const QString &username)
{
    const QString name_validate = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("-_");
    return containsChar(username, name_validate);
}

bool CreateAccountPage::onNameEditFinished(DLineEdit *edit)
{
    const QString &username = edit->lineEdit()->text();
    if (username.isEmpty()) {
        edit->showAlertMessage(tr("Username cannot be empty"));
        return false;
    }

    if (username.size() < 3 || username.size() > 32) {
        edit->showAlertMessage(tr("Username must be between 3 and 32 characters"));
        return false;
    }

    const QString compStr = "abcdefghijklmnopqrstuvwxyz";
    if (!compStr.contains(username.at(0))) {
        edit->showAlertMessage(tr("The first character must be in lower case"));
        return false;
    }

    if (!validateUsername(username)) {
        edit->showAlertMessage(tr("Username must only contain a~z, 0~9, - or _"));
        return false;
    }
    return true;
}
