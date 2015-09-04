#pragma once

#include "stdafx.h"

#include "Resource.h" // contains IDs of controls
#include "NumEdit.h" // custom control?

// boost
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"

#include "StackBuilder.h"
#include <deque>

class CDMDialog : public CDialog, public DMWorker
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

	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnBuild();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnEdtChangedSlices();
	afx_msg void OnBnClickedBtnRemove();
private:
	CButton chk_fixed;
	CButton chk_expand;
	CString s_Slices;
	CNumEdit edt_Slices;

	int numSlices;

	CListCtrl lst_Images;

	boost::shared_ptr<boost::mutex> dialogmtx;
	boost::mutex buildermtx;
	boost::mutex workmtx;

	std::list<boost::shared_ptr<StackBuilder>> builders;

	void addBuilder(DMImageGeneric im);

	void removeBuilder(unsigned long id);

	void addListItem(std::string name, std::string id);

	void removeListID(unsigned long id);

	void DoWork();
};
