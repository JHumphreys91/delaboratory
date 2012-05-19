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

#include "project.h"
#include "image_io.h"
#include <cassert>
#include <wx/wx.h>
#include <iostream>
#include "source_image_layer.h"
#include "curves_layer.h"
#include "conversion_layer.h"
#include "image_panel.h"
#include "str.h"
#include "histogram_panel.h"
#include "histogram_mode_panel.h"
#include "control_panel.h"
#include "view_mode_panel.h"
#include <sstream>
#include "layer_factory.h"
#include "xml.h"
#include "image_area_panel.h"
#include "memory_info_frame.h"
#include <iostream>
#include "main_frame.h"
#include "layer_processor.h"
#include "external_editor.h"
#include "channel_manager.h"
#include "layer_stack.h"
#include "layer_frame_manager.h"
#include "static_image.h"
#include "raw_module.h"
#include "zoom_manager.h"
#include "color_space_utils.h"
#include "operation_processor.h"
#include "main_window.h"
#include "conversion_processor.h"
#include "test_image.h"

#define ICC_MESSAGE 0

deProject::deProject(deLayerProcessor& _processor, deChannelManager& _previewChannelManager, deLayerStack& _layerStack, deLayerFrameManager& _layerFrameManager, deStaticImage& _sourceImage, deRawModule& _rawModule, deZoomManager& _zoomManager, deOperationProcessor& _operationProcessor, deMainWindow& _mainWindow)
:layerProcessor(_processor),
 viewModePanel(NULL),
 previewChannelManager(_previewChannelManager),
 controlPanel(NULL),
 memoryInfoFrame(NULL),
 viewManager(*this, _processor, _zoomManager),
 sourceImage(_sourceImage),
 layerStack(_layerStack),
 layerFrameManager(_layerFrameManager),
 histogramModePanel(NULL),
 imageAreaPanel(NULL),
 rawModule(_rawModule),
 zoomManager(_zoomManager),
 operationProcessor(_operationProcessor),
 mainWindow(_mainWindow)
{
    imageFileName = "";
    sourceImageFileName = "";
    receiveKeys = true;

    logInfo("project constructor");

    layerProcessor.setViewManager(&viewManager);

    resetLayerStack(deColorSpaceRGB);


}

void deProject::disableKeys()
{
    receiveKeys = false;
}

void deProject::enableKeys()
{
    receiveKeys = true;
}

deProject::~deProject()
{
    logInfo("project destructor");
    layerProcessor.lock();
    layerProcessor.unlock();
    layerStack.clear();
}

void deProject::setHistogramChannel(int channel)
{
    if (histogramModePanel)
    {
        histogramModePanel->updateMode(channel);
    }            
    layerProcessor.generateHistogram();
}

void deProject::onKey(int key)
{
    if (key == '`')
    {
        viewManager.setNormal();
    }
    if (key == '1')
    {
        viewManager.setSingleChannel(0);
    }
    if (key == '2')
    {
        viewManager.setSingleChannel(1);
    }
    if (key == '3')
    {
        viewManager.setSingleChannel(2);
    }
    if (key == '4')
    {
        viewManager.setSingleChannel(3);
    }
    if (key == WXK_F1)
    {
        setHistogramChannel(0);
    }
    if (key == WXK_F2)
    {
        setHistogramChannel(1);
    }
    if (key == WXK_F3)
    {
        setHistogramChannel(2);
    }
    if (key == WXK_F4)
    {
        setHistogramChannel(3);
    }

    layerFrameManager.onKey(key);
    if (controlPanel)
    {
        controlPanel->onKey(key);
    }        
}

void deProject::init(const std::string& fileName)
{
    if (openImage(fileName, true, deColorSpaceProPhoto))
    {
        return;
    }
    if (openImage(fileName, false, deColorSpaceRGB));
    {
        return;
    }
    open(fileName, true);
}

void deProject::freeImage()
{
}

void deProject::setTestImage(int s)
{
    if (rawModule.isActive())
    {
        wxMessageBox(_T("RAW module is already active"));
        return;
    }

    generateTestImage(sourceImage, s);
    if (imageAreaPanel)
    {
        imageAreaPanel->updateSize(true);
    }        
    resetLayerStack(sourceImage.getColorSpace());

/*
    freeImage();

    deSize size(s, s);

    sourceImage.setColorSpace(deColorSpaceRGB);
    sourceImage.setSize(size);
    deChannel* channelRR = sourceImage.getChannel(0);
    deChannel* channelGG = sourceImage.getChannel(1);
    deChannel* channelBB = sourceImage.getChannel(2);

    generateFractal(channelRR->getPixels(), channelGG->getPixels(), channelBB->getPixels(), size);

    imageFileName = "delaboratory_test_image";
    onImageNameUpdate();

    */
}

void deProject::resetLayerStack(deColorSpace colorSpace)
{
    logInfo("reset layer stack");

    layerProcessor.removeAllLayers();

    deBaseLayer* layer = createLayer("original", -1, colorSpace, layerStack, previewChannelManager, viewManager, sourceImage);

    if (layer)
    {
        operationProcessor.addNewLayerOnTop(layer, 0);
    }        

    previewChannelManager.destroyAllChannels();
    layerProcessor.updateAllImages(true);

    updateLayerGrid();

    setLastView();

}

deChannelManager& deProject::getPreviewChannelManager() 
{
    return previewChannelManager;
}

deSize deProject::getSourceImageSize() 
{
    deSize size = sourceImage.getSize();

    deValue x1;
    deValue y1;
    deValue x2;
    deValue y2;

    zoomManager.getZoom(x1, y1, x2, y2);

    deValue w = size.getW() * (x2 - x1);
    deValue h = size.getH() * (y2 - y1);

    return deSize(w, h);
}

deLayerStack& deProject::getLayerStack()
{
    return layerStack;
}

deLayerProcessor& deProject::getLayerProcessor()
{
    return layerProcessor;
}

void deProject::onChangeView(int a)
{
    logInfo("change view from " + str(a) + " START");
    layerProcessor.onChangeView(a);

    if (controlPanel)
    {
        controlPanel->onChangeView();
    }
    if (viewModePanel)
    {
        viewModePanel->updateNames();
    }
    if (histogramModePanel)
    {
        histogramModePanel->updateNames();
    }
    updateMemoryInfo();
    mainWindow.rebuild();
    logInfo("change view from " + str(a) + " DONE");
}

const deViewManager& deProject::getViewManager() const
{
    return viewManager;
}

deViewManager& deProject::getViewManager() 
{
    return viewManager;
}

bool deProject::exportFinalImage(const std::string& app, const std::string& type, const std::string& name, wxProgressDialog* progressDialog, bool saveAll, const std::string& dir)
{
    // name is taken from file dialog, it can be empty when we are exporting to external editor
    // but in this case we need correct imageFileName
    if ((name == "") && (imageFileName == ""))
    {
        wxMessageBox( _T("exporting final image failed - no file name set"));
        return false;
    }

    std::string fileName;
    
    if (name == "")
    {
        // path is a directory for temporary save, used only when exporting to external editor
        std::string path = getTmp();

        if (dir.size() > 0)
        {
            // now used also when exporting all images into one dir
            path = dir;
        }

        // we save file in the temporary directory
        fileName = path + "/" + imageFileName + "." + type;
    }        
    else
    {
        // wa save file in the location taken from file dialog
        fileName = name;
    }

    // remember original size of preview
    deSize originalSize = previewChannelManager.getChannelSizeFromChannelManager();

    // calculate final image in full size
    int view = viewManager.getView();

    previewChannelManager.setChannelSize(sourceImage.getSize());

    bool result = layerProcessor.updateImagesSmart(view, progressDialog, memoryInfoFrame, fileName, type, saveAll);

    if (!result)
    {
        wxMessageBox( _T("exporting final image failed - error during update images\n(probably out of memory)"));
    }

    // bring back original size of preview
    previewChannelManager.setChannelSize(originalSize);

    if (result)
    {
        // execute external editor
        if (app.size() > 0)
        {
            executeExternalEditor(fileName, app);
        }
    }

    // calculate image in preview size to continue editing
    layerProcessor.updateAllImages(true);

    return result;
}

void deProject::setLastView()
{
    int n = layerStack.getSize();
    n--;
    viewManager.setView(n);
}

void deProject::setViewModePanel(deViewModePanel* _viewModePanel)
{
    viewModePanel = _viewModePanel;
}

void deProject::setHistogramModePanel(deHistogramModePanel* _histogramModePanel)
{
    histogramModePanel = _histogramModePanel;
}


void deProject::setControlPanel(deControlPanel* _controlPanel)
{
    controlPanel = _controlPanel;
}

void deProject::onChangeViewMode()
{
    if (viewModePanel)
    {
        viewModePanel->updateMode();
    }
}

void deProject::save(const std::string& fileName, bool image)
{
    std::string f = fileName;

    size_t pos = f.rfind(".delab");
    if (pos != f.size() - 6) 
    {
        f += ".delab";
    }

    xmlDocPtr doc = xmlNewDoc(BAD_CAST("1.0"));

    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST("project"));
    xmlDocSetRootElement(doc, root);

    {
        xmlNodePtr child = xmlNewChild(root, NULL, BAD_CAST("layer_stack"), NULL);
        layerStack.save(child);

        if (image)
        {
            saveChild(root, "original", sourceImageFileName);
        }
    }

    xmlSaveFormatFile (f.c_str(), doc, 1); 
}

void deProject::loadLayer(xmlNodePtr root, int layerIndex)
{
    xmlNodePtr child = root->xmlChildrenNode;

    std::string type = "";
    std::string name = "";
    int source = -1;
    deColorSpace colorSpace = deColorSpaceInvalid;

    while (child)
    {
        if ((!xmlStrcmp(child->name, BAD_CAST("source")))) 
        {
            source = getInt(getContent(child));
        }

        if ((!xmlStrcmp(child->name, BAD_CAST("color_space")))) 
        {
            colorSpace = colorSpaceFromString(getContent(child));
        }

        if (type == "")
        // this is fix for multiple "type" node in single layer
        {
            if ((!xmlStrcmp(child->name, BAD_CAST("type")))) 
            {
                type = getContent(child);
            }
        }
        
        if ((!xmlStrcmp(child->name, BAD_CAST("name")))) 
        {
            name = getContent(child);
        }

        child = child->next;
    }
       
    deBaseLayer* layer = createLayer(type, source, colorSpace, layerStack, previewChannelManager, viewManager, sourceImage);

    if (layer)
    {
        operationProcessor.addNewLayerOnTop(layer, layerIndex);
        layer->load(root);
    }        


}

void deProject::loadLayers(xmlNodePtr root)
{
    layerStack.clear();

    xmlNodePtr child = root->xmlChildrenNode;

    int layerIndex = 0;

    while (child)
    {
        if ((!xmlStrcmp(child->name, BAD_CAST("layer")))) 
        {
            loadLayer(child, layerIndex);
            layerIndex++;
        }

        child = child->next;
    }

    previewChannelManager.destroyAllChannels();
    layerProcessor.updateAllImages(true);
}

void deProject::open(const std::string& fileName, bool image)
{
    xmlDocPtr doc = xmlParseFile(fileName.c_str());

    if (!doc)
    {
        return;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);

    if (!root)
    {
        return;
    }

    xmlNodePtr child = root->xmlChildrenNode;

    std::string imageFile = "";

    while (child)
    {
        if ((!xmlStrcmp(child->name, BAD_CAST("layer_stack")))) 
        {
            loadLayers(child);
        }

        if ((!xmlStrcmp(child->name, BAD_CAST("original")))) 
        {
            imageFile = getContent(child);
        }

        child = child->next;
    }

    if (image)
    {
        if (!openImage(imageFile, true, deColorSpaceProPhoto)) 
        {
            openImage(imageFile, false, deColorSpaceRGB);
        }
    }        

    setLastView();
    updateLayerGrid();
}

void deProject::newProject()
{
    resetLayerStack(deColorSpaceRGB);
    setLastView();
    updateLayerGrid();
}

void deProject::setImageAreaPanel(deImageAreaPanel* _imageAreaPanel)
{
    imageAreaPanel = _imageAreaPanel;
}

bool deProject::openImage(const std::string& fileName, bool raw, deColorSpace colorSpace)
{

    std::string info;

    freeImage();

    logInfo("open image " + fileName);

    deColorSpace oldColorSpace = sourceImage.getColorSpace();

    if (raw)
    { 
        bool valid = isRawValid(fileName);
        if (!valid)
        {
            logError("not a valid RAW " + fileName);
            return false;
        }

        info = getRawInfo(fileName);

        if (info.size() == 0)
        {
            logError("empty RAW info in " + fileName);
            return false;
        }

        layerProcessor.sendInfoEvent(DE_DCRAW_START);
        if (rawModule.loadRAW(fileName, sourceImage, colorSpace, true))
        {
            logInfo("found RAW " + fileName);
            bool failure = false;
            while (!rawModule.updateRawLoading(failure))
            {
                wxThread::Sleep(200);
                if (failure)
                {
                    logError("failed RAW load " + fileName);
                    layerProcessor.sendInfoEvent(DE_DCRAW_END);
                    return false;
                }
            }
            bool result = rawModule.loadRAW(fileName, sourceImage, colorSpace, false);
            if (!result)
            {
                return false;
            }

            mainWindow.startRawTimer();

        }
        else
        {
            logError("failed RAW " + fileName);
            layerProcessor.sendInfoEvent(DE_DCRAW_END);
            return false;
        }
    }
    else
    {
        logInfo("trying JPEG/TIFF image " + fileName);
        if (!loadImage(fileName, sourceImage, colorSpace))
        {
            logInfo("JPEG/TIFF image also failed");
            return false;
        }
    }            

    sourceImage.setInfo(info);

    imageFileName = removePathAndExtension(fileName);
    onImageNameUpdate();
    sourceImageFileName = fileName;

    previewChannelManager.destroyAllChannels();
    if (imageAreaPanel)
    {
        imageAreaPanel->updateSize(true);
    }        
    layerProcessor.updateAllImages(true);

    deColorSpace newColorSpace = sourceImage.getColorSpace();

    if (oldColorSpace != newColorSpace)
    {
        resetLayerStack(newColorSpace);
        onChangeView(0);
    }

    return true;
}

void deProject::openMemoryInfoFrame(wxWindow* parent)
{
    if (!memoryInfoFrame)
    {
        memoryInfoFrame = new deMemoryInfoFrame(parent, *this);
        memoryInfoFrame->Show();
    }
}

void deProject::closeMemoryInfoFrame()
{
    memoryInfoFrame = NULL;
}

void deProject::updateMemoryInfo()
{
    if (memoryInfoFrame)
    {
        memoryInfoFrame->update();
    }
}

bool deProject::isSourceValid() const
{
    return (!previewChannelManager.isImageEmpty());
}

void deProject::addActionLayer(const std::string& action)
{
    int layerIndex = layerStack.getSize();

    int s = viewManager.getView();

    deBaseLayer* vLayer = layerStack.getLayer(s);

    if (!vLayer)
    {
        return;
    }

    deColorSpace colorSpace = vLayer->getColorSpace();

    std::string actionDescription = getActionDescription(action);

    logInfo("creating action layer: " + action);

    deBaseLayer* layer = createLayer(action, s, colorSpace, layerStack, previewChannelManager, viewManager, sourceImage);

    if (layer)
    {
        operationProcessor.addNewLayerOnTop(layer, layerIndex);
        setLastView();
        updateLayerGrid();
    }
}    

void deProject::addConversionLayer(deColorSpace colorSpace)
{
    int layerIndex = layerStack.getSize();
    int s = viewManager.getView();

    std::string name = getColorSpaceName(colorSpace);

    logInfo("creating conversionlayer: " + name);

    deBaseLayer* layer = createLayer("conversion", s, colorSpace, layerStack, previewChannelManager, viewManager, sourceImage);

    if (layer)
    {
        operationProcessor.addNewLayerOnTop(layer, layerIndex);
        setLastView();
        updateLayerGrid();
        
    }
}        

void deProject::onImageNameUpdate()
{
    mainWindow.setImageName(imageFileName, sourceImage.getSize());
}

void deProject::onTimerUpdate()
{
    bool failure = false;

    bool result = rawModule.updateRawLoading(failure);

    if ((result) || (failure))
    {
        layerProcessor.sendInfoEvent(DE_DCRAW_END);
        mainWindow.stopRawTimer();
    }

    if (result)
    {
        mainWindow.postEvent(DE_IMAGE_LOAD_EVENT, 0);
    }

}

void deProject::updateLayerGrid()
{
    if (controlPanel)
    {
        controlPanel->updateLayerGrid2();
    }        
}
