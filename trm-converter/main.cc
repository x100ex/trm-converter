// Copyright (C) 2012, IMM UB RAS
// Copyright (C) 2012, Aleksander A. Popov <x100@yandex.ru>
// encoding : utf8
// кодировка : utf8

#include <cstdio>
#include <fstream>
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
#include <wx/aboutdlg.h>
#include "data.h"
using namespace std;

class MyFrame;
class Data;

class MyApp : public wxApp {
public:
  virtual bool OnInit();
  virtual int OnExit();
protected:
  MyFrame* frame_;
  Data* data_;
};

class MyFrame : public wxFrame {
public:
  MyFrame(Data* data,
          const wxString& title,
          const wxPoint& pos,
          const wxSize& size);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnLoad(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void DoGiveHelp(const wxString& text, bool show);
  void OnMenuClose(wxMenuEvent& event);

  DECLARE_EVENT_TABLE()

protected:
  Data* data_;
  wxMenu* menu_file_;
  wxString status_;
  wxString last_help_shown_;
};

enum {
  kIdQuit = 1,
  kIdLoad,
  kIdSave,
  kIdAbout
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(kIdQuit, MyFrame::OnQuit)
  EVT_MENU(kIdAbout, MyFrame::OnAbout)
  EVT_MENU(kIdLoad, MyFrame::OnLoad)
  EVT_MENU(kIdSave, MyFrame::OnSave)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
  frame_ = NULL;
  data_ = NULL;

  data_ = new Data;

  frame_ = new MyFrame(data_,
                       _("Trm Converter"),
                       wxDefaultPosition,
                       wxSize(450,340));
  frame_->Show(true);
  SetTopWindow(frame_);
  return true;
}

int MyApp::OnExit() {
  delete data_;
  data_ = NULL;
  return wxApp::OnExit();
}

MyFrame::MyFrame(Data* data,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size)
    : wxFrame(NULL, -1, title, pos, size),
      data_(data),
      menu_file_(NULL) {
  menu_file_ = new wxMenu;
  menu_file_->UpdateUI(NULL);
  menu_file_->Append(kIdLoad, _("&Load trm..."), _("Load trm..."));
  menu_file_->Append(kIdSave, _("&Save txt..."), _("&Save txt..."));
  wxMenuItem* item = menu_file_->FindItem(kIdSave);
  item->Enable(false);
  menu_file_->AppendSeparator();
  menu_file_->Append(kIdAbout, _("&About..."), _("About..."));
  menu_file_->AppendSeparator();
  menu_file_->Append(kIdQuit, _("E&xit"), _("Exit"));

  wxMenuBar* menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file_, _("&File"));
  SetMenuBar(menu_bar);

  status_ = _("Empty.");
  CreateStatusBar();
  SetStatusText(status_);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
  Close(true);
}

void MyFrame::OnMenuClose(wxMenuEvent& WXUNUSED(event))
{
    DoGiveHelp(wxEmptyString, false);
}

void MyFrame::DoGiveHelp(const wxString& text, bool show)
{
  if (m_statusBarPane < 0) return;
  wxStatusBar* statbar = GetStatusBar();
  if (! statbar) return;

  wxString help;
  if (show) {
    if (m_oldStatusText.empty()) {
      m_oldStatusText = statbar->GetStatusText(m_statusBarPane);
      if (m_oldStatusText.empty()) {
        // use special value to prevent us from doing this the next time
        m_oldStatusText += _T('\0');
      }
    }
    help = text;
    last_help_shown_ = help;
  } else { // hide the status bar text
    // also clear the old status text but remember it too to restore it below
    help.swap(m_oldStatusText);
    if (statbar->GetStatusText(m_statusBarPane) != last_help_shown_) {
      last_help_shown_.clear();
      // if the text was changed with an explicit SetStatusText() call
      // from the user code in the meanwhile, do not overwrite it with
      // the old status bar contents -- this is almost certainly not what
      // the user expects and would be very hard to avoid from user code
      return;
    }
  }
  statbar->SetStatusText(help, m_statusBarPane);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
  wxAboutDialogInfo info;
  info.SetName(_("Trm Converter"));
  info.SetVersion(_("1.0.0"));
  info.SetCopyright(_T("Copyright \xA9 2012, IMM UB RAS \n")
                    _T("Copyright \xA9 2012, ")
                    _T("Aleksander A. Popov <x100@yandex.ru>"));
  info.SetDescription(_("\nThis program convert `trm` to `txt`."));
  wxAboutBox(info);
}

void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event)) {
  if (! data_) return;
  wxString filename = wxFileSelector(
    _("Choose `trm` file to open"),
    _(""),
    _(""),
    _("Trm files (*.trm)|*.trm"),
    _("Trm files (*.trm)|*.trm|All files (*.*)|*.*"),
    wxFD_OPEN | wxFD_FILE_MUST_EXIST,
    NULL,
    -1,
    -1);
  if (filename.IsEmpty()) return;
  wxCharBuffer buf = filename.mb_str(wxConvLocal);
  data_->load(buf.data());
  if (! menu_file_) return;
  wxMenuItem* item = menu_file_->FindItem(kIdSave);
  if (! item) return;
  if (data_->isLoaded()) {
    item->Enable(true);
    status_ = _("Loaded.");
    SetStatusText(status_);
  } else {
    item->Enable(false);
    status_ = _("Empty.");
    SetStatusText(status_);
  }
  wxBell();
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event)) {
  if (data_ && data_->isLoaded()) {
    wxString filename = wxFileSelector(
      _("Choose `txt` file to save"),
      _(""),
      _("Untitled.txt"),
      _("Text files (*.txt)|*.txt"),
      _("Text files (*.txt)|*.txt|All files (*.*)|*.*"),
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
      NULL,
      -1,
      -1);
    wxCharBuffer buf = filename.mb_str(wxConvLocal);
    data_->save(buf.data());
    status_ = _("Saved.");
    SetStatusText(status_);
    wxBell();
  }
}
