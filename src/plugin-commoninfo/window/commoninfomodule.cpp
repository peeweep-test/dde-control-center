/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
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

#include "commoninfomodule.h"
#include "interface/pagemodule.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"
#include "src/plugin-commoninfo/operation/commoninfowork.h"

#include "bootwidget.h"
#include "userexperienceprogramwidget.h"
#include "developermodewidget.h"

#include <DSysInfo>
#include <QApplication>

using namespace DCC_NAMESPACE;
DCORE_USE_NAMESPACE

CommonInfoModule::CommonInfoModule(QObject *parent)
    : HListModule(parent)
    , m_worker(nullptr)
    , m_model(nullptr)
{
    m_model = new CommonInfoModel(this);
    m_worker = new CommonInfoWork(m_model, this);
}

CommonInfoModule::~CommonInfoModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void CommonInfoModule::active()
{
    m_worker->active();
}

QString CommonInfoPlugin::name() const
{
    return QStringLiteral("commoninfo");
}

ModuleObject *CommonInfoPlugin::module()
{   
    //一级菜单--通用设置
    CommonInfoModule *moduleInterface = new CommonInfoModule(this);
    moduleInterface->setName("commoninfo");
    moduleInterface->setDisplayName(tr("General Settings"));
    moduleInterface->setDescription(tr("General Settings"));
    moduleInterface->setIcon(QIcon::fromTheme("dcc_nav_commoninfo"));

    //二级菜单--启动菜单
    ModuleObject *moduleBootMenu = new PageModule("bootMenu", tr("Boot Menu"), this);
    BootModule *bootModule = new BootModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleBootMenu->appendChild(bootModule);
    moduleInterface->appendChild(moduleBootMenu);

    //二级菜单--开发者模式
    ModuleObject *moduleDeveloperMode = new PageModule("developerMode", tr("Developer Mode"), this);
    DeveloperModeModule *developerModeModule = new DeveloperModeModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleDeveloperMode->appendChild(developerModeModule);
    moduleInterface->appendChild(moduleDeveloperMode);

    //二级菜单--开发者模式
    ModuleObject *moduleUserExperienceProgram = new PageModule("userExperienceProgram", tr("User Experience Program"), this);
    UserExperienceProgramModule *userExperienceProgramModule = new UserExperienceProgramModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleUserExperienceProgram->appendChild(userExperienceProgramModule);
    moduleInterface->appendChild(moduleUserExperienceProgram);

    return moduleInterface;
}

QString CommonInfoPlugin::location() const
{
    return "22";
}

QWidget *DeveloperModeModule::page()
{
    DeveloperModeWidget *w = new DeveloperModeWidget;
    w->setModel(m_model);
    connect(w, &DeveloperModeWidget::requestLogin, m_worker, &CommonInfoWork::login);
    connect(w, &DeveloperModeWidget::enableDeveloperMode, this, [=](bool enabled) {
        m_worker->setEnableDeveloperMode(enabled);
    });
    return w;
}

QWidget *UserExperienceProgramModule::page()
{
    UserExperienceProgramWidget *w = new UserExperienceProgramWidget();
    w->setModel(m_model);
    connect(w, &UserExperienceProgramWidget::enableUeProgram, m_worker, &CommonInfoWork::setUeProgram);
    return w;
}

QWidget *BootModule::page()
{
    BootWidget *w = new BootWidget();
    w->setModel(m_model);
    connect(w, &BootWidget::bootdelay, m_worker, &CommonInfoWork::setBootDelay);
    connect(w, &BootWidget::enableTheme, m_worker, &CommonInfoWork::setEnableTheme);
    connect(w, &BootWidget::enableGrubEditAuth, m_worker, [this, w](bool value){
        if (value) {
            w->showGrubEditAuthPasswdDialog(false);
        } else {
            m_worker->disableGrubEditAuth();
        }
    });
    connect(w, &BootWidget::setGrubEditPasswd, m_worker, &CommonInfoWork::onSetGrubEditPasswd);
    connect(w, &BootWidget::defaultEntry, m_worker, &CommonInfoWork::setDefaultEntry);
    connect(w, &BootWidget::requestSetBackground, m_worker, &CommonInfoWork::setBackground);

    w->setGrubEditAuthVisible(m_model->isShowGrubEditAuth());
    return w;
}