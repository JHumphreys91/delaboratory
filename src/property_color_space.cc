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

#include "property_color_space.h"
#include "color_space_selector.h"
#include "preview_stack.h"
#include "project.h"

dePropertyColorSpace::dePropertyColorSpace(deLayer& _parent)
:deProperty(_parent)
{
    colorSpace = deColorSpaceInvalid;
}

dePropertyColorSpace::~dePropertyColorSpace()
{
}

void dePropertyColorSpace::setColorSpace(deColorSpace _colorSpace)
{
    colorSpace = _colorSpace;
}

void dePropertyColorSpace::addPanelContent(wxPanel* panel, wxSizer* sizer)
{
    wxPanel* selector = new deColorSpaceSelector(panel, *this);
    sizer->Add(selector);
}    
