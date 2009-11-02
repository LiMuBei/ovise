#pragma once
#ifndef DOTSCENE_BASE_CONFIGURATION_H_
#define DOTSCENE_BASE_CONFIGURATION_H_

// Include WX
#include <wx/string.h>
#include <wx/filename.h>

// Solution's includes
#include "../OViSEAux/UniqueNameManager.h"
#include "../OViSEAux/OViSELogging.h"
#include "../OViSEAux/SceneNodeOffset.h"

// Include Ogre
#include "Ogre.h"

class DotSceneBaseConfiguration // TODO: Parameters -> integrity-check, range-check
{
public:
	DotSceneBaseConfiguration(	OViSELogging *Log,
								UniqueNameManager *PrototypeNameMgr,
								wxString SceneManagerName,
								wxString UniqueNameOfDotSceneManager,
								wxString UniqueNameOfAssociatedResourceGroup);
	~DotSceneBaseConfiguration(void);

	bool doAttachNodes;
	bool doAttachExternals;
	bool doAttachEnvironment;

	wxFileName LocationOfMeshFiles;

	bool doExportNotSelectedChildToo;
	bool doExportMeshFiles;
	bool doOverwriteWhileExport;

	OViSELogging *Log;

	UniqueNameManager *PrototypeNameMgr;

	wxString SceneManagerName;
	wxString UniqueNameOfDotSceneManager;
	wxString UniqueNameOfAssociatedResourceGroup;

	SceneNodeOffset SceneOffset;

	void SetUniqueName(wxString UniqueNameOfDotSceneManager);
	void SetUniqueNameOfOwnedResourceGroup(wxString UniqueNameOfOwnedResourceGroup);
};

#endif /*DOTSCENE_BASE_CONFIGURATION_H_*/