// CircleDrawingDlg.cpp : 구현 파일
//

#include "pch.h"
#include "CircleDrawing.h" // 프로젝트 이름으로 변경
#include "CircleDrawingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCircleDrawingDlg 대화 상자

CCircleDrawingDlg::CCircleDrawingDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CIRCLEDRAWING_DIALOG, pParent),
    m_circleRadius(5), // 기본 반지름
    m_lineThickness(2),  // 기본 두께
    m_threadRunning(false)
{
}

CCircleDrawingDlg::~CCircleDrawingDlg()
{
    if (m_animationThread.joinable()) {
        m_threadRunning = false;
        m_animationThread.join(); // 스레드 종료 대기
    }
}

void CCircleDrawingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_RADIUS, m_circleRadius);
    DDX_Text(pDX, IDC_EDIT_THICKNESS, m_lineThickness);
}

BEGIN_MESSAGE_MAP(CCircleDrawingDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CCircleDrawingDlg::OnBnClickedButtonClear)
    ON_BN_CLICKED(IDC_BUTTON_RANDOM, &CCircleDrawingDlg::OnBnClickedButtonRandom)
    ON_EN_CHANGE(IDC_EDIT_RADIUS, &CCircleDrawingDlg::OnEnChangeEditRadius)
    ON_EN_CHANGE(IDC_EDIT_THICKNESS, &CCircleDrawingDlg::OnEnChangeEditThickness)
END_MESSAGE_MAP()

// CCircleDrawingDlg 메시지 처리기

BOOL CCircleDrawingDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return TRUE; // 포커스를 이 대화 상자에 설정하지 않음
}

void CCircleDrawingDlg::OnPaint()
{
    CPaintDC dc(this); // 그리기 위한 장치 컨텍스트

    // 클릭 지점 원 그리기
    for (size_t i = 0; i < m_points.size(); ++i) {
        if (i < 3) {
            CBrush brush(RGB(0, 0, 0)); // 검은색 브러시
            dc.SelectObject(&brush);
            dc.Ellipse(m_points[i].x - m_circleRadius, m_points[i].y - m_circleRadius,
                m_points[i].x + m_circleRadius, m_points[i].y + m_circleRadius);
        }
    }

    // 3개의 점을 지나는 원 그리기
    if (m_points.size() == 3) {
        Point center;
        double radius;

        // 원의 중심과 반지름 계산
        CalculateCircle(center, radius);

        CPen pen(PS_SOLID, m_lineThickness, RGB(0, 0, 0)); // 검은색 펜
        dc.SelectObject(&pen);

        // **원 내부를 투명하게 만들기 위해 NULL_BRUSH 선택**
        CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

        // 원 내부를 비우고 경계선만 그리기
        dc.Ellipse(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
    }
}

void CCircleDrawingDlg::CalculateCircle(Point& center, double& radius)
{
    // 세 점을 지나는 원의 중심과 반지름 계산
    double x1 = m_points[0].x, y1 = m_points[0].y;
    double x2 = m_points[1].x, y2 = m_points[1].y;
    double x3 = m_points[2].x, y3 = m_points[2].y;

    double A = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
    double B = (x1 * x1 + y1 * y1) * (y3 - y2) + (x2 * x2 + y2 * y2) * (y1 - y3) + (x3 * x3 + y3 * y3) * (y2 - y1);
    double C = (x1 * x1 + y1 * y1) * (x2 - x3) + (x2 * x2 + y2 * y2) * (x3 - x1) + (x3 * x3 + y3 * y3) * (x1 - x2);

    center.x = -B / (2 * A);
    center.y = -C / (2 * A);
    radius = sqrt((center.x - x1) * (center.x - x1) + (center.y - y1) * (center.y - y1));
}

void CCircleDrawingDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_points.size() < 3) {
        // 클릭 지점 저장
        m_points.push_back({ (double)point.x, (double)point.y });
        UpdateCoordinatesUI();
        Invalidate(); // 화면 업데이트
    }
    else {
        // 드래그 시작
        for (size_t i = 0; i < m_points.size(); ++i) {
            if (abs(m_points[i].x - point.x) < m_circleRadius && abs(m_points[i].y - point.y) < m_circleRadius) {
                m_dragging = true;
                m_dragIndex = i;
                break;
            }
        }
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

void CCircleDrawingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_dragging) {
        // 점 이동
        m_points[m_dragIndex].x = point.x;
        m_points[m_dragIndex].y = point.y;
        Invalidate(); // 화면 업데이트
    }
    CDialogEx::OnMouseMove(nFlags, point);
}

void CCircleDrawingDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_dragging = false; // 드래그 종료
    CDialogEx::OnLButtonUp(nFlags, point);
}

void CCircleDrawingDlg::UpdateCoordinatesUI()
{
    CString coords;
    for (const auto& pt : m_points) {
        coords.AppendFormat(L"(%.1f, %.1f)\n", pt.x, pt.y);
    }
    SetDlgItemText(IDC_STATIC_COORDS, coords);
}

void CCircleDrawingDlg::OnBnClickedButtonClear()
{
    m_points.clear();
    Invalidate();
}

void CCircleDrawingDlg::OnBnClickedButtonRandom()
{
    if (!m_threadRunning) {
        m_threadRunning = true;
        m_animationThread = std::thread(&CCircleDrawingDlg::MovePointsRandomly, this);
    }
}

void CCircleDrawingDlg::MovePointsRandomly()
{
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        for (auto& pt : m_points) {
            pt.x += (rand() % 20 - 10); // 랜덤으로 x 좌표 이동
            pt.y += (rand() % 20 - 10); // 랜덤으로 y 좌표 이동
        }
        UpdateCoordinatesUI();
        Invalidate(); // 화면 업데이트
    }
    m_threadRunning = false;
}

void CCircleDrawingDlg::OnEnChangeEditRadius()
{
    UpdateData(TRUE); // 데이터를 UI에서 가져옴
}

void CCircleDrawingDlg::OnEnChangeEditThickness()
{
    UpdateData(TRUE); // 데이터를 UI에서 가져옴
}
