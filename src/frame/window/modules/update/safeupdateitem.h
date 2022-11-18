// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SAFEUPDATEITEM_H
#define SAFEUPDATEITEM_H

#include "updatesettingitem.h"

namespace dcc {
namespace update {


class SafeUpdateItem: public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit SafeUpdateItem(QWidget *parent = nullptr);

    void init();

    void setData(UpdateItemInfo *updateItemInfo) override;
};

}
}

#endif // SAFEUPDATEITEM_H