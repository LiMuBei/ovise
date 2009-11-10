/***************************************************************
 * Name:      OViSEWxMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Alexander Kasper (akasper@ira.uka.de)
 * Created:   2008-09-11
 * Copyright: Alexander Kasper (http://i61www.ira.uka.de/users/akasper)
 * License:
 **************************************************************/

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

	mWindowManager = new wxAuiManager(this);

	wxOgreRenderWindow::SetOgreRoot(ogreRoot);
	mRoot = ogreRoot;

	mMainRenderWin = new wxOgreRenderWindow(NULL, NULL, this, WINDOW_MainRender);
	mWindowManager->AddPane(mMainRenderWin, wxCENTER, wxT("RenderWindow"));
	mMainRenderWin->SetStatusBar(statusBar);

	mMainRenderWin->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler( OViSEWxFrame::OnViewClick ), NULL, this);

    finishOgreInitialization();
	setupObjectProperties();

	wxImageList *sceneTreeImageList = new wxImageList(16, 16, true, 5);
	loadSceneTreeImageList(sceneTreeImageList);

	mSceneTree = new OViSESceneTree(mSceneHdlr->getSceneManager(), this, SCENETREE, wxDefaultPosition, wxSize(300, -1), wxTR_EDIT_LABELS | wxTR_MULTIPLE | wxTR_DEFAULT_STYLE);
	mWindowManager->AddPane(mSceneTree, wxRIGHT, wxT("Scene structure"));
	mSceneTree->SetImageList(sceneTreeImageList);
	mSceneTree->initTree();

	logBox = new wxListBox(this, wxID_ANY);
	mWindowManager->AddPane(logBox, wxBOTTOM, wxT("Log"));
	mLogBoxListener = new OViSELogListener(logBox);

	//mAddMeshDialog = NULL;

	Ogre::LogManager::getSingletonPtr()->getDefaultLog()->addListener(mLogBoxListener);

	this->Maximize(true);

	mWindowManager->Update();

	// When selection in OViSESceneTree changed, call OViSEWxFrame::OnSelectionChanged(...) !
	this->mSceneTree->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( OViSEWxFrame::OnTreeSelectionChanged ), NULL, this);
	// OViSEWxFrame::OnViewClick handles the other direction

	// Parking call here for a short time whil REFACTORING. H.R.
	// Initialize DotSceneManager
	this->mDotSceneMgr = new OViSEDotSceneManager(OViSEDotSceneManager::CreateDefaultConfiguration(ToWxString("StandardFactory"), OgreAPIMediator::GetSingletonPtr()->GetActiveSceneManager().UniqueName()));
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

	mCam->setPosition(Ogre::Vector3(0,0,20));
	// Look back along -Z
	mCam->lookAt(Ogre::Vector3::ZERO);
    mCam->setNearClipDistance(5);

    mCam->setFixedYawAxis(true);
	mCam->setQueryFlags(0x01);

	Ogre::SceneNode *camFocusNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("mainCamFocusNode");
	camFocusNode->setFixedYawAxis(true);
	Ogre::SceneNode *camNode = camFocusNode->createChildSceneNode("CamNode");
	camNode->setFixedYawAxis(true);
	camNode->setPosition(0, 10, 20);
	camNode->attachObject(mCam);

    Ogre::Viewport *mVp = mMainRenderWin->GetRenderWindow()->addViewport(mCam);
	mMainRenderWin->SetCamera(mCam, camFocusNode);

    Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();

	//mSceneHdlr->createDefaultScene();

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    mCam->setAutoAspectRatio(true);
}

void OViSEWxFrame::setupObjectProperties()
{
	mObjectProperties = new wxPropertyGrid(this, PGID, wxDefaultPosition, wxSize(300, -1));
	mWindowManager->AddPane(mObjectProperties, wxRIGHT, wxT("Properties"));
	this->Connect(PGID, wxEVT_PG_CHANGED, wxPropertyGridEventHandler(OViSEWxFrame::OnPropertyChange));
	mObjectProperties->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
	
	mObjectProperties->Append(new wxPropertyCategory(wxT("Node Properties")));
	mObjectProperties->Append(new wxStringProperty(wxT("Name"), wxT("NodeName")));
	mObjectProperties->SetPropertyValidator(wxT("NodeName"), wxTextValidator(wxFILTER_ALPHANUMERIC));

	wxPGProperty* tID = mObjectProperties->Append(new wxStringProperty(wxT("Translation"), wxPG_LABEL, wxT("<composed>")));
	mObjectProperties->AppendIn(tID, new wxFloatProperty(wxT("x"), wxT("tx")));
	mObjectProperties->SetPropertyValidator(wxT("Translation.tx"), wxTextValidator(wxFILTER_NUMERIC));
	mObjectProperties->AppendIn(tID, new wxFloatProperty(wxT("y"), wxT("ty")));
	mObjectProperties->SetPropertyValidator(wxT("Translation.ty"), wxTextValidator(wxFILTER_NUMERIC));
	mObjectProperties->AppendIn(tID, new wxFloatProperty(wxT("z"), wxT("tz")));
	mObjectProperties->SetPropertyValidator(wxT("Translation.tz"), wxTextValidator(wxFILTER_NUMERIC));

	wxPGProperty* rID = mObjectProperties->Append(new wxStringProperty(wxT("Rotation"), wxPG_LABEL, wxT("<composed>")));
	mObjectProperties->AppendIn(rID, new wxFloatProperty(wxT("x"), wxT("rx")));
	mObjectProperties->SetPropertyValidator(wxT("Rotation.rx"), wxTextValidator(wxFILTER_NUMERIC));
	mObjectProperties->AppendIn(rID, new wxFloatProperty(wxT("y"), wxT("ry")));
	mObjectProperties->SetPropertyValidator(wxT("Rotation.ry"), wxTextValidator(wxFILTER_NUMERIC));
	mObjectProperties->AppendIn(rID, new wxFloatProperty(wxT("z"), wxT("rz")));
	mObjectProperties->SetPropertyValidator(wxT("Rotation.rz"), wxTextValidator(wxFILTER_NUMERIC));

	wxPGProperty* sID = mObjectProperties->Append(new wxStringProperty(wxT("Scale"), wxPG_LABEL, wxT("<composed>")));
	mObjectProperties->AppendIn(sID, new wxFloatProperty(wxT("x"), wxT("sx")));
	mObjectProperties->SetPropertyValidator(wxT("Scale.sx"), wxTextValidator(wxFILTER_NUMERIC));
	mObjectProperties->AppendIn(sID, new wxFloatProperty(wxT("y"), wxT("sy")));
	mObjectProperties->SetPropertyValidator(wxT("Scale.sy"), wxTextValidator(wxFILTER_NUMERIC));
	mObjectProperties->AppendIn(sID, new wxFloatProperty(wxT("z"), wxT("sz")));
	mObjectProperties->SetPropertyValidator(wxT("Scale.sz"), wxTextValidator(wxFILTER_NUMERIC));

	mObjectProperties->Append(new wxPropertyCategory(wxT("Mesh Properties")));
	mObjectProperties->Append(new wxStringProperty(wxT("Name"), wxT("MeshName")));
	mObjectProperties->SetPropertyValidator(wxT("MeshName"), wxTextValidator(wxFILTER_ALPHANUMERIC));
	mObjectProperties->Append(new wxStringProperty(wxT("Material"), wxT("MeshMaterial")));
	mObjectProperties->SetPropertyValidator(wxT("MeshMaterial"), wxTextValidator(wxFILTER_ALPHANUMERIC));

	mObjectProperties->SetSplitterLeft(true);
}

void OViSEWxFrame::loadSceneTreeImageList(wxImageList *list)
{
	// index 0 = ROOT
	wxBitmap rootIcon(wxT("../Media/data/home_16.png"), wxBITMAP_TYPE_PNG);
	list->Add(rootIcon);
	// index 1 = SCENE_NODE
	wxBitmap nodeIcon(wxT("../Media/data/folder_16.png"), wxBITMAP_TYPE_PNG);
	list->Add(nodeIcon);
	// index 2 = MESH
	wxBitmap meshIcon(wxT("../Media/data/box_16.png"), wxBITMAP_TYPE_PNG);
	list->Add(meshIcon);
	// index 3 = CAMERA
	wxBitmap cameraIcon(wxT("../Media/data/camera_16.png"), wxBITMAP_TYPE_PNG);
	list->Add(cameraIcon);
	// index 4 = LIGHT
	wxBitmap lightIcon(wxT("../Media/data/lightbulb_16.png"), wxBITMAP_TYPE_PNG);
	list->Add(lightIcon);
}

void OViSEWxFrame::OnClose(wxCloseEvent &event)
{
	for(std::map<std::string, wxFrame*>::iterator i = mViewWindows.begin(); i != mViewWindows.end(); i++)
	{
		(*i).second->Close();
	}
    Destroy();

	OViSESceneHandling::release();
}

void OViSEWxFrame::OnQuit(wxCommandEvent &event)
{
	for(std::map<std::string, wxFrame*>::iterator i = mViewWindows.begin(); i != mViewWindows.end(); i++)
	{
		(*i).second->Close();
	}
    Destroy();

	OViSESceneHandling::release();
}

void OViSEWxFrame::OnAbout(wxCommandEvent &event)
{
    wxAboutDialogInfo info;
    info.SetName(wxT("OViSE"));
    info.SetVersion(wxT("0.2 Beta (orc)"));

	wxString description = wxT("Institute of Computer Science and Engineering (CSE)\n\r");
	description += wxT("Industrial Applications of Computer Science and Micro Systems (IAIM)\n");
	description += wxT("Prof. Dr. R. Dillmann\n");
	description += wxT("Department of Computer Science\n");
	description += wxT("Karlsruhe Institute of Technology (KIT)\n");
	description += wxT("Ogre Framework for scene visualization. Uses Ogre3D (http://www.ogre3d.org)");
	info.SetDescription(description);

    info.SetCopyright(wxT("(C) 2008-2009 "));

	info.AddDeveloper(wxT("Programming - Alexander Kasper <akasper@ira.uka.de>"));
	info.AddDeveloper(wxT("Programming - Henning Renartz <hrenart@gmx.de>"));

	wxString licenseText = wxT("Permission is hereby granted, free of charge,");
	licenseText += wxT("to any person obtaining a copy of this software and associated documentation files (the \"Software\"), ");
	licenseText += wxT("to deal in the Software without restriction, including without limitation the rights to use, copy, modify, ");
	licenseText += wxT("merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the ");
	licenseText += wxT("Software is furnished to do so, subject to the following conditions:\n\n");
	licenseText += wxT("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\n");
	licenseText += wxT("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED ");
	licenseText += wxT("TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE ");
	licenseText += wxT("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, ");
	licenseText += wxT("TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
	info.SetLicense(licenseText);

	info.SetWebSite(wxT("http://code.google.com/p/ovise/"));

    wxAboutBox(info);
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
	Ogre::SceneNode *camFocusNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	camFocusNode->setFixedYawAxis(true);
	Ogre::SceneNode *camNode = camFocusNode->createChildSceneNode();
	camNode->setFixedYawAxis(true);
	camNode->setPosition(0, 0, 10);
	camNode->attachObject(newCam);
    newCam->setNearClipDistance(5);
	newCam->setQueryFlags(0x01);

	// Need to create a new Frame to display the new renderwindow in
	wxFrame *newFrame = new wxFrame(this, wxID_ANY, wxCamName);
	wxOgreRenderWindow *newRenderWin = new wxOgreRenderWindow(newCam, camFocusNode, newFrame, wxID_ANY);
	newRenderWin->SetOgreRoot(mRoot);
	mViewWindows[std::string(wxCamName.ToAscii())] = newFrame;

	newRenderWin->GetRenderWindow()->addViewport(newCam);
	newRenderWin->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler( OViSEWxFrame::OnViewClick ), NULL, this);
	newCam->setAutoAspectRatio(true);

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
	/*
	if(mAddMeshDialog == NULL)
	{
		mAddMeshDialog = new OViSEAddMeshDialog(this, wxID_HIGHEST + 1);
		mAddMeshDialog->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler( OViSEWxFrame::OnAddMeshDialogClose ), NULL, this);
	}
	mAddMeshDialog->Show();
	*/
}

void OViSEWxFrame::OnAddMeshDialogClose(wxCloseEvent& event)
{
	event.Skip();
	//mAddMeshDialog = NULL;
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

	// REACTORING of "OViSESceneHandling" takes affect here! //

	Ogre::Camera *cam = win->GetCamera();
	Ogre::MovableObject *selectedObject = mSceneHdlr->getSelectedObject(sx, sy, d, cam, cam->getSceneManager()->getName());
	QualifiedNameCollection QNames = OgreAPIMediator::GetSingletonPtr()->GetQueryObjects(sx, sy, cam, OgreAPIMediator::GetSingletonPtr()->GetActiveSceneManager());
	
	if(!QNames.IsEmpty())
	{
		// CASE:				ACTION:
		// 1:	-			->	Unselect all, Select one or nothing
		// 2:	CTRL		->	Unselect one or nothing, Add one Selection(front)
		// 3:	SHIFT		->	Unselect all, Add complete Query
		// 4:	CTRL, SHIFT ->	Unselect all in complete Query, Add complete Query < IRREGULAR: only unselect, if all from Query are selected

		if (event.ControlDown())
		{
			if (event.ShiftDown())
			{
				// CASE 4
			}
			else
			{
				// CASE 2
			}
		}
		else
		{
			if (event.ShiftDown())
			{
				// CASE 3: Unselect all, Add complete Query
				SelectionManager::getSingletonPtr()->Selection.Clear();
				this->mSceneTree->UnselectAll();

				if (QNames.GetCount() > 0)
				{
					for (unsigned long IT = 0; IT < QNames.GetCount(); IT++)
					{
						// Add to all selection // Selection is clear, so it's not neccessary to test, if QName is already in there
						SelectionManager::getSingletonPtr()->Selection.Add(QNames[IT]);
						
						// Select in SceneTree-View
						wxTreeItemId Item = this->mSceneTree->Items[QNames[IT].UniqueName()];
						this->mSceneTree->SelectItem(Item);
					}
				}
			}
			else
			{
				// CASE 1: Unselect all, Select one or nothing
				SelectionManager::getSingletonPtr()->Selection.Clear();
				this->mSceneTree->UnselectAll();

				if (QNames.GetCount() > 0)
				{
					// Add to first selection // Selection is clear, so it's not neccessary to test, if QName is already in there
					SelectionManager::getSingletonPtr()->Selection.Add(QNames[0]);

					// Select in SceneTree-View
					wxTreeItemId Item = this->mSceneTree->Items[QNames[0].UniqueName()];
					this->mSceneTree->SelectItem(Item);
				}
			}
		}
	}
	/*

		//if(!event.ControlDown()) this->RemoveAllSelectedObjects(); // ????
		wxString ObjectName = ToWxString(selectedObject->getName());
		wxTreeItemId Item = this->mSceneTree->Items[ObjectName];
		if(OViSESelectionManager::getSingletonPtr()->Selection.SelectedObjects.count(ObjectName) == 1/*this->mSceneTree->IsSelected(Item)*//*)
		{
			if(event.ControlDown()) this->mSceneTree->UnselectItem(Item);
			else this->mSceneTree->UnselectAll();
		}
		else
		{
			if(event.ControlDown()) this->mSceneTree->SelectItem(Item);
			else
			{
				wxArrayTreeItemIds selections;
				this->mSceneTree->GetSelections(selections);
				this->mSceneTree->UnselectAll();
				this->mSceneTree->SelectItem(Item);
			}
		}
		//this->ToogleSelectedObject(selectedObject, ToWxString(cam->getSceneManager()->getName()));
	}
	else
	{
		this->mSceneTree->UnselectAll();
	}*/
}

void OViSEWxFrame::OnDynamicShadowsChange(wxCommandEvent& event)
{
	if(event.IsChecked())
	{
		mSceneHdlr->dynamicShadows(true);
	}
	else
	{
		mSceneHdlr->dynamicShadows(false);
	}
}

void OViSEWxFrame::OnPropertyChange(wxPropertyGridEvent& event)
{
	wxPGProperty *prop = event.GetProperty();
	SelectionManager::getSingletonPtr()->HandlePropertyChanged(prop);
	/*
    // It may be NULL
    if ( !prop )
        return;

    // Get name of changed property
    const wxString& name = prop->GetName();
	std::string objname = (const char*)mObjectProperties->GetPropertyValueAsString(wxT("MeshName")).ToAscii();

	OViSESelectionMap selObjs = OViSESceneHandling::getSingletonPtr()->getSelectedObjects();
	if(selObjs.empty())
		return;
	Ogre::SceneNode *snode = selObjs[objname]->getParentSceneNode();
	if(!snode)
		return;
	Ogre::Vector3 pos = snode->getPosition();
	Ogre::Vector3 scale = snode->getScale();
	Ogre::Quaternion tmprot = snode->getOrientation();

	wxVariant tmp = prop->GetValue();
	wxString tmpstr = tmp.GetString();

	double vals[3];

	wxStringTokenizer checker(tmpstr, wxT(";"));

	if(name == wxString(wxT("Translation")))
	{
		for(int i=0; i<3; i++)
		{
			checker.GetNextToken().ToDouble(&vals[i]);
		}
		pos = Ogre::Vector3(vals[0], vals[1], vals[2]);
	}
	if(name == wxString(wxT("Translation.tx")))
	{
		float tx = tmp.GetDouble();
		pos.x = tx;
	}
	if(name == wxString(wxT("Translation.ty")))
	{
		float ty = tmp.GetDouble();
		pos.y = ty;
	}
	if(name == wxString(wxT("Translation.tz")))
	{
		float tz = tmp.GetDouble();
		pos.z = tz;
	}

	snode->setPosition(pos);

	if(name == wxString(wxT("Rotation")))
	{
		for(int i=0; i<3; i++)
		{
			checker.GetNextToken().ToDouble(&vals[i]);
		}
		Ogre::Radian p = Ogre::Radian(Ogre::Degree(vals[0])) - tmprot.getPitch();
		Ogre::Radian r = Ogre::Radian(Ogre::Degree(vals[1])) - tmprot.getRoll();
		Ogre::Radian y = Ogre::Radian(Ogre::Degree(vals[2])) - tmprot.getYaw();
		snode->pitch(p);
		snode->roll(r);
		snode->yaw(y);
	}
	if(name == wxString(wxT("Rotation.rx")))
	{
		float rx = tmp.GetDouble();
		snode->pitch(Ogre::Radian(Ogre::Degree(rx)) - tmprot.getPitch());
	}
	if(name == wxString(wxT("Rotation.ry")))
	{
		float ry = tmp.GetDouble();
		snode->pitch(Ogre::Radian(Ogre::Degree(ry)) - tmprot.getRoll());
	}
	if(name == wxString(wxT("Rotation.rz")))
	{
		float rz = tmp.GetDouble();
		snode->pitch(Ogre::Radian(Ogre::Degree(rz)) - tmprot.getYaw());
	}

	if(name == wxString(wxT("Scale")))
	{
		for(int i=0; i<3; i++)
		{
			checker.GetNextToken().ToDouble(&vals[i]);
		}
		scale = Ogre::Vector3(vals[0], vals[1], vals[2]);
	}
	if(name == wxString(wxT("Scale.sx")))
	{
		float sx = tmp.GetDouble();
		scale.x = sx;
	}
	if(name == wxString(wxT("Scale.sy")))
	{
		float sy = tmp.GetDouble();
		scale.y = sy;
	}
	if(name == wxString(wxT("Scale.sz")))
	{
		float sz = tmp.GetDouble();
		scale.z = sz;
	}

	snode->setScale(scale);

	*/
	// call update for scene structure
	mSceneHdlr->showSceneGraphStructure(true);
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

void OViSEWxFrame::OnMenuDeleteMeshes(wxCommandEvent &event)
{
	deleteMeshes();
}

void OViSEWxFrame::clearObjectProperties()
{
	mObjectProperties->Clear();
}

void OViSEWxFrame::deleteMeshes()
{
	OViSESelectionMap so = OViSESceneHandling::getSingletonPtr()->getSelectedObjects();
	if(so.empty())
	{
		wxMessageDialog dlg(this, wxT("No meshes selected"), wxT("Error"), wxOK | wxCENTRE | wxICON_ERROR);
		dlg.ShowModal();
		return;
	}

	wxMessageDialog confirmationDialog(this, wxT("Delete selected meshes?"), wxT("Confirmation required"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
	if(confirmationDialog.ShowModal() == wxID_YES)
	{
		for(OViSESelectionMap::iterator it=so.begin(); it != so.end(); it++)
		{
			OViSESceneHandling::getSingletonPtr()->deleteMesh(it->first);
		}
	}
}


void OViSEWxFrame::OnTreeSelectionChanged( wxTreeEvent& event )
{
	wxTreeItemId Item = event.GetItem();

	wxString Msg = ToWxString("SELECTED ITEMS:");

	if (!Item.IsOk()) // case : UnselectAll()
	{
		this->RemoveAllSelectedObjects();
	}
	else // case : UnselectItem(...)
	{
		wxString TreeItemLabel = this->mSceneTree->GetItemText(Item);

		Ogre::MovableObject* selectedObject = this->mSceneHdlr->getSelectedObject(TreeItemLabel, this->mSceneMgr);

		this->RemoveAllSelectedObjects();

		wxArrayTreeItemIds SelectedItems;
		this->mSceneTree->GetSelections(SelectedItems);

		wxString SceneManagerName;// = SelectionManager::getSingletonPtr()->Selection.getSceneManagerName();

		
		int count = SelectedItems.Count();
		for(unsigned int IT = 0; IT < SelectedItems.Count(); IT++)
		{
			wxString ItemLabel = this->mSceneTree->GetItemText(SelectedItems[IT]);
			Msg << ToWxString(" '") << ItemLabel << ("' ");
			
			//OViSEOgreEnums::MovableObject::MovableType Type = OViSESelectionManager::getSingletonPtr()->Selection.SelectedObjects[ItemLabel];
			Ogre::MovableObject* SelectedObject = OgreAPIMediator::GetSingletonPtr()->getMovableObjectPtr(
				SceneManagerName,
				ItemLabel);

			if(SelectedObject != NULL)
			{
				this->AddSelectedObject(SelectedObject, ToWxString(this->mMainRenderWin->GetCamera()->getSceneManager()->getName()));
			}
		}
	}

	this->Log.WriteToOgreLog(Msg, Log.Normal);
	/*
	{
		

		//this->ToogleSelectedObject(selectedObject, ToWxString(this->mMainRenderWin->GetCamera()->getSceneManager()->getName()));
		
		
	}*/
}

void OViSEWxFrame::AddSelectedObject(Ogre::MovableObject* selectedObject, wxString SceneManagerName)
{
	/*
	//mSceneHdlr->addObjectToSelection(selectedObject, true, ToStdString(SceneManagerName));
	//setObjectProperties(selectedObject);

	// #Mark:NewSelection#

	wxString ObjectName = ToWxString(selectedObject->getName());

	/SelectionManager::getSingletonPtr()->Selection.setSceneManagerName(SceneManagerName);
	/*if (OViSESelectionManager::getSingletonPtr()->Selection.hasMovableObject(ObjectName))
	{
		// Remove from selection
		OViSESelectionManager::getSingletonPtr()->Selection.removeMovableObject(ObjectName); // Toggle item
		
		// Hide bounding box
		selectedObject->getParentSceneNode()->showBoundingBox(false);
	}
	else
	{*//*
		// Add to selection
		OgreEnums::MovableObject::MovableType Type = OViSEOgreEnums::EnumTranslator_MovableType::GetSingletonPtr()->getStringAsEnum(ToWxString(selectedObject->getMovableType()));
		SelectionManager::getSingletonPtr()->Selection.addMovableObject(ObjectName, Type);
		
		// Show bounding box
		selectedObject->getParentSceneNode()->showBoundingBox(true);
	//}

	SelectionManager::getSingletonPtr()->generatePropertyGridContentFromSelection(this->mObjectProperties);
	*/
}

void OViSEWxFrame::RemoveAllSelectedObjects()
{
	//this->mSceneHdlr->clearObjectSelection(/*cam->getSceneManager()->getName()*/);
	//clearObjectProperties();
	/*
	wxArrayString Keys = SelectionManager::getSingletonPtr()->Selection.getAllMovableObjectNames();
	wxString NameOfSceneManager = OViSESelectionManager::getSingletonPtr()->Selection.getSceneManagerName();
	for (int IT = 0; IT < Keys.Count(); IT++)
	{
		wxString SceneManagerName = OViSESelectionManager::getSingletonPtr()->Selection.getSceneManagerName();
		OViSEOgreEnums::MovableObject::MovableType Type = OViSESelectionManager::getSingletonPtr()->Selection.SelectedObjects[Keys[IT]];

		Ogre::MovableObject* SelectedObject = OgreAPIMediator::GetSingletonPtr()->getMovableObjectPtr(
			OViSESelectionManager::getSingletonPtr()->Selection.getSceneManagerName(),
			Keys[IT],
			Type);

		if(SelectedObject != NULL) SelectedObject->getParentSceneNode()->showBoundingBox(false);
	}
	// #Mark:NewSelection#
	OViSESelectionManager::getSingletonPtr()->Selection.removeAll();

	this->mObjectProperties->Clear();
	*/
}
void OViSEWxFrame::OnOpenPrototypeManagement( wxCommandEvent& event )
{
	OViSEPrototypeManagementDialog *PrototypeManagementDlg = new OViSEPrototypeManagementDialog(this, this->mSceneHdlr);
	//PrototypeManagementDlg->SetAvailablePrototypes(this->mSceneHdlr->GetAvailablePrototypesOfDotSceneManager());
	PrototypeManagementDlg->ShowModal();
	//this->mSceneHdlr->AttachNewScene(AttachSceneDlg->GetResultingUniqueNameOfPrototype());
	delete PrototypeManagementDlg;
}

void OViSEWxFrame::OnLoadPointCloud(wxCommandEvent& event)
{
	wxFileDialog pcFileDialog(this, wxT("Select pointcloud file"), wxEmptyString, wxEmptyString, wxT("*.txt"));
	if(pcFileDialog.ShowModal() == wxID_OK)
	{
		wxString pcName = pcFileDialog.GetFilename();
		pcName.Truncate(pcName.Length() - 4);
		wxString pcEntName = pcName + wxT("Entity");
		wxFileInputStream input(pcFileDialog.GetDirectory() + wxT("\\") + pcFileDialog.GetFilename());
		wxTextInputStream text(input);
		wxString line;
		float t;

		std::vector<float> pointvector;
		int counter = 0, cc = 0;
		while(!input.Eof())
		{
			text >> t;
			pointvector.push_back(t);
			cc++;
			if(cc == 3)
			{
				counter++;
				cc = 0;
			}
		}
		
		float *pointlist = new float[counter*3-1];
		for(int i = 0; i<counter*3-1; i++)
		{
			pointlist[i] = pointvector.at(i);
		}

		OViSEPointcloud *pc = new OViSEPointcloud(std::string(pcName.mb_str()), "General", counter, pointlist, NULL);

		Ogre::SceneManager *scnMgr = OViSESceneHandling::getSingletonPtr()->getSceneManager();

		Ogre::Entity *pcEnt = scnMgr->createEntity(std::string(pcEntName.mb_str()), std::string(pcName.mb_str()));
		pcEnt->setMaterialName("Pointcloud");
		scnMgr->getRootSceneNode()->attachObject(pcEnt);
	}
}

void OViSEWxFrame::OnShowSceneStructure(wxCommandEvent &event)
{
	mSceneHdlr->showSceneGraphStructure();
}


void OViSEWxFrame::OnTestStuff( wxCommandEvent& event )
{
	mSceneHdlr->testStuff();
}

void OViSEWxFrame::OnStartStopFrameListeners(wxCommandEvent& event)
{
	if(event.IsChecked())
		mSceneHdlr->startStopFrameListeners(true);
	else
		mSceneHdlr->startStopFrameListeners(false);
}