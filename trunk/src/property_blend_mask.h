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

#ifndef _DE_PROPERTY_BLEND_MASK_H
#define _DE_PROPERTY_BLEND_MASK_H

#include "property.h"
#include <string>

class dePropertyBlendMask:public deProperty
{
    private:
        int layerIndex;
        int channel;
        std::string label;

    public:
        dePropertyBlendMask(deLayer& _parent, const std::string& _label);
        virtual ~dePropertyBlendMask();

        int getLayerIndex() const {return layerIndex;};
        void setLayerIndex(int _index);
        int getChannel() const {return channel;};
        void setChannel(int _channel);

        virtual void addPanelContent(wxPanel* panel, wxSizer* sizer);
        virtual void saveContent(xmlNodePtr node);

};

#endif