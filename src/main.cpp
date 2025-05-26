#include <wx/wx.h>
#include <wx/thread.h>

#include "cube.h"
#include "settings.h"
#include "settingspanel.h"
#include "openglcanvas.h"
#include "audio.h"

// thread id
enum
{
  AUDIO_THREAD_ID = wxID_HIGHEST + 1
};
wxDECLARE_EVENT(wxEVT_COMMAND_AUDIOTHREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_AUDIOTHREAD_UPDATE, wxThreadEvent);

class MyFrame;

class MyApp : public wxApp
{
public:
  MyApp() {}
  bool OnInit() wxOVERRIDE;

private:
  file_reader test;
};

//
class audio_thread : public wxThread
{
public:
  audio_thread(MyFrame *handler) : wxThread(wxTHREAD_DETACHED)
  {
    m_pHandler = handler;
  }
  ~audio_thread();

protected:
  virtual ExitCode Entry();
  MyFrame *m_pHandler;
};

// inside the frame class we need to have events that can be passed onto the audio thread.
// audio thread is a member of the frame so we can have processes inside the audiothread class.
class MyFrame : public wxFrame
{
public:
  MyFrame(const wxString &title);

  void DoStartThread(wxCommandEvent &);
  void DoPauseThread(wxCommandEvent &);

  // resume routine is nearly indentical to pausing according to docs.
  void DoResumeThread();
  void OnThreadUpdate(wxCommandEvent &);
  void OnThreadCompletion(wxCommandEvent &);
  void OnClose(wxCloseEvent &);

private:
  // i don't really understand the naming convention for the thread
  // but docs do it this way so for now i keep it this way.
  audio_thread *m_pThread;
  wxCriticalSection m_pThreadCS;
  friend class audio_thread;
  wxDECLARE_EVENT_TABLE();

  OpenGLCanvas *openGLCanvas{nullptr};
  SettingsPanel *settingsPanel{nullptr};
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE(MyFrame::OnClose)
        EVT_MENU(AUDIO_THREAD_ID, MyFrame::DoStartThread)
            EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_AUDIOTHREAD_UPDATE, MyFrame::OnThreadUpdate)
                EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_AUDIOTHREAD_COMPLETED, MyFrame::OnThreadCompletion)
                    wxEND_EVENT_TABLE()

                        wxDEFINE_EVENT(wxEVT_COMMAND_AUDIOTHREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUDIOTHREAD_UPDATE, wxThreadEvent);

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  if (!wxApp::OnInit())
    return false;

  MyFrame *frame = new MyFrame("pyramid");
  frame->Show(true);
  test.setSample("/Users/jakemorgan/Documents/xSamples/kirbs/Wave0.wav");
  std::cout << "message from oninit! \nwx configuration set defaults to default and jake isn't sure how to change that... \nuse COUT to debug!!!!" << std::endl;
  // probably need to initialize all the audio processing stuff here at some point
  return true;
}

MyFrame::MyFrame(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title)
{
  wxGLAttributes vAttrs;
  vAttrs.PlatformDefaults().Defaults().EndList();

  if (wxGLCanvas::IsDisplaySupported(vAttrs))
  {
    auto sizer = new wxBoxSizer(wxHORIZONTAL);

    settingsPanel = new SettingsPanel(this);
    settingsPanel->SetMinSize(FromDIP(wxSize(200, 600)));

    openGLCanvas = new OpenGLCanvas(this, vAttrs, settingsPanel->GetSettings());
    openGLCanvas->SetMinSize(FromDIP(wxSize(600, 600)));
    openGLCanvas->SetFocus();

    sizer->Add(openGLCanvas, 1, wxEXPAND);
    sizer->Add(settingsPanel, 0, wxEXPAND);

    SetSizerAndFit(sizer);

    settingsPanel->Bind(SETTINGS_CHANGED_EVENT, [this](wxCommandEvent &event)
                        {
                                openGLCanvas->SetSettings(settingsPanel->GetSettings());
                                openGLCanvas->Refresh(); });

    openGLCanvas->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent &event)
                       { openGLCanvas->SetFocus(); });
  }
}

void MyFrame::DoStartThread(wxCommandEvent &)
{
  m_pThread = new audio_thread(this);
  if (m_pThread->Run() != wxTHREAD_NO_ERROR)
  {
    wxLogError("Can't create the thread!");
    delete m_pThread;
    m_pThread = nullptr;
  }
}

wxThread::ExitCode audio_thread::Entry()
{
  while (!TestDestroy())
  {
    // not entirely sure what this is used for.
    wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_AUDIOTHREAD_UPDATE));
  }
  // signal the event handler that this thread is going to be destroyed
  // NOTE: here we assume that using the m_pHandler pointer is safe;
  // (in this case this is assured by the MyFrame destructor) .. from docs
  wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_AUDIOTHREAD_COMPLETED));

  return (wxThread::ExitCode)0; // success
}

audio_thread::~audio_thread()
{
  wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);

  // thread is being destroyed; make sure not to leave dangling pointer ie mem leak
  m_pHandler->m_pThread = nullptr;
}

void MyFrame::OnThreadCompletion(wxCommandEvent &)
{
  // if this breaks switch to wxlogdebug
  wxMessageOutputDebug().Printf("MYFRAME: audio_thread exited! \n");
}
void MyFrame::OnThreadUpdate(wxCommandEvent &)
{
  wxMessageOutputDebug().Printf("MYFRAME: audio_thread update... \n");
}
void MyFrame::DoPauseThread(wxCommandEvent &)
{
  // ensures that the pointer to this thread won't be modified while we pause it.
  wxCriticalSectionLocker enter(m_pThreadCS);
  if (m_pThread)
  { // make sure thread exists
    // could have some problems here according to docs depending on OS scheduling.
    if (m_pThread->Pause() != wxTHREAD_NO_ERROR)
      wxLogError("can't pause thread");
  }
}
void MyFrame::OnClose(wxCloseEvent &)
{
  {
    wxCriticalSectionLocker enter(m_pThreadCS);
    if (m_pThread)
    {
      wxMessageOutputDebug().Printf("MYFRAME: deleting audio_thread");
      if (m_pThread->Delete() != wxTHREAD_NO_ERROR)
        wxLogError("can't delete audio_thread");
    }
  }
  // criticalsection in brackets since it needs to be locked down while that block of code happens
  while (1)
  {
    {
      wxCriticalSectionLocker enter(m_pThreadCS);
      if (!m_pThread)
        break;
    }
    // wait for thread to finish its work
    wxThread::This()->Sleep(1);
  }
  Destroy();
}
