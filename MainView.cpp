// MainView.cpp : implementation file
//

#include "pch.h"
#include "las-parser.h"
#include "MainView.h"
#include "MainDocument.h"

#include <random>
#include <algorithm>


// CMainView

IMPLEMENT_DYNCREATE(CMainView, CFormView)

CMainView::CMainView()
	: CFormView(IDD_MAIN_VIEW)
{

}

CMainView::~CMainView()
{
    ReleaseCurvePoints();
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_CURVES, m_comboBoxCurves);
}

BEGIN_MESSAGE_MAP(CMainView, CFormView)
	ON_COMMAND(ID_FILE_OPEN32771, &CMainView::OnFileOpen)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_FILE_NAME, &CMainView::OnUpdateFilenamePane)
    ON_CBN_SELCHANGE(IDC_COMBO_CURVES, &CMainView::OnCbnSelchangeComboCurves)
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()
    ON_BN_CLICKED(IDC_BUTTON_CHANGE_PALETTE, &CMainView::OnBnClickedButtonChangeColors)
END_MESSAGE_MAP()


// CMainView diagnostics

#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainView message handlers


void CMainView::OnFileOpen()
{
    // Create File Dialog
    CFileDialog fileDialog(
        TRUE, _T("Open LAS File"), nullptr, 
        OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
        _T("LAS Files (*.las)|*.las||"), this);

    // Show Dialog, Handle OK Result
    if (fileDialog.DoModal() == IDOK)
    {
        // Get Document
        CDocument* document = GetDocument();

        if (document) 
        {
            // Intialize LAS.
            static_cast<CMainDocument*>(document)->
                InitializeLas(fileDialog.GetPathName());
        }
    }
}

void CMainView::OnUpdateFilenamePane(CCmdUI* pCmdUI)
{
    pCmdUI->Enable();

    pCmdUI->SetText(m_lasParserMessage ? 
        *m_lasParserMessage : _T("Ready"));
}


void CMainView::OnDraw(CDC* deviceContext)
{
    CRect viewport;
    GetClientRect(&viewport);

    // Create Pen for Gridlines.
    CPen pen{ PS_DOT, 1, RGB(200, 200, 200) };
    CPen* originalPen = deviceContext->SelectObject(&pen);

    // Draw Vertical Gridlines.
    for (int x = 0; x < viewport.right;
        x += m_gridCellMargin)
    {
        deviceContext->MoveTo(x, 0);
        deviceContext->LineTo(x, viewport.bottom);
    }

    // Draw Horizontal Gridlines.
    for (int y = 0; y < viewport.bottom;
        y += m_gridCellMargin)
    {
        deviceContext->MoveTo(0, y);
        deviceContext->LineTo(viewport.right, y);
    }

    // Restore Pen.
    deviceContext->SelectObject(originalPen);
    pen.DeleteObject();

    if (!m_las)
        return;

    DrawLenged();
    DrawScale();

    // Create Pen for Curves.
    pen.CreatePen(PS_SOLID, 2,
        m_curveLegendItems[m_comboBoxCurves.GetCurSel()].m_color);

    originalPen = deviceContext->SelectObject(&pen);

    // Draw Curve Points.
    deviceContext->Polyline(
        m_curvePoints,
        m_las->GetNumberOfCurves());

    deviceContext->SelectObject(originalPen);
}

void CMainView::ReleaseCurvePoints()
{
    if (m_curvePoints)
    {
        delete[] m_curvePoints;
        m_curvePoints = nullptr;
    }
}

void CMainView::Transform()
{
    if (!m_las)
        return;

    CRect viewport;
    GetClientRect(&viewport);

    // Translation Offsets.
    m_xOffset = (viewport.Width() * 0.035); // 3.5%% of the Width.
    m_yOffset = (viewport.Height() * 0.12); // 12% of the Height.

    // Margins.
    m_xMargin = (viewport.Width() * 0.28); // 28% of the Width.
    m_yMargin = (viewport.Height() * 0.13); // 13% of the Height.

    // Scaling Factors.
    m_xScale = (viewport.Width() - m_xMargin) /
        (m_las->GetMaximumDataValue() - 
            m_las->GetMinimumDataValue());

    m_yScale = (viewport.Height() - m_yMargin) /
        (m_las->GetMaximumDepth() - 
            m_las->GetMinimumDepth());
}

void CMainView::DrawLenged()
{
    if (m_curveLegendItems.size() == 0)
        return;

    // Get Device Context.
    CDC* deviceContext{ GetDC() };

    // Get Viewport Bounding Volume.
    CRect viewport{};
    GetClientRect(&viewport);

    // Create Font.
    unsigned short fontHeight{ 
        static_cast<unsigned short>(viewport.Height() * 0.02) }; // 2% of the Height.

    CFont font{};
    font.CreateFont(
        fontHeight,                         // Font height (size)
        0,                                  // Font width
        0,                                  // Escapement angle
        0,                                  // Orientation angle
        FW_HEAVY,                           // Weight (normal)
        FALSE,                              // Italic
        FALSE,                              // Underline
        0,                                  // Strikeout
        ANSI_CHARSET,                       // Character set
        OUT_DEFAULT_PRECIS,                 // Output precision
        CLIP_DEFAULT_PRECIS,                // Clipping precision
        DEFAULT_QUALITY,                    // Quality
        DEFAULT_PITCH | FF_DONTCARE,        // Pitch and family
        _T("Ariel")                         // Font name
    );

    // Set Device Font.
    CFont* originalFont = deviceContext->SelectObject(&font);

    // Set Text Color.
    deviceContext->SetTextColor(RGB(100, 100, 100));

    unsigned short rectangleHeight{ 
        static_cast<unsigned short>(viewport.Height() * 0.009) }; // 0.9% of the Height.

    unsigned short rectangleWidth{ 
        static_cast<unsigned short>(viewport.Width() * 0.021) }; // 2.1% of the Width

    int rectangleLeft{ 
        static_cast<int>(viewport.right - viewport.Width() * 0.21) }; // 21% of the Width.

    int rectangleRight{ rectangleLeft + rectangleWidth };

    int textLeft{ 
        static_cast<int>(rectangleRight + viewport.Width() * 0.007) }; // 0.7% of Gap.

    int dataTextLeft{ 
        static_cast<int>(textLeft + viewport.Width() * 0.10) };

    int initialTextTop{ static_cast<int>(viewport.top + viewport.Height() * 0.075) }; // 0.76% of the Height.
    int textTop{ initialTextTop };

    int rectanlgeTop{ (2 * textTop + fontHeight - rectangleHeight) / 2 };
    int rectangleBottom{ rectanlgeTop + rectangleHeight };

    // Draw Text.
    CString text{};

    for (const auto& item : m_curveLegendItems)
    {
        // Create Brush.
        CBrush brush;
        brush.CreateSolidBrush(item.m_color);

        // Set Device Brush.
        CBrush* originalBrush = deviceContext->SelectObject(&brush);

        // Define Rectangle Bounding Volume.
        CRect rectangle(
            rectangleLeft, rectanlgeTop, 
            rectangleRight, rectangleBottom);

        // Draw Rectangle.
        deviceContext->Rectangle(&rectangle);

        // Restore Brush.
        deviceContext->SelectObject(originalBrush);

        // Set Trasnparent Background.
        int originalBackgroundMode = deviceContext->SetBkMode(TRANSPARENT);
     
        text.Format(
            _T("%ls : %ls"), 
            item.m_mnemonic, item.m_unit);

        deviceContext->TextOut(textLeft, textTop, text);

        text.Format(_T("%.4f"), item.m_data);

        deviceContext->TextOut(dataTextLeft, textTop, text);

        // Restore Background.
        deviceContext->SetBkMode(originalBackgroundMode);

        textTop += fontHeight + viewport.Height() * 0.015; // 1.5% Gap among items.
        rectanlgeTop = (2 * textTop + fontHeight - rectangleHeight) / 2;
        rectangleBottom = rectanlgeTop + rectangleHeight;
    }

    // Create Pen.
    CPen pen{ PS_DOT, 3, RGB(180, 180, 180) };
    CPen* originalPen = deviceContext->SelectObject(&pen);

    // Draw Legend Boundary.
    int boundaryLeft{ static_cast<int>(viewport.right - viewport.Width() * 0.23) };
    int boundaryTop{ static_cast<int>(viewport.top + viewport.Height() * 0.046) };
    int boundaryRight{ static_cast<int>(viewport.right - viewport.Width() * 0.014) };

    deviceContext->MoveTo(boundaryLeft, boundaryTop);
    deviceContext->LineTo(boundaryRight, boundaryTop);
    deviceContext->LineTo(boundaryRight, viewport.top + rectangleBottom);
    deviceContext->LineTo(boundaryLeft, viewport.top + rectangleBottom);
    deviceContext->LineTo(boundaryLeft, boundaryTop);

    // Restore Pen.
    deviceContext->SelectObject(originalPen);
    pen.DeleteObject();

    // Restore original font.
    deviceContext->SelectObject(originalFont);
    font.DeleteObject();

    // Lengend Data Rect.
    m_legendDataRect.left = dataTextLeft;
    m_legendDataRect.top = initialTextTop;
    m_legendDataRect.right = static_cast<int>(viewport.right - viewport.Width() * 0.015);
    m_legendDataRect.bottom = textTop;
}

void CMainView::DrawScale()
{
    // Get Device Context.
    CDC* deviceContext{ GetDC() };

    // Get Viewport Bounding Volume.
    CRect viewport{};
    GetClientRect(&viewport);

    // Create Font.
    CFont font{};
    font.CreateFont(
        static_cast<int>(viewport.Height() * 0.015),    // Font height (size)
        0,                                              // Font width
        0,                                              // Escapement angle
        0,                                              // Orientation angle
        FW_BOLD,                                        // Weight (normal)
        FALSE,                                          // Italic
        FALSE,                                          // Underline
        0,                                              // Strikeout
        ANSI_CHARSET,                                   // Character set
        OUT_DEFAULT_PRECIS,                             // Output precision
        CLIP_DEFAULT_PRECIS,                            // Clipping precision
        DEFAULT_QUALITY,                                // Quality
        DEFAULT_PITCH | FF_DONTCARE,                    // Pitch and family
        _T("Ariel")                                     // Font name
    );

    // Set Device Font.
    CFont* originalFont = deviceContext->SelectObject(&font);

    // Set Text Color.
    deviceContext->SetTextColor(RGB(100, 100, 100));

    size_t numberOfCurves = m_las->GetNumberOfCurves();
    size_t numberOfCurveDetails = m_las->GetNumberOfCurveDetails();

    double minimumDepth = m_las->GetMinimumDepth();
    double maximumDepth = m_las->GetMaximumDepth();

    std::string depthUnit = m_las->GetCurveInformation()[0].m_unit;

    const std::vector<double>& logData = m_las->GetAsciiLogData();

    // Calcuate Step Size.
    int stepSize{ 1 };

    if (numberOfCurves > 6)
    {
        double numberOfIntervals{ 6.0 };
        stepSize = (maximumDepth - minimumDepth) /
            (numberOfIntervals - 1.0);
    }

    // Set Trasnparent Background.
    int originalBackgroundMode = deviceContext->SetBkMode(TRANSPARENT);
    CString text{};

    for (size_t i = 0; i < numberOfCurves; i += stepSize)
    {
        double depth = logData[i * numberOfCurveDetails];
        long textY = (depth - minimumDepth) * m_yScale + m_yOffset;

        text.Format(_T("%d %ls"), (int)depth, CString(depthUnit.c_str()));

        deviceContext->TextOut(0, textY, text);
    }

    // Restore Background.
    deviceContext->SetBkMode(originalBackgroundMode);

    // Create Pen.
    CPen pen{ PS_DOT, 2, RGB(100, 100, 100) };
    CPen* originalPen = deviceContext->SelectObject(&pen);

    int originLeft{ static_cast<int>(m_xOffset - viewport.Width() * 0.002) };

    // Draw Horizontal Scale Line.
    deviceContext->MoveTo(originLeft, m_yOffset);
    deviceContext->LineTo(viewport.Width() - m_xMargin + m_xOffset, m_yOffset);

    // Draw Vertical Scale Line.
    deviceContext->MoveTo(originLeft, m_yOffset);
    deviceContext->LineTo(originLeft, viewport.Height());

    // Restore Pen.
    deviceContext->SelectObject(originalPen);
    pen.DeleteObject();

    // Restore original font.
    deviceContext->SelectObject(originalFont);
    font.DeleteObject();
}

void CMainView::UpdateCurvePoints()
{
    if (!m_las)
        return;

    size_t numberOfCurves = m_las->GetNumberOfCurves();
    size_t numberOfCurveDetails = m_las->GetNumberOfCurveDetails();

    // Release previous memory.
    ReleaseCurvePoints();

    m_curvePoints = new POINT[numberOfCurves];
    const std::vector<double>& logData = m_las->GetAsciiLogData();

    double minimumDataValue = m_las->GetMinimumDataValue();
    double minimumDepth = m_las->GetMinimumDepth();

    for (size_t i = 0; i < numberOfCurves; ++i)
    {
        m_curvePoints[i].x = (logData[i * numberOfCurveDetails +
            m_comboBoxCurves.GetCurSel() + 1] -
            minimumDataValue) * m_xScale + m_xOffset;

        m_curvePoints[i].y = (logData[i * numberOfCurveDetails] -
            minimumDepth) * m_yScale + m_yOffset;
    }

    // Update the Window to reflect changes.
    Invalidate();
    UpdateWindow();
}

void CMainView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    // Disable ComboBox.
    m_comboBoxCurves.EnableWindow(FALSE);
}


void CMainView::OnCbnSelchangeComboCurves()
{
    UpdateCurvePoints();
}


void CMainView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
    // Get Document
    auto* document = static_cast<CMainDocument*>(GetDocument());

    // Get LAS
    m_las = document->GetLas();

    // Get LAS Parser Message;
    m_lasParserMessage = document->GetLasParserMessage();
    
    if (m_las)
    {
        // Clear previous curves from ComboBox.
        if (m_comboBoxCurves.GetCount() > 0)
        {          
            m_comboBoxCurves.ResetContent();
        }

        // Clear Curve Legend Items.
        if (m_curveLegendItems.size() > 0)
        {
            m_curveLegendItems.clear();
        }

        // Set up a random number generator
        std::random_device randomDevice{};
        std::mt19937 randomGenerator(randomDevice());
        std::uniform_int_distribution<int> colorRange(0, 255);

        // Populate ComboBox and legend items with new curve entries.
        DelimitedLineSection const& curveInformation = m_las->GetCurveInformation();

        for (size_t i = 1; i < curveInformation.size(); ++i)
        {
            const auto& delimitedLine = curveInformation[i];

            m_comboBoxCurves.AddString(
                CString(delimitedLine.m_mnemonic.c_str()));

            CURVE_LEGEND_ITEM item{};

            item.m_color = RGB(
                colorRange(randomGenerator),
                colorRange(randomGenerator),
                colorRange(randomGenerator));

            item.m_mnemonic = CString(delimitedLine.m_mnemonic.c_str());
            item.m_unit = CString(delimitedLine.m_unit.c_str());

            m_curveLegendItems.push_back(item);
        }

        Transform();        

        // Enable ComboBox.
        m_comboBoxCurves.EnableWindow(TRUE);

        // Set Current Selection.
        m_comboBoxCurves.SetCurSel(0);
     
        // Enable Colors Button.
        GetDlgItem(IDC_BUTTON_CHANGE_COLORS)->EnableWindow(TRUE);

        // Update Curve Points.
        UpdateCurvePoints();
    }
    else // Reset state.
    {
        if (m_comboBoxCurves.GetCount() > 0)
        {
            m_comboBoxCurves.ResetContent();
        }

        // Disable ComboBox.
        m_comboBoxCurves.EnableWindow(FALSE);

        // Disable Color Button.
        GetDlgItem(IDC_BUTTON_CHANGE_COLORS)->EnableWindow(FALSE);

        ReleaseCurvePoints();

        // Clear Legend Items.
        m_curveLegendItems.clear();

        m_xScale = 0;
        m_yScale = 0;

        m_xOffset = 0;
        m_yOffset = 0;

        m_xMargin = 0;
        m_yMargin = 0;
    }

    // Update the View to reflect changes.
    Invalidate();
    UpdateWindow();
}


void CMainView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    Transform();
    UpdateCurvePoints();
}


void CMainView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_las)
        return;

    POINT mousePoint{ point.x, point.y };
    POINT* curvePointsEnd = m_curvePoints + m_las->GetNumberOfCurves();

    // Binary search on points.
    POINT* curvePoint = std::lower_bound(
        m_curvePoints, curvePointsEnd, 
        mousePoint, [](const POINT& p1, const POINT& p2) 
        { return p1.y < p2.y; });

    // If curve point is found.
    if (curvePoint != curvePointsEnd && 
        curvePoint->y == mousePoint.y) 
    {
        // Calculate the index using pointer arithmetic.
        int index = curvePoint - m_curvePoints;

        const std::vector<double>& logData = m_las->GetAsciiLogData();
        size_t numberOfCurveDetails = m_las->GetNumberOfCurveDetails();

        // Get Depth.
        double depth = logData[index * numberOfCurveDetails];

        // Populate Legend Items with data against depth found.
        for (int i = 0; i < m_curveLegendItems.size(); ++i)
        {
            m_curveLegendItems[i].m_data = 
                logData[index * numberOfCurveDetails + (i + 1)];
        }

        // Repaint Legend Data.
        InvalidateRect(m_legendDataRect);
        UpdateWindow();
    }

    CFormView::OnMouseMove(nFlags, point);
}


void CMainView::OnBnClickedButtonChangeColors()
{
    if (!m_las)
        return;

    // Set up a random number generator
    std::random_device randomDevice{};
    std::mt19937 randomGenerator(randomDevice());
    std::uniform_int_distribution<int> colorRange(0, 255);

    // Update Colors.
    for (auto& item : m_curveLegendItems)
    {
        item.m_color = RGB(
            colorRange(randomGenerator),
            colorRange(randomGenerator),
            colorRange(randomGenerator));
    }

    // Repaint View.
    Invalidate();
    UpdateWindow();
}
