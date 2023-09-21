#pragma once
#include "Las.h"


struct CURVE_LEGEND_ITEM
{
	COLORREF m_color{};
	CString m_mnemonic{};
	CString m_unit{};
	double m_data{};

	CURVE_LEGEND_ITEM() = default;

	CURVE_LEGEND_ITEM(
		COLORREF color, CString name, 
		CString unit, double value) : 
		m_color(color), m_mnemonic(name), 
		m_unit(unit), m_data(value) {}
};

// CMainView form view
class CMainView : public CFormView
{
	DECLARE_DYNCREATE(CMainView)

protected:
	CMainView();           // protected constructor used by dynamic creation
	virtual ~CMainView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_VIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFilenamePane(CCmdUI* pCmdUI);

	
private:
	
	CComboBox m_comboBoxCurves{};
	CRect m_legendDataRect{};

	const int m_gridCellMargin{ 20 };

	double m_xScale{};
	double m_yScale{};

	double m_xOffset{};
	double m_yOffset{};

	double m_xMargin{};
	double m_yMargin{};
	
	const Las* m_las{ nullptr };
	const CString* m_lasParserMessage{ nullptr };

	POINT* m_curvePoints{ nullptr };
	std::vector<CURVE_LEGEND_ITEM> m_curveLegendItems{};

	void ReleaseCurvePoints();
	void Transform();

	void DrawLenged();
	void DrawScale();
	void UpdateCurvePoints();
	
	virtual void OnDraw(CDC* /*pDC*/);
	virtual void OnInitialUpdate();
	afx_msg void OnCbnSelchangeComboCurves();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonChangeColors();
};


