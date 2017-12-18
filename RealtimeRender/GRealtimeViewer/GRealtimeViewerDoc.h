// GRealtimeViewerDoc.h : interface of the CGRealtimeViewerDoc class
//


#pragma once

// namespace girl
// {

class CGRealtimeViewerDoc : public CDocument
{
protected: // create from serialization only
	CGRealtimeViewerDoc();
	DECLARE_DYNCREATE(CGRealtimeViewerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CGRealtimeViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
// 
// 
//}//namespace