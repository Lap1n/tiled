/*
 * commandbutton.cpp
 * Copyright 2010, Jeff Bland <jksb@member.fsf.org>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "updatelayerbutton.h"
#include "commanddatamodel.h"
#include "commanddialog.h"
#include "mapdocument.h"
#include "utils.h"
#include "updatelayercommand.h"
#include <QEvent>
#include <QMenu>
#include <QMessageBox>
#include "tilelayer.h"
#include "stampbrush.h"
using namespace Tiled;
using namespace Tiled::Utils;
using namespace Tiled::Internal;

UpdateLayerButton::UpdateLayerButton(QWidget *parent)
    : QToolButton(parent)
    , mMenu(new QMenu(this))
{
    setIcon(QIcon(QLatin1String(":images/24x24/system-run.png")));
    setThemeIcon(this, "system-run");
    retranslateUi();

    setPopupMode(QToolButton::MenuButtonPopup);
    setMenu(mMenu);

    connect(mMenu, SIGNAL(aboutToShow()), SLOT(populateMenu()));
    connect(this, SIGNAL(clicked()), SLOT(runCommand()));
}

void UpdateLayerButton::runCommand()
{
     mMapDocument =  dynamic_cast<MapDocument*>(DocumentManager::instance()->currentDocument());
       mMapDocument =  dynamic_cast<MapDocument*>(DocumentManager::instance()->currentDocument()) ;


       QRegion paintedRegion;
       QVector<StampBrush::PaintOperation> operations;
       QHash<TileLayer *, QRegion> regionCache;
        std::map<QString,TileLayer> tileLayerMap;
       if(mMapDocument->currentLayer()->isTileLayer()){
           TileLayer* currentLayer = dynamic_cast<TileLayer*>(mMapDocument->currentLayer());
           int index=0;
           int x=0;
           int y=0;
           for(Cell cell : *currentLayer){
               y=index/currentLayer->width();
               x=index%currentLayer->width();
                int test = cell.tileId();
                if(test!=-1){
                    if(cell.tile()->hasProperty(tr("Target Layer"))&&test!=-1){
                       QString targetLayerName=cell.tile()->properties().value(tr("Target Layer")).toString();
                       TileLayer* targetLayer=  getSpecifiedTileLayer(targetLayerName);
                       targetLayer->setCell(x,y,cell);
                    }
                }
               index++;
           }
       }

}

void UpdateLayerButton::showDialog()
{
    CommandDialog dialog(window());
    dialog.exec();
}

void UpdateLayerButton::populateMenu()
{
    mMenu->clear();

    // Use a data model for getting the command list to avoid having to
    // manually parse the settings
    const CommandDataModel model;
    const QList<Command> &commands = model.allCommands();

    foreach (const Command &command, commands) {
        if (!command.isEnabled)
            continue;

        QAction *action = mMenu->addAction(command.name);
        action->setStatusTip(command.command);
        action->setData(command.toQVariant());
        connect(action, SIGNAL(triggered()), SLOT(runCommand()));
    }

    if (!mMenu->isEmpty())
        mMenu->addSeparator();

    // Add "Edit Commands..." action
    QAction *action = mMenu->addAction(tr("Edit Commands..."));
    connect(action, SIGNAL(triggered()), SLOT(showDialog()));
}

void UpdateLayerButton::changeEvent(QEvent *event)
{
    QToolButton::changeEvent(event);

    switch (event->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

void UpdateLayerButton::retranslateUi()
{
    setToolTip(tr("Execute Command"));
    setShortcut(QKeySequence(tr("F5")));
}


TileLayer *UpdateLayerButton::getSpecifiedTileLayer(QString& layerName) const
{
     // const QString layerName =(prop.value(tr("Layer Name"))).toString();
    if (!mMapDocument)
        return nullptr;
    int index=0;
    foreach(Layer* tl , mMapDocument->map()->layers()){
        if(tl->name()==layerName){
           //  mapDocument()->setCurrentLayerIndex(index);
             return dynamic_cast<TileLayer*>(tl);
        }
        index++;
    }
    //If it doesn't exist, we create a new layer
    return dynamic_cast<TileLayer*>(mMapDocument->addLayer(Layer::TypeFlag::TileLayerType,layerName));

}
