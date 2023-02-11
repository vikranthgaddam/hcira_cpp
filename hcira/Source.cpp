#include <wx/wx.h>
#include "user_interface.cpp"
#include <wx/graphics.h>
#include <vector>
#include <string>
#include <math.h>
#include <cmath>
#include <utility>
#include <stdlib.h>
#include <chrono>
#include <wx/math.h>
#include <iostream>
using namespace std;

class Point {
public:
    double x;
    double y;
    Point(double x, double y) {
        this->x = x;
        this->y = y;
    }
};


class MyCanvas : public wxWindow
{
public:
    //Canvas for drawing 
    MyCanvas(wxWindow* parent) : wxWindow(parent, wxID_ANY)
    {
        Connect(wxEVT_PAINT, wxPaintEventHandler(MyCanvas::OnPaint));
        Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MyCanvas::OnLeftDown));
        Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MyCanvas::OnLeftUp));
        Connect(wxEVT_MOTION, wxMouseEventHandler(MyCanvas::OnMotion));

    }


private:
    //event handler is called when the canvas is repainted
    void OnPaint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);
        wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

        gc->SetPen(wxPen(*wxRED, 100));
        for (size_t i = 1; i < m_points.size(); i++)
        {
            dc.DrawLine(m_points[i - 1].x, m_points[i - 1].y, m_points[i].x, m_points[i].y);
        }

        delete gc;
    }
    //event handler is called when the left mouse button is pressed and pushes position to m_points
    void OnLeftDown(wxMouseEvent& event)
    {
        m_points.clear();
        m_points.push_back(event.GetPosition());

        //a.Recognize(m_points,true);
        CaptureMouse();
    }
    //event handler is called when the left mouse button is released
    void OnLeftUp(wxMouseEvent& event)
    {
        m_points.push_back(event.GetPosition());
        ReleaseMouse();
        points.clear();
        for (auto& wxPoint : m_points) {
            points.emplace_back(wxPoint.x, wxPoint.y);
        }
        for (auto point : points) {
            cout << "x: " << point.x << " y: " << point.y << endl; //this didn't write the output on the console rather has to set a breakpoint to check the values of the variables.
        }
        Refresh();
    }
    //function event handler is called when the mouse is moved
    void OnMotion(wxMouseEvent& event)
    {
        if (event.Dragging() && event.LeftIsDown())
        {
            m_points.push_back(event.GetPosition());
            Refresh();
        }
    }

    vector<wxPoint> m_points;
    //points being stored for future reference
    vector<Point> points;

};



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
