#pragma once

/**
@file
Subclass of PathConfigDialog, which is generated by wxFormBuilder.
*/

#include "MainFrameBase.h"

class OViSEPathConfigDialog : public PathConfigDialog
{
public:
	/** Constructor */
	OViSEPathConfigDialog(wxString CmdPath, wxString BasePath, wxString MediaPath, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Configure path settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
	~OViSEPathConfigDialog(void);

	wxString GetCmdPath();
	wxString GetBasePath();
	wxString GetMediaPath();
};
