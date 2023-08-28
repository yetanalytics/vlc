/*****************************************************************************
 * Copyright (C) 2023 VLC authors and VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * ( at your option ) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

import QtQuick 2.12

import "qrc:///widgets/" as Widgets
import "qrc:///player/" as Player
import "qrc:///style/"

import org.videolan.vlc 0.1

Widgets.IconControlButton {
    visible: Player.isInteractive
    iconText: VLCIcons.ic_fluent_arrow_move
    text: I18n.qtr("Navigation Buttons")

    onClicked: MainCtx.navBoxToggled()
}
