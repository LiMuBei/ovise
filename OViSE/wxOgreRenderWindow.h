#ifndef WXOGRERENDERWINDOW_H_INCLUDED
#define WXOGRERENDERWINDOW_H_INCLUDED

#include <wx/wx.h>
#include <Ogre.h>
#include "OViSEInputHandler.h"

/** wxWidgets Ogre render window widget.
	Strongly based on the existing wxOgre widget implementation, this one
	isolates the wx component from Ogre, acting as a simple bind to render
	inside a wxWidgets window.

	@author Jesús Alonso Abad 'Kencho', Other contributors (original wxOgre).
 */
class wxOgreRenderWindow : public wxControl {
	DECLARE_CLASS (wxOgreRenderWindow)
	DECLARE_EVENT_TABLE ()
// Type definitions ------------------------------------------------------------
	public:
		/** Mouse events callback type.
			Any static function with the signature of a mouse events handler does the work.
		 */
		typedef void (*MouseEventsCallback)(wxMouseEvent &);

// Attributes ------------------------------------------------------------------
	protected:
		/// A shared reference to the Ogre root.
		static Ogre::Root *msOgreRoot;

		/// This control's own render window reference.
		Ogre::RenderWindow *mRenderWindow;

		// Pointer to the camera associated with this view
		Ogre::Camera *mCamera;

		/// Timer to sync the rendering to a "constant" frame rate.
		wxTimer *mRenderTimer;

		/// Pointer to status bar for FPS
		wxStatusBar *mStatusBar;

		/// The Id of the next render window
		static unsigned int msNextRenderWindowId;

		// Registered callbacks
		/// Callback for mouse events.
		MouseEventsCallback mMouseEventsCallback;

		/// For handling of mouse and keyboard input
		OViSEInputHandler *mInputHandler;

// Methods ---------------------------------------------------------------------
	public:
		/** wx-like Constructor.
		    @param cam Camera to use for this render window
			@param parent The parent wxWindow component.
			@param id The control id.
			@param pos The default position.
			@param size The default size.
			@param style The default style for this component.
			@param validator A default validator for the component.
		 */
		wxOgreRenderWindow (Ogre::Camera *cam, wxWindow *parent, wxWindowID id,
				const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
				long style = wxSUNKEN_BORDER, const wxValidator &validator = wxDefaultValidator);

		/** Default constructor.
			Allows the "standard" wxWidgets' two-step construction.
		 */
		wxOgreRenderWindow ();

		/** Creation method (for the two-step construction).
			@param parent The parent wxWindow component.
			@param id The control id.
			@param pos The default position.
			@param size The default size.
			@param style The default style for this component.
			@param validator A default validator for the component.
		 */
		bool Create (wxWindow *parent, wxWindowID id,
				const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
				long style = wxSUNKEN_BORDER, const wxValidator &validator = wxDefaultValidator);

		/** Virtual destructor.
		 */
		virtual ~wxOgreRenderWindow ();

		/** Initialisation method.
		 */
		virtual void Init ();

		/** Overrides the default implementation.
			This override is here for convenience. Returns a symbolic 320x240px size.
			@return A size of 320x240 (just a symbolic 4:3 size).
		 */
		virtual wxSize DoGetBestSize () const;

		/** Gets the current Ogre root reference associated.
			@return The current reference to Ogre's Root.
		 */
		static Ogre::Root *GetOgreRoot ();

		/** Sets the Ogre root for all the wxOgreRenderWindow instances.
			@param root The new OgreRoot.
		 */
		static void SetOgreRoot (Ogre::Root *root);

		/** Sets the camera associated with this view
		  */
		void SetCamera(Ogre::Camera *cam);
		Ogre::Camera* GetCamera();

		/** Gets the associated Ogre render window.
			@return The render window used to paint this control.
		 */
		Ogre::RenderWindow *GetRenderWindow () const;

		/** Sets the render timer period.
			@param period The number of milliseconds before the next notification.
				A negative or zero value will stop the timer.
		 */
		void SetRenderTimerPeriod (int period);

		/** Set status bar for FPS information
		 */
		void SetStatusBar(wxStatusBar *sBar);

		/** Sets the mouse events callback.
			@param callback The callback function.
		 */
		void SetMouseEventsCallback (MouseEventsCallback callback);

		/** Updating function.
		 */
		virtual void Update ();

		/** Painting event callback.
			@param evt Data regarding the painting event.
		 */
		virtual void OnPaint (wxPaintEvent &evt);

		/** Render timer event callback.
			@param evt Data regarding the timer event.
		 */
		virtual void OnRenderTimer (wxTimerEvent &evt);

		/** Resizing events callback.
			@param evt Data regarding the resize event.
		 */
		virtual void OnSize (wxSizeEvent &evt);

		/** Mouse events callback.
			@remarks Note this will call the specified callback function to process
				the event.
			@param evt Data regarding the mouse event.
		 */
		virtual void OnMouseEvents (wxMouseEvent &evt);

		/** Keyboard events callback.
		 */
		virtual void OnKeyEvents(wxKeyEvent &evt);

	protected:
		/** Creates an Ogre render window for this widget.
		 */
		virtual void CreateRenderWindow ();

		/** Gets the handle for the render window.
			@return The render window handle.
		 */
		virtual Ogre::String GetOgreHandle () const;

};

#endif // WXOGRERENDERWINDOW_H_INCLUDED
