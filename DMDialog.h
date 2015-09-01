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

	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnEdtChangedExposure();
private:
	CButton chk_STEM;
	CButton chk_TEM;
	CString s_Exposure;
	CNumEdit edt_Exposure;

	boost::shared_ptr<boost::mutex> dialogmtx;
	boost::mutex buildermtx;
	boost::mutex workmtx;

	std::list<boost::shared_ptr<StackBuilder>> builders;

	void addBuilder(StackBuilder tobuild)
	{
		// TODO: don't allow duplicates
		while (!buildermtx.try_lock()) { }
		//boost::lock_guard<boost::mutex> lock(buildermtx);
		builders.push_back(boost::make_shared<StackBuilder>(tobuild));
		buildermtx.unlock();

		// set equivalent of push_back
		// builders.insert(builders.end(), boost::make_shared<StackBuilder>(tobuild));
	}

	//boost::shared_ptr<StackBuilder> getBuilder(int index)
	//{
	//	boost::lock_guard<boost::mutex> lock(buildermtx);
	//	return builders[index];
	//}

	void DoWork()
	{
		boost::lock_guard<boost::mutex> lock(workmtx);
		while (!IsStopping())
		{
			boost::lock_guard<boost::mutex> lock(buildermtx);
			std::list<boost::shared_ptr<StackBuilder>>::iterator it = builders.begin();
			while (it != builders.end())
			{
				// post-increment operator returns a copy, then increment
				//std::list<boost::shared_ptr<StackBuilder>>::iterator current = it++;
				boost::shared_ptr<StackBuilder> currentbuilder = *it;
				//DMresult << "testing if open" << DMendl;
				bool isopen = currentbuilder->watchedImage.IsOpen();
				if (isopen)
					++it;
				else
					it = builders.erase(it);
			}

			// spam this!!
			//DMresult << "Number watched: " << builders.size() << DMendl;
			Sleep(100);
		}
	}
};
