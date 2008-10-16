#ifndef __OViSEAddMeshDialog__
#define __OViSEAddMeshDialog__

/**
@file
Subclass of AddMeshDialog, which is generated by wxFormBuilder.
*/

#include "GUIFrame.h"
#include "wxOgreRenderWindow.h"
#include "OViSESceneHandling.h"

class MeshDialogFrameListener : public Ogre::FrameListener
{
public:
	MeshDialogFrameListener(Ogre::SceneManager *scnMgr);
	~MeshDialogFrameListener();

protected:
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameEnded(const Ogre::FrameEvent& evt);

	Ogre::SceneManager *mSceneManager;
};



/** Implementing AddMeshDialog */
class OViSEAddMeshDialog : public AddMeshDialog
{
protected:
	// Handlers for AddMeshDialog events.
	void OnCloseDialog( wxCloseEvent& event );
	void OnMeshListSelect( wxCommandEvent& event );
	void OnOkClick( wxCommandEvent& event );
	void OnApplyClick( wxCommandEvent& event );
	void OnCancelClick( wxCommandEvent& event );

	void updateMeshList();
	
public:
	/** Constructor */
	OViSEAddMeshDialog( wxWindow* parent, wxWindowID id);
	virtual ~OViSEAddMeshDialog();

private:
	Ogre::SceneManager *mSceneMgr;
	OViSESceneHandling *mSceneHandler;
	wxOgreRenderWindow *mRenderWin;
	Ogre::Camera *mCam;
	Ogre::Light *mLight;
	Ogre::SceneNode *mMeshNode;

	MeshDialogFrameListener *mListener;

	wxString lastSelected;
};

#endif // __OViSEAddMeshDialog__
