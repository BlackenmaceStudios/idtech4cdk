using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private
{
    class UserInterfaceManager
    {
        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_UserInterface_LoadGui")]
        private static extern IntPtr TOOLAPI_UserInterface_LoadGui(string mapName);

        public static UserInterface LoadGUI(string uiName)
        {
            IntPtr handle = TOOLAPI_UserInterface_LoadGui(uiName);

            if (handle == IntPtr.Zero)
                return null;

            UserInterface ui = new UserInterface();

            ui.AttachToMemory(typeof(UserInterface), handle);

            return ui;
        }
    }
    public class UserInterface : NativeClass
    {
        public delegate void           Deructor_t(IntPtr ptr);

	    public delegate void				Free_t(IntPtr ptr) ;

								        // Returns the name of the gui.
	    public delegate string 		Name_t(IntPtr ptr)  ;

								        // Returns a comment on the gui.
	    public delegate string 		Comment_t(IntPtr ptr)  ;

								        // Returns true if the gui is interactive.
	    public delegate bool				IsInteractive_t(IntPtr ptr)  ;

	    public delegate bool				IsUniqued_t(IntPtr ptr)  ;

	    public delegate void				SetUniqued_t( IntPtr ptr, bool b ) ;
								        // returns false if it failed to load
	    public delegate bool				InitFromFile_t( IntPtr ptr, string qpath, bool rebuild = true, bool cache = true ) ;

								        // handles an event, can return an action string, the caller interprets
								        // any return and acts accordingly
	    public delegate string 		HandleEvent_t( IntPtr ptr, IntPtr ev, int time, IntPtr t  ) ;

								        // handles a named event
	    public delegate void				HandleNamedEvent_t( IntPtr ptr, string eventName ) ;

								        // repaints the ui
	    public delegate void				Redraw_t( IntPtr ptr, int time ) ;

								        // repaints the cursor
	    public delegate void				DrawCursor_t(IntPtr ptr) ;

								        // Provides read access to the idDict that holds this gui's state.
	    public delegate IntPtr		State_t(IntPtr ptr)  ;

								        // Removes a gui state variable
	    public delegate void				DeleteStateVar_t( IntPtr ptr, string varName ) ;

								        // Sets a gui state variable.
	    public delegate void				SetStateString_t( IntPtr ptr, string varName, string value ) ;
	    public delegate void				SetStateBool_t( IntPtr ptr, string varName,  bool value ) ;
	    public delegate void				SetStateInt_t( IntPtr ptr, string varName,  int value ) ;
	    public delegate void				SetStateFloat_t( IntPtr ptr, string varName,  float value ) ;

								        // Gets a gui state variable
	    public delegate  string 			GetStateString_t( IntPtr ptr, string varName,  string  defaultString = "" )  ;
	    public delegate bool				GetStateBool_t( IntPtr ptr, string varName,  string  defaultString = "0" )   ;
	    public delegate int					GetStateInt_t( IntPtr ptr, string varName,  string  defaultString = "0" )  ;
	    public delegate float				GetStateFloat_t( IntPtr ptr, string varName,  string  defaultString = "0" )  ;

								        // The state has changed and the gui needs to update from the state idDict.
	    public delegate void				StateChanged_t( IntPtr ptr, int time, bool redraw = false ) ;

								        // Activated the gui.
	    public delegate IntPtr 		Activate_t( IntPtr ptr, bool activate, int time ) ;

								        // Triggers the gui and runs the onTrigger scripts.
	    public delegate void				Trigger_t( IntPtr ptr, int time ) ;

	        public delegate	void				ReadFromDemoFile_t( IntPtr ptr, IntPtr f) ;
	        public delegate	void				WriteToDemoFile_t(  IntPtr ptr, IntPtr f ) ;

	    public delegate bool				WriteToSaveGame_t( IntPtr ptr, IntPtr savefile )  ;
	    public delegate bool				ReadFromSaveGame_t( IntPtr ptr, IntPtr savefile ) ;
	    public delegate void				SetKeyBindingNames_t(IntPtr ptr) ;

	    public delegate void				SetCursor_t( IntPtr ptr, float x, float y ) ;
	    public delegate float				CursorX_t(IntPtr ptr) ;
	    public delegate float				CursorY_t(IntPtr ptr) ;


        public Deructor_t Deructor;

	    public Free_t Free ;

								        // Returns the name of the gui.
	    public Name_t Name  ;

								        // Returns a comment on the gui.
	   public Comment_t Comment  ;

								        // Returns true if the gui is interactive.
	    public IsInteractive_t IsInteractive  ;

	    public IsUniqued_t IsUniqued  ;

	    public SetUniqued_t SetUniqued ;
								        // returns false if it failed to load
	    public InitFromFile_t InitFromFile;

								        // handles an event, can return an action string, the caller interprets
								        // any return and acts accordingly
	    public HandleEvent_t HandleEvent;

								        // handles a named event
	    public HandleNamedEvent_t HandleNamedEvent;

								        // repaints the ui
	    public Redraw_t Redraw;

								        // repaints the cursor
	    public DrawCursor_t DrawCursor ;

								        // Provides read access to the idDict that holds this gui's state.
	    public State_t State  ;

								        // Removes a gui state variable
	    public DeleteStateVar_t DeleteStateVar ;

        // Activated the gui.
        public Activate_t Activate;

								        // Sets a gui state variable.
	    public SetStateString_t SetStateString;
	    public SetStateBool_t SetStateBool ;
	    public SetStateInt_t SetStateInt;
	    public SetStateFloat_t SetStateFloat ;

								        // Gets a gui state variable
	    public GetStateString_t GetStateString  ;
	    public GetStateBool_t GetStateBool   ;
	    public GetStateInt_t GetStateInt  ;
	    public GetStateFloat_t GetStateFloat ;

								        // The state has changed and the gui needs to update from the state idDict.
	    public StateChanged_t StateChanged;

						

								        // Triggers the gui and runs the onTrigger scripts.
	   public Trigger_t Trigger ;

	   public ReadFromDemoFile_t ReadFromDemoFile ;
	   public WriteToDemoFile_t WriteToDemoFile ;

	   public WriteToSaveGame_t WriteToSaveGame  ;
	   public ReadFromSaveGame_t ReadFromSaveGame ;
	   public SetKeyBindingNames_t SetKeyBindingNames ;

	   public SetCursor_t SetCursor ;
	   public CursorX_t CursorX ;
       public CursorY_t CursorY;

    }
}
