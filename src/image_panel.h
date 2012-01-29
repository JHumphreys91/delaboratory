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

#ifndef _DE_IMAGE_PANEL_H
#define _DE_IMAGE_PANEL_H

#include "wx/wx.h"
#include "renderer.h"
#include "value.h"
class deProject;
class deSamplerManager;

class deImagePanel:public wxPanel
{
private:
	
    deProject& project;
    bool clicked;
    deSamplerManager& samplerManager;

	void click(wxMouseEvent &event);
	void release(wxMouseEvent &event);
	void move(wxMouseEvent &event);
	void wheel(wxMouseEvent &event);

    bool onClick(deValue x, deValue y);
    bool onMove(deValue x, deValue y);
    bool onRelease();

    void drawSamplers(wxDC& dc);

public:
	deImagePanel(wxWindow* parent, deProject& _project, deSamplerManager& _samplerManager);
	virtual ~deImagePanel();

	void paintEvent(wxPaintEvent & evt);
	void render(wxDC& dc);
    void repaintImagePanel();

	DECLARE_EVENT_TABLE()

};

#endif
