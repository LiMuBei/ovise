/***************************************************************
 * Name:      OViSEWxMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Alexander Kasper (akasper@ira.uka.de)
 * Created:   2008-09-11
 * Copyright: Alexander Kasper (http://i61www.ira.uka.de/users/akasper)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "OViSEWxMain.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}


OViSEWxFrame::OViSEWxFrame(wxFrame *frame, Ogre::Root *ogreRoot)
    : GUIFrame(frame)
{
#if wxUSE_STATUSBAR
    statusBar->SetStatusText(_("OViSE started up."), 0);
	statusBar->SetStatusText(wxT("FPS:"), 1);
#endif

	mSplitter = new wxSplitterWindow(this, wxID_ANY);
	mSecondSplitter = new wxSplitterWindow(mSplitter, wxID_ANY);

	wxOgreRenderWindow::SetOgreRoot(ogreRoot);
	mRoot = ogreRoot;
	mMainRenderWin = new wxOgreRenderWindow(mSecondSplitter, WINDOW_MainRender);
	mMainRenderWin->SetStatusBar(statusBar);

	mMainRenderWin->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler( OViSEWxFrame::OnViewClick ), NULL, this);
	
    finishOgreInitialization();
	setupObjectProperties();

	logBox = new wxListBox(mSplitter, wxID_ANY);
	mSplitter->SplitHorizontally(mSecondSplitter, logBox, this->GetSize().GetHeight()*0.85);
	mSecondSplitter->SplitVertically(mMainRenderWin, mObjectProperties, this->GetSize().GetWidth()*0.85);
	mLogBoxListener = new OViSELogListener(logBox);

	mAddMeshDialog = NULL;

	Ogre::LogManager::getSingletonPtr()->getDefaultLog()->addListener(mLogBoxListener);
}

OViSEWxFrame::~OViSEWxFrame()
{
	Ogre::LogManager::getSingletonPtr()->getDefaultLog()->removeListener(mLogBoxListener);
}

void OViSEWxFrame::finishOgreInitialization()
{
	mSceneHdlr = OViSESceneHandling::getSingletonPtr();
	mSceneMgr = mSceneHdlr->getSceneManager();

    mCam = mSceneMgr->createCamera("MainCam");

	// Position it at 500 in Z direction
    mCam->setPosition(Ogre::Vector3(0,10,20));
	// Look back along -Z
	mCam->lookAt(Ogre::Vector3::ZERO);
    mCam->setNearClipDistance(5);

    mCam->setFixedYawAxis(true);

    Ogre::Viewport *mVp = mMainRenderWin->GetRenderWindow()->addViewport(mCam);
	mMainRenderWin->SetCamera(mCam);

    Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();

	mSceneHdlr->createDefaultScene();

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    mCam->setAutoAspectRatio(true);
}

void OViSEWxFrame::setupObjectProperties()
{
	mObjectProperties = new wxPropertyGrid(mSecondSplitter, PGID);
	mObjectProperties->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
	
	mObjectProperties->Append(new wxPropertyCategory(wxT("Node Properties")));
	mObjectProperties->Append(new wxStringProperty(wxT("Name"), wxT("NodeName")));
	wxPGId tID = mObjectProperties->Append(new wxStringProperty(wxT("Translation"), wxPG_LABEL, wxT("<composed>")));
	mObjectProperties->AppendIn(tID, new wxFloatProperty(wxT("x"), wxT("tx")));
	mObjectProperties->AppendIn(tID, new wxFloatProperty(wxT("y"), wxT("ty")));
	mObjectProperties->AppendIn(tID, new wxFloatProperty(wxT("z"), wxT("tz")));
	wxPGId rID = mObjectProperties->Append(new wxStringProperty(wxT("Rotation"), wxPG_LABEL, wxT("<composed>")));
	mObjectProperties->AppendIn(rID, new wxFloatProperty(wxT("x"), wxT("rx")));
	mObjectProperties->AppendIn(rID, new wxFloatProperty(wxT("y"), wxT("ry")));
	mObjectProperties->AppendIn(rID, new wxFloatProperty(wxT("z"), wxT("rz")));
	wxPGId sID = mObjectProperties->Append(new wxStringProperty(wxT("Scale"), wxPG_LABEL, wxT("<composed>")));
	mObjectProperties->AppendIn(sID, new wxFloatProperty(wxT("x"), wxT("sx")));
	mObjectProperties->AppendIn(sID, new wxFloatProperty(wxT("y"), wxT("sy")));
	mObjectProperties->AppendIn(sID, new wxFloatProperty(wxT("z"), wxT("sz")));

	mObjectProperties->Append(new wxPropertyCategory(wxT("Mesh Properties")));
	mObjectProperties->Append(new wxStringProperty(wxT("Name"), wxT("MeshName")));
	mObjectProperties->Append(new wxStringProperty(wxT("Material"), wxT("MeshMaterial")));
}

BEGIN_EVENT_TABLE(OViSEWxFrame, GUIFrame)
	EVT_PG_CHANGED( PGID, OViSEWxFrame::OnPropertyChange )
END_EVENT_TABLE()

void OViSEWxFrame::OnClose(wxCloseEvent &event)
{
	for(std::map<std::string, wxFrame*>::iterator i = mViewWindows.begin(); i != mViewWindows.end(); i++)
	{
		(*i).second->Close();
	}
    Destroy();
}

void OViSEWxFrame::OnQuit(wxCommandEvent &event)
{
	for(std::map<std::string, wxFrame*>::iterator i = mViewWindows.begin(); i != mViewWindows.end(); i++)
	{
		(*i).second->Destroy();
	}
    Destroy();
}

void OViSEWxFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void OViSEWxFrame::OnAddView(wxCommandEvent &event)
{
	int camNo = 0;
	wxString wxCamName;
	wxCamName.Printf(wxT("Camera %i"), camNo);
	while(mSceneMgr->hasCamera(Ogre::String(wxCamName.ToAscii())))
	{
		camNo += 1;
		wxCamName.Printf(wxT("Camera %i"), camNo);
	}
	Ogre::Camera *newCam = mSceneMgr->createCamera(Ogre::String(wxCamName.ToAscii()));
	mAdditionalCameras[Ogre::String(wxCamName.ToAscii())] =  newCam;
	newCam->setPosition(Ogre::Vector3(0,10,20));
	// Look back along -Z
    newCam->setDirection(Ogre::Vector3(-1, 0, 0));
    newCam->setNearClipDistance(5);

	// Need to create a new Frame to display the new renderwindow in
	wxFrame *newFrame = new wxFrame(this, wxID_ANY, wxCamName);
	wxOgreRenderWindow *newRenderWin = new wxOgreRenderWindow(newFrame, wxID_ANY);
	newRenderWin->SetCamera(newCam);
	newRenderWin->SetOgreRoot(mRoot);
	mViewWindows[std::string(wxCamName.ToAscii())] = newFrame;

	newRenderWin->GetRenderWindow()->addViewport(newCam);
	newRenderWin->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler( OViSEWxFrame::OnViewClick ), NULL, this);
	newCam->setAutoAspectRatio(true);

	newFrame->SetSize(50, 50, 640, 480);
	newFrame->Show();
	newFrame->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler( OViSEWxFrame::OnAdditionalViewClose ), NULL, this);
}

void OViSEWxFrame::OnAdditionalViewClose(wxCloseEvent &event)
{
	wxFrame *tmp = dynamic_cast<wxFrame*>(event.GetEventObject());
	std::string name = std::string(tmp->GetTitle().ToAscii());
	mViewWindows.erase(name);
	event.Skip();
}

void OViSEWxFrame::OnSaveScreenToFile(wxCommandEvent &event)
{
	wxFileDialog *fDialog = new wxFileDialog(this, wxT("Save screen to file"), wxGetHomeDir(), wxT("OViSEScreenshot.png"),
		wxT("PNG files (*.png)|*.png"), wxFD_SAVE);
	int rval = fDialog->ShowModal();
	if(rval == wxID_OK)
	{
		wxString fullPath = fDialog->GetPath();
		mMainRenderWin->GetRenderWindow()->writeContentsToFile(Ogre::String(fullPath.ToAscii()));
	}
	fDialog->Destroy();
}

void OViSEWxFrame::OnSceneAddMesh(wxCommandEvent &event)
{
	if(mAddMeshDialog == NULL)
	{
		mAddMeshDialog = new OViSEAddMeshDialog(this, wxID_HIGHEST + 1);
		mAddMeshDialog->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler( OViSEWxFrame::OnAddMeshDialogClose ), NULL, this);
	}
	mAddMeshDialog->Show();
}

void OViSEWxFrame::OnAddMeshDialogClose(wxCloseEvent& event)
{
	mAddMeshDialog->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler( OViSEWxFrame::OnAddMeshDialogClose ), NULL, this);
	if(mAddMeshDialog->Destroy())
		mAddMeshDialog = NULL;
}

void OViSEWxFrame::OnViewClick(wxMouseEvent& event)
{
	wxOgreRenderWindow *win = dynamic_cast<wxOgreRenderWindow*>(event.GetEventObject());
	wxPoint p = event.GetPosition();
	wxPoint t = win->GetScreenPosition();
	int width = win->GetRenderWindow()->getWidth();
	int height = win->GetRenderWindow()->getHeight();
	wxPoint s = p;
	float sx = (float)s.x / (float)width;
	float sy = (float)s.y / (float)height;
	float d = -1;

	Ogre::Camera *cam = win->GetCamera();
	Ogre::MovableObject *selectedObject = mSceneHdlr->getSelectedObject(sx, sy, d, cam, cam->getSceneManager()->getName());
	if(selectedObject != NULL)
	{
		if(!event.ControlDown())
			mSceneHdlr->clearObjectSelection(cam->getSceneManager()->getName());
		mSceneHdlr->addObjectToSelection(selectedObject->getParentSceneNode(), true, cam->getSceneManager()->getName());
		setObjectProperties(selectedObject);
	}
	else
	{
		mSceneHdlr->clearObjectSelection(cam->getSceneManager()->getName());
		clearObjectProperties();
	}
}

void OViSEWxFrame::OnPropertyChange(wxPropertyGridEvent& event)
{
	wxPGProperty *prop = event.GetProperty();

    // It may be NULL
    if ( !prop )
        return;

    // Get name of changed property
    const wxString& name = prop->GetName();

	if(name == wxString(wxT("Translation.tx")))
	{
		float tx = prop->GetValue().GetDouble();
	}
	if(name == wxString(wxT("Translation.ty")))
	{
		float ty = prop->GetValue().GetDouble();
	}

    // Get resulting value
    wxVariant value = prop->GetValue();
}

void OViSEWxFrame::setObjectProperties(Ogre::MovableObject *object)
{
	Ogre::SceneNode *pnode = object->getParentSceneNode();
	mObjectProperties->SetPropertyValue(wxT("NodeName"), wxString(pnode->getName().c_str(), wxConvUTF8));

	mObjectProperties->SetPropertyValue(wxT("Translation.tx"), (float)pnode->getPosition().x);
	mObjectProperties->SetPropertyValue(wxT("Translation.ty"), (float)pnode->getPosition().y);
	mObjectProperties->SetPropertyValue(wxT("Translation.tz"), (float)pnode->getPosition().z);

	mObjectProperties->SetPropertyValue(wxT("Rotation.rx"), (float)pnode->getOrientation().getPitch().valueDegrees());
	mObjectProperties->SetPropertyValue(wxT("Rotation.ry"), (float)pnode->getOrientation().getRoll().valueDegrees());
	mObjectProperties->SetPropertyValue(wxT("Rotation.rz"), (float)pnode->getOrientation().getYaw().valueDegrees());

	mObjectProperties->SetPropertyValue(wxT("Scale.sx"), (float)pnode->getScale().x);
	mObjectProperties->SetPropertyValue(wxT("Scale.sy"), (float)pnode->getScale().y);
	mObjectProperties->SetPropertyValue(wxT("Scale.sz"), (float)pnode->getScale().z);

	mObjectProperties->SetPropertyValue(wxT("MeshName"), wxString(object->getName().c_str(), wxConvUTF8));
	
	mObjectProperties->SetPropertyValue(wxT("MeshMaterial"), wxT("TODO"));
}

void OViSEWxFrame::clearObjectProperties()
{
	mObjectProperties->ClearPropertyValue(wxT("NodeName"));

	mObjectProperties->ClearPropertyValue(wxT("Translation"));
	mObjectProperties->ClearPropertyValue(wxT("Translation.tx"));
	mObjectProperties->ClearPropertyValue(wxT("Translation.ty"));
	mObjectProperties->ClearPropertyValue(wxT("Translation.tz"));
	
	mObjectProperties->ClearPropertyValue(wxT("Rotation"));
	mObjectProperties->ClearPropertyValue(wxT("Rotation.rx"));
	mObjectProperties->ClearPropertyValue(wxT("Rotation.ry"));
	mObjectProperties->ClearPropertyValue(wxT("Rotation.rz"));

	mObjectProperties->ClearPropertyValue(wxT("Scale"));
	mObjectProperties->ClearPropertyValue(wxT("Scale.sx"));
	mObjectProperties->ClearPropertyValue(wxT("Scale.sy"));
	mObjectProperties->ClearPropertyValue(wxT("Scale.sz"));

	mObjectProperties->ClearPropertyValue(wxT("MeshName"));
	mObjectProperties->ClearPropertyValue(wxT("MeshMaterial"));
}