#include <wx/wx.h>
#include <wx/graphics.h>

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
        CaptureMouse();
    }
    //event handler is called when the left mouse button is released
    void OnLeftUp(wxMouseEvent& event)
    {
        m_points.push_back(event.GetPosition());
        ReleaseMouse();
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

    std::vector<wxPoint> m_points;
    //points being stored for future reference
};