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

#include "property_layer_index.h"
#include "layer_selector.h"

dePropertyLayerIndex::dePropertyLayerIndex(deLayer& _parent, const std::string& _label)
:deProperty(_parent), label(_label)
{
    index = -1;
}

dePropertyLayerIndex::~dePropertyLayerIndex()
{
}

void dePropertyLayerIndex::setIndex(int _index)
{
    index = _index;
}

void dePropertyLayerIndex::addPanelContent(wxPanel* panel, wxSizer* sizer)
{
    wxPanel* selector = new deLayerSelector(panel, *this, label);
    sizer->Add(selector);
}    