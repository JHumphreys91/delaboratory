/*
    delaboratory - color correction utility
    Copyright (C) 2011 Jacek Poplawski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DE_LAYER_WITH_BLENDING_H
#define _DE_LAYER_WITH_BLENDING_H

#include "switchable_layer.h"
#include "blend_mode.h"

class deLayerWithBlending:public deSwitchableLayer
{
    private:
        deBlendMode blendMode;
        deValue opacity;
        deImage imageBlendPass;

        virtual const deImage& getLayerImage() const;
        virtual void updateChannelUsage(std::map<int, int>& channelUsage, int layerIndex) const;

    protected:        
        virtual bool isChannelNeutral(int index) = 0;
        virtual bool updateBlendAllChannels();
        virtual void processSingleChannel(int channel);
        virtual bool updateImage();

    public:
        deLayerWithBlending(deColorSpace _colorSpace, deChannelManager& _channelManager, int _sourceLayer, deLayerStack& _layerStack);
        virtual ~deLayerWithBlending();

        bool isBlendingEnabled() const;

        deBlendMode getBlendMode() const {return blendMode;};
        deValue getOpacity() const;

        void setBlendMode(deBlendMode mode);
        void setOpacity(deValue _opacity);

        bool updateBlend(int i);



};

#endif