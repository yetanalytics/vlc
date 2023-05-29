/*****************************************************************************
 * Copyright (C) 2021 VLC authors and VideoLAN
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

import org.videolan.vlc 0.1

import "qrc:///style/"

ControlBar {
    id: root

    height: 0

    visible: false

    state: (Player.playingState === Player.PLAYING_STATE_STOPPED) ? ""
                                                                  : "expanded"

    textPosition: (MainCtx.pinVideoControls) ? ControlBar.TimeTextPosition.LeftRightSlider
                                             : ControlBar.TimeTextPosition.Hide

    sliderHeight: (MainCtx.pinVideoControls) ? VLCStyle.heightBar_xxsmall
                                             : VLCStyle.dp(3, VLCStyle.scale)

    bookmarksHeight: (MainCtx.pinVideoControls) ? VLCStyle.controlBarBookmarksHeight
                                                : VLCStyle.icon_xsmall * 0.7

    identifier: PlayerControlbarModel.Miniplayer

    states: State {
        name: "expanded"

        PropertyChanges {
            target: root
            visible: true
            height: implicitHeight
        }
    }

    transitions: Transition {
        from: ""; to: "expanded"
        reversible: true

        SequentialAnimation {
            // visible should change first, in order for inner layouts to calculate implicitHeight correctly
            PropertyAction { property: "visible" }
            NumberAnimation { property: "height"; easing.type: Easing.InOutSine; duration: VLCStyle.duration_long; }
        }
    }
}
