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

#include "wx/wx.h"
#include "project.h"
#include "main_frame.h"
#include "str.h"
#include "rgb2xyz2lab.h"
#include "layer_processor.h"
#include "logger.h"
#include "channel_manager.h"
#include "layer_stack.h"
#include "layer_frame_manager.h"

class deLaboratory: public wxApp
{	
    public:
        deLaboratory()
        :wxApp(),
         processor(previewChannelManager, layerStack, layerFrameManager),
         project(processor, previewChannelManager, sourceChannelManager, layerStack, layerFrameManager)
        {
            frame = NULL;
        }

        ~deLaboratory()
        {
        }

    private:
    	virtual bool OnInit();
    	virtual int OnExit();
        deMainFrame* frame;
        deSamplerManager samplerManager;
        deLayerStack layerStack;
        deLayerFrameManager layerFrameManager;
        deChannelManager previewChannelManager;
        deChannelManager sourceChannelManager;
        deLayerProcessor processor;
        deProject project;

        virtual int FilterEvent(wxEvent& event);

};


IMPLEMENT_APP(deLaboratory)

int deLaboratory::FilterEvent(wxEvent& event)
{
    if (project.shouldReceiveKeys())
    {
        if  (event.GetEventType()==wxEVT_KEY_DOWN )
        {
             project.onKey(((wxKeyEvent&)event).GetKeyCode());
             if (frame)
             {
                 frame->onKey(((wxKeyEvent&)event).GetKeyCode());
             }                 
             return true;
        }
    }

    return -1;
}


int deLaboratory::OnExit()
{
    project.log("OnExit");
    return 0;
}   

bool deLaboratory::OnInit()
{
    logMessage("deLaboratory::OnInit");

    wxInitAllImageHandlers();

	int width = 1200;
	int height = 960;

	frame = new deMainFrame( wxSize(width,height), project, processor, samplerManager);

    if (argc > 1)
    {
        wxString a = argv[1];
        project.init(str(a));
    }

	frame->Show(TRUE);
	SetTopWindow(frame);

    initLAB();

    logMessage("setMainFrame...");

    processor.setMainFrame(frame);

    logMessage("startWorkerThread...");

    processor.startWorkerThread();

    logMessage("OnInit done");

	return TRUE;
} 


	
