/*
 * offsetlayer.cpp
 * Copyright 2009, Jeff Bland <jeff@teamphobic.com>
 * Copyright 2009-2014, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
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
#include "updatelayercommand.h"
#include "offsetlayer.h"

#include "imagelayer.h"
#include "layermodel.h"
#include "map.h"
#include "mapdocument.h"
#include "maprenderer.h"
#include "objectgroup.h"
#include "tilelayer.h"

#include <QCoreApplication>

using namespace Tiled;
using namespace Tiled::Internal;

UpdateLayerCommand::UpdateLayerCommand(MapDocument *mapDocument)
    : QUndoCommand(QCoreApplication::translate("Undo Commands",
                                               "Update Layer"))
    , mMapDocument(mapDocument)
{
    // Create the offset layer (once)
    Layer *layer = mMapDocument->map()->layerAt(mIndex);
    mOffsetLayer = layer->clone();

    switch (mOffsetLayer->layerType()) {
    case Layer::TileLayerType:
         //   mMapDocument->
        break;
    case Layer::ObjectGroupType:
        break;
    case Layer::ImageLayerType:
        break;
    }
}

UpdateLayerCommand::~UpdateLayerCommand()
{
    delete mOriginalLayer;
    delete mOffsetLayer;
}

void UpdateLayerCommand::undo()
{
    mOffsetLayer = swapLayer(mOriginalLayer);
    mOriginalLayer = nullptr;
}

void UpdateLayerCommand::redo()
{
    /*
    Q_ASSERT(!mOriginalLayer);
    mOriginalLayer = swapLayer(mOffsetLayer);
    mOffsetLayer = nullptr;
    */
    mOriginalLayer = swapLayer(mOffsetLayer);
}

Layer *UpdateLayerCommand::swapLayer(Layer *layer)
{
    const int currentIndex = mMapDocument->currentLayerIndex();

    LayerModel *layerModel = mMapDocument->layerModel();
    Layer *replaced = layerModel->takeLayerAt(mIndex);
    layerModel->insertLayer(mIndex, layer);

    if (mIndex == currentIndex)
        mMapDocument->setCurrentLayerIndex(mIndex);

    return replaced;
}
