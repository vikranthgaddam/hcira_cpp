#include <wx/wx.h>
#include "user_interface.cpp"



class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        wxFrame* frame = new wxFrame(NULL, wxID_ANY, "$1 Recognizer");
        MyCanvas* canvas = new MyCanvas(frame);
        frame->Show();
        return true;
    }
};

//creates an instance of the MyApp
wxIMPLEMENT_APP(MyApp);
