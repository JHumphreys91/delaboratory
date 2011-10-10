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

#ifndef _DE_CONTROL_PANEL_H
#define _DE_CONTROL_PANEL_H

#include <wx/wx.h>
#include <map>
#include <vector>
#include "color_space.h"
class deProject;
class deLayerGridPanel;
class deSamplerManagerFrame;

class deControlPanel:public wxPanel
{
    private:
        deProject* project;
        deLayerGridPanel* layerGridPanel;
        wxSizer* mainSizer;

        deSamplerManagerFrame* samplerManagerFrame;

        wxButton* samplers;

        wxButton* exportTIFF;
        wxButton* externalEditor;

        std::map<int, deColorSpace> convertButtonsColorSpaces;
        std::vector<wxButton*> convertButtons;

        std::map<int, std::string> actionButtonsNames;
        std::vector<wxButton*> actionButtons;

        wxButton* deleteLayer;

        void click(wxCommandEvent &event);

        void setConversions();

    public:
        deControlPanel(wxWindow* parent, deProject* _project, deLayerGridPanel* _layerGridPanel);
        ~deControlPanel();

        void onChangeView();

        void closeSamplerManagerFrame();
        void updateSamplerManagerFrame();

        bool samplersVisible() const;

        void updateLayerGrid();
};

/*
#include <wx/listctrl.h>
#include <vector>


class deLayerGridPanel:public wxPanel
{
    class deLayerRow
    {
        public:
            deLayerRow(int _index)
            :index(_index)
            {
            };

            int index;
            wxStaticText* id;
            wxRadioButton* view;
            wxStaticText* name;
            wxButton* action;
            wxButton* blend;
            wxCheckBox* enabled;
    };

    private:
        std::vector<deLayerRow> layerRows;

        deProject* project;


        wxFlexGridSizer* gridSizer;

        wxButton* addCurvesQuick;
        wxButton* addMixerQuick;
        wxButton* addLABQuick;
        wxButton* addLCHQuick;
        wxButton* addRGBQuick;

        wxButton* addCurves;
        wxButton* addLAB;
        wxButton* addRGB;
        wxButton* addHSV;
        wxButton* addHSL;


        void buildRows();
        void clearRows();
        void check(wxCommandEvent &event);
        void select(wxCommandEvent &event);
        void repaintImage();
    
    public:
        deLayerGridPanel(wxWindow* parent, deProject* _project);
        ~deLayerGridPanel();


//      void update();
        
};

*/

#endif