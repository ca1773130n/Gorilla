// GRealtimeViewerDoc.cpp : implementation of the CGRealtimeViewerDoc class
//
#include "stdafx.h"
#include "GRealtimeViewer.h"
#include "GRealtimeViewerDoc.h"

#include "MainFrm.h"
#include <conio.h>

//using namespace girl;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGRealtimeViewerDoc

IMPLEMENT_DYNCREATE(CGRealtimeViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGRealtimeViewerDoc, CDocument)
END_MESSAGE_MAP()


// CGRealtimeViewerDoc construction/destruction

CGRealtimeViewerDoc::CGRealtimeViewerDoc()
{
	// TODO: add one-time construction code here

}

CGRealtimeViewerDoc::~CGRealtimeViewerDoc()
{
}

BOOL CGRealtimeViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CGRealtimeViewerDoc serialization

void CGRealtimeViewerDoc::Serialize(CArchive& ar)
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


// CGRealtimeViewerDoc diagnostics

#ifdef _DEBUG
void CGRealtimeViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGRealtimeViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGRealtimeViewerDoc commands
BOOL CGRealtimeViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	_cprintf ("%s\n", lpszPathName);
	AfxMessageBox (lpszPathName);

 	((CMainFrame*)AfxGetMainWnd ())->SetOpenFile (lpszPathName);

	return TRUE;
}
