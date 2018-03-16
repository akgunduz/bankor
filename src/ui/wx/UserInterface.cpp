/////////////////////////////////////////////////////////////////////////////
// Name:        UserInterface.cpp
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 22 Oct 2015 14:22:19 EEST
// RCS-ID:      
// Copyright:   All rights reserved
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), Thu 22 Oct 2015 14:22:19 EEST

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "UserInterface.h"

////@begin XPM images
////@end XPM images


/*
 * UserInterface type definition
 */

IMPLEMENT_CLASS( UserInterface, wxFrame )


/*
 * UserInterface event table definition
 */

BEGIN_EVENT_TABLE( UserInterface, wxFrame )

////@begin UserInterface event table entries
    EVT_TOGGLEBUTTON( ID_DIST_INIT, UserInterface::OnDistInitClick )
    EVT_BUTTON( ID_DIST_POLL, UserInterface::OnDistPollClick )
    EVT_TOGGLEBUTTON( ID_COLL_INIT, UserInterface::OnCollInitClick )
    EVT_BUTTON( ID_COLL_PROCESS, UserInterface::OnCollProcessClick )
    EVT_TOGGLEBUTTON( ID_NODE_INIT, UserInterface::OnNodeInitClick )
////@end UserInterface event table entries

    EVT_TREELIST_ITEM_CHECKED( ID_COLL_JOB_LIST, UserInterface::OnCollJobListChecked )

END_EVENT_TABLE()


/*
 * UserInterface constructors
 */

UserInterface::UserInterface()
{
    Init();
}

UserInterface::UserInterface( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * UserInterface creator
 */

bool UserInterface::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin UserInterface creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end UserInterface creation
    
    ui_event = new wxEventTypeTag<wxCommandEvent>(wxNewEventType());
    Bind(*ui_event, &UserInterface::updateUI, this, 0, UI_UPDATE_MAX - 1, NULL);
    uiUpdater[UI_UPDATE_LOG] = &UserInterface::updateLog;

    distInit();
    collInit();
    nodeInit();
    return true;
}


/*
 * UserInterface destructor
 */

UserInterface::~UserInterface()
{
////@begin UserInterface destruction
////@end UserInterface destruction
}


/*
 * Member initialisation
 */

void UserInterface::Init()
{
    Log::setLogLevel(LEVEL_WARN);
    Log::set_ui_callback(this, updateUICallback);

    collJobList = NULL;

////@begin UserInterface member initialisation
    distBackupRate = NULL;
    distCollInterface = NULL;
    distCollDeviceAddress = NULL;
    distBackupStatus = NULL;
    distInitBtn = NULL;
    distPollBtn = NULL;
    distCollList = NULL;
    distNodeList = NULL;
    distLog = NULL;
    nodeInterface = NULL;
    distNodeDeviceAddress = NULL;
    collDistAddress = NULL;
    collNodeDeviceAddress = NULL;
    collNodeAddress = NULL;
    collInitBtn = NULL;
    collProcessBtn = NULL;
    collProcessList = NULL;
    collLog = NULL;
    collDistDeviceAddress = NULL;
    nodeCollAddress = NULL;
    nodeDeviceAddress = NULL;
    nodeState = NULL;
    nodeInitBtn = NULL;
    nodeFileList = NULL;
    nodeExecList = NULL;
    nodeLog = NULL;
////@end UserInterface member initialisation
}


/*
 * Control creation for UserInterface
 */

void UserInterface::CreateControls()
{    
////@begin UserInterface content construction
    UserInterface* itemFrame1 = this;

    wxNotebook* itemNotebook2 = new wxNotebook( itemFrame1, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    wxPanel* itemPanel3 = new wxPanel( itemNotebook2, ID_PANEL_DISTRIBUTOR, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemPanel3->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticText* itemStaticText4 = new wxStaticText( itemPanel3, wxID_STATIC, _("Collector Interface :"), wxPoint(10, 30), wxDefaultSize, 0 );

    wxStaticText* itemStaticText5 = new wxStaticText( itemPanel3, wxID_STATIC, _("Backup Rate :"), wxPoint(10, 110), wxDefaultSize, 0 );

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel3, wxID_STATIC, _("Collectors :"), wxPoint(10, 280), wxDefaultSize, 0 );

    distBackupRate = new wxTextCtrl( itemPanel3, ID_DIST_BACKUP_RATE, _("0"), wxPoint(150, 106), wxSize(130, -1), 0 );

    wxArrayString distCollInterfaceStrings;
    distCollInterface = new wxChoice( itemPanel3, ID_DIST_CONNECT_INTERFACE, wxPoint(150, 26), wxSize(130, -1), distCollInterfaceStrings, 0 );

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, wxID_STATIC, _("Binded Address :"), wxPoint(10, 70), wxDefaultSize, 0 );

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel3, wxID_STATIC, _("Backup Status :"), wxPoint(310, 110), wxDefaultSize, 0 );

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel3, wxID_STATIC, _("Nodes :"), wxPoint(310, 280), wxDefaultSize, 0 );

    distCollDeviceAddress = new wxStaticText( itemPanel3, ID_DIST_BINDED_ADDRESS, _("0"), wxPoint(150, 70), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    distBackupStatus = new wxStaticText( itemPanel3, ID_DIST_BACKUP_STATUS, _("0"), wxPoint(450, 110), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    distInitBtn = new wxToggleButton( itemPanel3, ID_DIST_INIT, _("Init"), wxPoint(10, 210), wxSize(270, 50), 0 );
    distInitBtn->SetValue(false);

    distPollBtn = new wxButton( itemPanel3, ID_DIST_POLL, _("Poll"), wxPoint(310, 210), wxSize(270, 50), 0 );

    distCollList = new wxListCtrl( itemPanel3, ID_DIST_COLL_LIST, wxPoint(10, 320), wxSize(270, 200), wxLC_REPORT );

    distNodeList = new wxListCtrl( itemPanel3, ID_DIST_NODE_LIST, wxPoint(310, 320), wxSize(270, 200), wxLC_REPORT );

    wxArrayString distLogStrings;
    distLog = new wxListBox( itemPanel3, ID_DIST_LOG, wxPoint(10, 540), wxSize(570, 180), distLogStrings, wxLB_SINGLE );

    wxStaticText* itemStaticText19 = new wxStaticText( itemPanel3, wxID_STATIC, _("Node Interface :"), wxPoint(310, 30), wxDefaultSize, 0 );

    wxArrayString nodeInterfaceStrings;
    nodeInterface = new wxChoice( itemPanel3, ID_CHOICE, wxPoint(450, 26), wxSize(130, -1), nodeInterfaceStrings, 0 );

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel3, wxID_STATIC, _("Binded Address :"), wxPoint(310, 70), wxDefaultSize, 0 );

    distNodeDeviceAddress = new wxStaticText( itemPanel3, ID_STATICTEXT, _("0"), wxPoint(450, 70), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    itemNotebook2->AddPage(itemPanel3, _("Distributor"));

    wxPanel* itemPanel23 = new wxPanel( itemNotebook2, ID_PANEL_COLLECTOR, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemPanel23->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    collDistAddress = new wxStaticText( itemPanel23, ID_COLL_DIST_ADDRESS, _("0"), wxPoint(160, 70), wxSize(120, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    wxStaticText* itemStaticText25 = new wxStaticText( itemPanel23, wxID_STATIC, _("Distributor Address :"), wxPoint(10, 70), wxDefaultSize, 0 );

    wxStaticText* itemStaticText26 = new wxStaticText( itemPanel23, wxID_STATIC, _("Jobs :"), wxPoint(10, 280), wxDefaultSize, 0 );

    wxStaticText* itemStaticText27 = new wxStaticText( itemPanel23, wxID_STATIC, _("Binded Address :"), wxPoint(310, 30), wxDefaultSize, 0 );

    wxStaticText* itemStaticText28 = new wxStaticText( itemPanel23, wxID_STATIC, _("Last Node Address :"), wxPoint(310, 70), wxDefaultSize, 0 );

    wxStaticText* itemStaticText29 = new wxStaticText( itemPanel23, wxID_STATIC, _("Process :"), wxPoint(310, 280), wxDefaultSize, 0 );

    collNodeDeviceAddress = new wxStaticText( itemPanel23, ID_COLL_BINDED_ADDRESS, _("0"), wxPoint(450, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    collNodeAddress = new wxStaticText( itemPanel23, ID_COLL_NODE_ADDRESS, _("0"), wxPoint(460, 70), wxSize(120, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    collInitBtn = new wxToggleButton( itemPanel23, ID_COLL_INIT, _("Init"), wxPoint(10, 210), wxSize(270, 50), 0 );
    collInitBtn->SetValue(false);

    collProcessBtn = new wxButton( itemPanel23, ID_COLL_PROCESS, _("Process"), wxPoint(310, 210), wxSize(270, 50), 0 );

    wxArrayString collProcessListStrings;
    collProcessList = new wxListBox( itemPanel23, ID_COLL_PROCESS_LIST, wxPoint(310, 320), wxSize(270, 200), collProcessListStrings, wxLB_SINGLE );

    wxArrayString collLogStrings;
    collLog = new wxListBox( itemPanel23, ID_COLL_LOG, wxPoint(10, 540), wxSize(570, 180), collLogStrings, wxLB_SINGLE );

    wxStaticText* itemStaticText36 = new wxStaticText( itemPanel23, wxID_STATIC, _("Binded Address :"), wxPoint(10, 30), wxDefaultSize, 0 );

    collDistDeviceAddress = new wxStaticText( itemPanel23, ID_STATICTEXT1, _("0"), wxPoint(150, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    itemNotebook2->AddPage(itemPanel23, _("Collector"));

    wxPanel* itemPanel38 = new wxPanel( itemNotebook2, ID_NODE_BINDED_ADDRESS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemPanel38->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    nodeCollAddress = new wxStaticText( itemPanel38, ID_NODE_COLL_ADDRESS, _("0"), wxPoint(160, 70), wxSize(120, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    wxStaticText* itemStaticText40 = new wxStaticText( itemPanel38, wxID_STATIC, _("Collector Address :"), wxPoint(10, 70), wxDefaultSize, 0 );

    wxStaticText* itemStaticText41 = new wxStaticText( itemPanel38, wxID_STATIC, _("Files :"), wxPoint(10, 280), wxDefaultSize, 0 );

    wxStaticText* itemStaticText42 = new wxStaticText( itemPanel38, wxID_STATIC, _("Binded Address :"), wxPoint(10, 30), wxDefaultSize, 0 );

    wxStaticText* itemStaticText43 = new wxStaticText( itemPanel38, wxID_STATIC, _("State :"), wxPoint(310, 70), wxDefaultSize, 0 );

    wxStaticText* itemStaticText44 = new wxStaticText( itemPanel38, wxID_STATIC, _("Executors :"), wxPoint(310, 280), wxDefaultSize, 0 );

    nodeDeviceAddress = new wxStaticText( itemPanel38, ID_NODE_BINDED_ADDRESS, _("0"), wxPoint(450, 30), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    nodeState = new wxStaticText( itemPanel38, ID_NODE_STATE, _("0"), wxPoint(450, 70), wxSize(130, -1), wxALIGN_RIGHT|wxST_NO_AUTORESIZE );

    nodeInitBtn = new wxToggleButton( itemPanel38, ID_NODE_INIT, _("Init"), wxPoint(10, 210), wxSize(570, 50), 0 );
    nodeInitBtn->SetValue(false);

    nodeFileList = new wxListCtrl( itemPanel38, ID_NODE_FILE_LIST, wxPoint(10, 320), wxSize(270, 200), wxLC_REPORT );

    wxArrayString nodeExecListStrings;
    nodeExecList = new wxListBox( itemPanel38, ID_NODE_EXEC_LIST, wxPoint(310, 320), wxSize(270, 200), nodeExecListStrings, wxLB_SINGLE );

    wxArrayString nodeLogStrings;
    nodeLog = new wxListBox( itemPanel38, ID_NODE_LOG, wxPoint(10, 540), wxSize(570, 180), nodeLogStrings, wxLB_SINGLE );

    itemNotebook2->AddPage(itemPanel38, _("Node"));

////@end UserInterface content construction

    collJobList = new wxTreeListCtrl( itemPanel23, ID_COLL_JOB_LIST, wxPoint(10, 320), wxSize(270, 200), wxTL_3STATE);

}

/*
 * Should we show tooltips?
 */

bool UserInterface::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap UserInterface::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin UserInterface bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end UserInterface bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon UserInterface::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin UserInterface icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end UserInterface icon retrieval
}


void UserInterface::updateUIEvent(int id, void *data) {

    wxCommandEvent event(*ui_event);
    event.SetId(id);
    event.SetClientData(data);
    wxPostEvent(this, event);

}

void UserInterface::updateUI(wxCommandEvent& event) {

    int id = event.GetId();

    if (uiUpdater[id] != nullptr) {
        ((this)->*(uiUpdater[id]))(event);
    }
}

void UserInterface::updateUICallback(void *context, int id, void *data) {
    ((UserInterface*) context)->updateUIEvent(id, data);
}

void UserInterface::updateLog(wxCommandEvent& event) {

    //EventData *data = (EventData *)event.GetClientData();
   // genericLog->Append(data->dataStr);

}
