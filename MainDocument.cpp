
// MainDocument.cpp : implementation of the CMainDocument class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "las-parser.h"
#endif

#include "MainDocument.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainDocument

IMPLEMENT_DYNCREATE(CMainDocument, CDocument)

BEGIN_MESSAGE_MAP(CMainDocument, CDocument)
END_MESSAGE_MAP()


// CMainDocument construction/destruction

CMainDocument::CMainDocument() noexcept
{
	// TODO: add one-time construction code here

}

CMainDocument::~CMainDocument()
{
	ReleaseLas();
}

BOOL CMainDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CMainDocument serialization

void CMainDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CMainDocument::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CMainDocument::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMainDocument::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMainDocument diagnostics

#ifdef _DEBUG
void CMainDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CMainDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMainDocument commands
void CMainDocument::ReleaseLas()
{
	// If occupied, release memory.
	if (m_las)
	{
		delete m_las;
		m_las = nullptr;
	}
}

void CMainDocument::InitializeLas(const CString& filename)
{
	// If occupied, release memory.
	ReleaseLas();

	std::string resultMessage{};

	// Parse LAS File.
	m_las = LasParser::Parse(CStringA(filename).GetBuffer(), resultMessage);
	m_lasParserMessage = CString(resultMessage.c_str());

	// Update All Views.
	UpdateAllViews(NULL);
}

const Las* CMainDocument::GetLas() const
{
	return m_las;
}

const CString* CMainDocument::GetLasParserMessage() const
{
	return &m_lasParserMessage;
}


