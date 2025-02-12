// CircleDrawingDlg.h

#pragma once
#include <vector>
#include <thread>

struct Point
{
    double x, y;
};

class CCircleDrawingDlg : public CDialogEx
{
public:
    CCircleDrawingDlg(CWnd* pParent = nullptr);
    ~CCircleDrawingDlg();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedButtonClear();
    afx_msg void OnBnClickedButtonRandom();
    afx_msg void OnEnChangeEditRadius();
    afx_msg void OnEnChangeEditThickness();

    DECLARE_MESSAGE_MAP()

private:
    std::vector<Point> m_points;
    double m_circleRadius; // 클릭 지점 원의 반지름
    int m_lineThickness; // 정원의 선 두께
    bool m_dragging = false;
    int m_dragIndex = -1;
    bool m_threadRunning = false;
    std::thread m_animationThread;

    void UpdateCoordinatesUI();
    void MovePointsRandomly();
    void CalculateCircle(Point& center, double& radius);
};
