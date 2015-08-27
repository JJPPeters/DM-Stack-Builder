#pragma once

#include "stdafx.h"

#include "Resource.h" // contains IDs of controls
#include "NumEdit.h" // custom control?

// boost
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"

#include "StackBuilder.h"

class CDMDialog : public CDialog
{
	DECLARE_DYNCREATE(CDMDialog)

public:
	CDMDialog(CWnd* pParent = NULL);  // standard constructor
	virtual ~CDMDialog();

	enum { IDD = IDD_DMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(UINT templateID, CWnd* pParentWnd = NULL);
	
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnBnClickedBtnGpa();
private:
	CComboBox combo_CLdev;
	CComboBox combo_method;
	CNumEdit txt_bfactor;
	CString s_bfactor;
	CNumEdit txt_thresh;
	CString s_thresh;
	CProgressCtrl progressBar;
	CButton chk_XCF;
	CButton chk_PCF;

	boost::shared_ptr<boost::mutex> dialogmtx;
	boost::mutex progressmtx; // is this needed

	boost::shared_ptr<StackBuilder> builder;
public:

	void SetProgressRange(int start, int end) { boost::lock_guard<boost::mutex> lock(progressmtx); progressBar.SetRange(start, end); }
	void SetProgressPos(int pos) { boost::lock_guard<boost::mutex> lock(progressmtx); progressBar.SetPos(pos); }
};
