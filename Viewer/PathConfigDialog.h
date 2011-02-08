#ifndef OVISE_PATH_CONFIG_DLG_H
#define OVISE_PATH_CONFIG_DLG_H

/**
   @file
   Subclass of PathConfigDialog, which is generated by wxFormBuilder.
 */

#include "MainFrameBase.h"

class OViSEPathConfigDialog : public PathConfigDialog
{
public:
	/** Constructor */
	OViSEPathConfigDialog( wxString  PluginPath,
	                       wxString  MediaPath,
	                       wxWindow* parent );
	~OViSEPathConfigDialog( void );

	wxString GetPluginPath();
	wxString GetMediaPath();
};

#endif // OVISE_PATH_CONFIG_DLG_H
