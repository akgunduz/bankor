/////////////////////////////////////////////////////////////////////////////
// Name:        UserInterface.h
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 22 Oct 2015 14:22:19 EEST
// RCS-ID:      
// Copyright:   All rights reserved
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), Thu 22 Oct 2015 14:22:19 EEST

#ifndef _USERINTERFACE_H_
#define _USERINTERFACE_H_

#include "Distributor.h"
#include "Collector.h"
#include <wx/treelist.h>

////@begin includes
#include "wx/frame.h"
#include "wx/notebook.h"
#include "wx/tglbtn.h"
#include "wx/listctrl.h"
////@end includes

class UserInterface;

typedef void (UserInterface::*fUIUpdater)(wxCommandEvent &event);

////@begin forward declarations
class wxToggleButton;
class wxListCtrl;
////@end forward declarations

#define ID_COLL_JOB_LIST 10100

////@begin control identifiers
#define ID_USERINTERFACE 10000
#define ID_NOTEBOOK 10001
#define ID_PANEL_DISTRIBUTOR 10002
#define ID_DIST_BACKUP_RATE 10007
#define ID_DIST_CONNECT_INTERFACE 10006
#define ID_DIST_BINDED_ADDRESS 10008
#define ID_DIST_BACKUP_STATUS 10005
#define ID_DIST_INIT 10010
#define ID_DIST_POLL 10011
#define ID_DIST_COLL_LIST 10012
#define ID_DIST_NODE_LIST 10009
#define ID_DIST_LOG 10013
#define ID_PANEL_COLLECTOR 10014
#define ID_COLL_DIST_ADDRESS 10004
#define ID_COLL_CONNECT_INTERFACE 10016
#define ID_COLL_BINDED_ADDRESS 10017
#define ID_COLL_NODE_ADDRESS 10018
#define ID_COLL_INIT 10019
#define ID_COLL_PROCESS 10020
#define ID_COLL_PROCESS_LIST 10003
#define ID_COLL_LOG 10023
#define ID_NODE_BINDED_ADDRESS 10015
#define ID_NODE_COLL_ADDRESS 10022
#define ID_NODE_CONNECT_INTERFACE 10014
#define ID_NODE_STATE 10026
#define ID_NODE_INIT 10027
#define ID_NODE_FILE_LIST 10029
#define ID_NODE_EXEC_LIST 10030
#define ID_NODE_LOG 10031
#define SYMBOL_USERINTERFACE_STYLE wxDEFAULT_FRAME_STYLE|wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_USERINTERFACE_TITLE _("Bankor")
#define SYMBOL_USERINTERFACE_IDNAME ID_USERINTERFACE
#define SYMBOL_USERINTERFACE_SIZE wxSize(600, 700)
#define SYMBOL_USERINTERFACE_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * UserInterface class declaration
 */

class UserInterface: public wxFrame
{    
    DECLARE_CLASS( UserInterface )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    UserInterface();
    UserInterface( wxWindow* parent, wxWindowID id = SYMBOL_USERINTERFACE_IDNAME, const wxString& caption = SYMBOL_USERINTERFACE_TITLE, const wxPoint& pos = SYMBOL_USERINTERFACE_POSITION, const wxSize& size = SYMBOL_USERINTERFACE_SIZE, long style = SYMBOL_USERINTERFACE_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_USERINTERFACE_IDNAME, const wxString& caption = SYMBOL_USERINTERFACE_TITLE, const wxPoint& pos = SYMBOL_USERINTERFACE_POSITION, const wxSize& size = SYMBOL_USERINTERFACE_SIZE, long style = SYMBOL_USERINTERFACE_STYLE );

    /// Destructor
    ~UserInterface();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    wxEventTypeTag<wxCommandEvent> *ui_event;

    fUIUpdater uiUpdater[UI_UPDATE_MAX];

    //Generic

    void updateUIEvent(int, void*);

    void updateUI(wxCommandEvent &event);
    void updateLog(wxCommandEvent &event);

    //Distributor

    Distributor *distObject;

    void distInit();

    void distUpdateAddresses(wxCommandEvent& event);
    void distAddtoCollectorList(wxCommandEvent& event);
    void distAddtoNodeList(wxCommandEvent& event);

    void distUpdateLog(wxCommandEvent &event);
    void distUpdateBackup(wxCommandEvent &event);

    //Collector

    Collector *collObject;

    void collInit();

    void collUpdateAddresses(wxCommandEvent& event);
    void collUpdateAttachedDistAddress(wxCommandEvent& event);
    void collUpdateAttachedNodeAddress(wxCommandEvent& event);

    void collUpdateLog(wxCommandEvent &event);
    void collUpdateJobList(wxCommandEvent &event);
    void collUpdateProcessList(wxCommandEvent &event);

    //Node

    Node *nodeObject;

    void nodeInit();

    void nodeUpdateAddresses(wxCommandEvent& event);
    void nodeUpdateState(wxCommandEvent& event);
    void nodeUpdateAttachedCollAddress(wxCommandEvent& event);

    void nodeUpdateLog(wxCommandEvent &event);
    void nodeUpdateFileList(wxCommandEvent &event);
    void nodeUpdateExecList(wxCommandEvent &event);
    void nodeUpdateClear(wxCommandEvent &event);

////@begin UserInterface event handler declarations

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_DIST_INIT
    void OnDistInitClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DIST_POLL
    void OnDistPollClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_COLL_INIT
    void OnCollInitClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COLL_PROCESS
    void OnCollProcessClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_NODE_INIT
    void OnNodeInitClick( wxCommandEvent& event );

////@end UserInterface event handler declarations

    void OnCollJobListChecked( wxTreeListEvent& event );

////@begin UserInterface member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end UserInterface member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    wxTreeListCtrl* collJobList;

////@begin UserInterface member variables
    wxTextCtrl* distBackupRate;
    wxChoice* distConnectInterface;
    wxStaticText* distAddress;
    wxStaticText* distBackupStatus;
    wxToggleButton* distInitBtn;
    wxButton* distPollBtn;
    wxListCtrl* distCollList;
    wxListCtrl* distNodeList;
    wxListBox* distLog;
    wxStaticText* collDistAddress;
    wxChoice* collConnectInterface;
    wxStaticText* collAddress;
    wxStaticText* collNodeAddress;
    wxToggleButton* collInitBtn;
    wxButton* collProcessBtn;
    wxListBox* collProcessList;
    wxListBox* collLog;
    wxStaticText* nodeCollAddress;
    wxChoice* nodeConnectInterface;
    wxStaticText* nodeAddress;
    wxStaticText* nodeState;
    wxToggleButton* nodeInitBtn;
    wxListCtrl* nodeFileList;
    wxListBox* nodeExecList;
    wxListBox* nodeLog;
////@end UserInterface member variables
};

#endif
    // _USERINTERFACE_H_
