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
		// wait to get the lock, this seems dodgy.
		while (!buildermtx.try_lock()) { }

		// find if this image already exists

		bool exists = false;

		std::list<boost::shared_ptr<StackBuilder>>::iterator it = builders.begin();
		while (it != builders.end())
		{
			boost::shared_ptr<StackBuilder> current = *it;

			if (current->GetImageID() == tobuild.GetImageID())
			{
				exists = true;
			}
			++it;
		}

		if (exists)
		{
			DMresult << "Already watching this image." << DMendl;
			return;
		}

		builders.push_back(boost::make_shared<StackBuilder>(tobuild));
		buildermtx.unlock();

		// set equivalent of push_back, for reference
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
				boost::shared_ptr<StackBuilder> currentbuilder = *it;
				bool isopen = currentbuilder->IsImageOpen();
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
