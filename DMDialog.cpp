#include "stdafx.h"
#include "DialogPlugIn.h"
#include "DMPluginStubs.h"
#include "DMDialog.h"
#include "DMWrapper/DMout.h"

#include"boost/lexical_cast.hpp"

IMPLEMENT_DYNCREATE(CDMDialog, CDialog)

BEGIN_MESSAGE_MAP(CDMDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_ADD, &CDMDialog::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &CDMDialog::OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_BUILD, &CDMDialog::OnBnClickedBtnBuild)
	ON_BN_CLICKED(IDC_BTN_STOP, &CDMDialog::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CDMDialog::OnBnClickedBtnPause)
	ON_EN_CHANGE(IDC_EDIT_SLICES, &CDMDialog::OnEdtChangedSlices)
END_MESSAGE_MAP()

CDMDialog::CDMDialog(CWnd* pParent) : CDialog(CDMDialog::IDD, pParent), dialogmtx(new boost::mutex), s_Slices("3"), numSlices(3)
{
	Start();
}

CDMDialog::~CDMDialog(){}

void CDMDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RDO_FIXED, chk_fixed);
	DDX_Control(pDX, IDC_RDO_EXPAND, chk_expand);
	DDX_Control(pDX, IDC_EDIT_SLICES, edt_Slices);
	DDX_Text(pDX, IDC_EDIT_SLICES, s_Slices);
	DDX_Control(pDX, IDC_LIST_IMAGES, lst_Images);
}

BOOL CDMDialog::Create(UINT templateID, CWnd* pParentWnd){return CDialog::Create(IDD, pParentWnd);}

void CDMDialog::OnPaint(){CDialog::OnPaint();}

BOOL CDMDialog::OnEraseBkgnd(CDC* pDC){return CDialog::OnEraseBkgnd(pDC);}

BOOL CDMDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	chk_fixed.SetCheck(1);

	lst_Images.InsertColumn(0, "Name", LVCFMT_LEFT, 80);
	lst_Images.InsertColumn(1, "ID", LVCFMT_CENTER, 30);

	return true;
}

void CDMDialog::OnBnClickedBtnAdd()
{
	try
	{
		// need to catch if there is no front image :D
		DMImageGeneric front = DMImageGeneric();
		front.fromFront();

		addBuilder(front);
	}
	catch (const std::exception& ex)
	{
		DMresult << ex.what() << DMendl;
		return;
	}
}

void CDMDialog::addBuilder(DMImageGeneric im)
{
	// wait to get the lock, this seems dodgy.
	// remember to unlock it at any exit point
	boost::lock_guard<boost::mutex> guard(*dialogmtx);
	//while (!buildermtx.try_lock()) {}

	// find if this image already exists

	bool exists = false;

	std::string title = im.GetTitle();
	long imid = im.getID();

	StackBuilder tobuild = StackBuilder(this, dialogmtx, im);

	std::list<boost::shared_ptr<StackBuilder>>::iterator it = builders.begin();
	while (it != builders.end())
	{
		boost::shared_ptr<StackBuilder> current = *it;

		if (current->GetImageID() == tobuild.GetImageID())
			exists = true;

		++it;
	}

	if (exists)
	{
		DMresult << "Already watching this image." << DMendl;
		//buildermtx.unlock(); // remember to unlock this!!
		return;
	}

	builders.push_back(boost::make_shared<StackBuilder>(tobuild));

	// add to the list box showing the watched images
	addListItem(title, boost::lexical_cast<std::string>(imid));

	//buildermtx.unlock(); // remember to unlock this!!
}

void CDMDialog::removeBuilder(unsigned long id)
{
	boost::lock_guard<boost::mutex> guard(*dialogmtx);
	//while (!buildermtx.try_lock()) {}

	std::list<boost::shared_ptr<StackBuilder>>::iterator it = builders.begin();
	while (it != builders.end())
	{
		boost::shared_ptr<StackBuilder> current = *it;

		if (current->GetImageID() == id)
			it = builders.erase(it); // keep iterator valid even after erase
		else
			++it;
	}

	//buildermtx.unlock();
}

void CDMDialog::OnBnClickedBtnRemove()
{
	//http://stackoverflow.com/questions/20839499/how-to-delete-selected-row-of-a-list-control-in-mfc
	UINT i, uSelectedCount = lst_Images.GetSelectedCount();
	int  nItem;
	for (i = 0; i < uSelectedCount; i++)
	{
		nItem = lst_Images.GetNextItem(-1, LVNI_SELECTED);
		ASSERT(nItem != -1);

		std::string sID(lst_Images.GetItemText(nItem, 1));

		unsigned long id = boost::lexical_cast<unsigned long>(sID);

		// remove from our std::list
		removeBuilder(id);

		// remove from list control
		lst_Images.DeleteItem(nItem);
	}
}

void CDMDialog::addListItem(std::string name, std::string id)
{
	int nitem = lst_Images.InsertItem(0, name.c_str());
	lst_Images.SetItemText(nitem, 1, id.c_str());
}

void CDMDialog::OnBnClickedBtnBuild()
{
	boost::lock_guard<boost::mutex> guard(*dialogmtx);
	//while (!buildermtx.try_lock()) {}

	std::list<boost::shared_ptr<StackBuilder>>::iterator it = builders.begin();
	while (it != builders.end())
	{
		boost::shared_ptr<StackBuilder> current = *it;

		// get selected type
		if (chk_fixed.GetCheck() == BST_CHECKED)
			current->StartBuildingFixed(numSlices);
		else if (chk_expand.GetCheck() == BST_CHECKED)
			current->StartBuildingExpanding(numSlices);
		++it;
	}

	//buildermtx.unlock();
}

void CDMDialog::OnBnClickedBtnStop()
{
	//DMImage selected = DMImage();
	//selected.fromFront();

	//for (int pos = 0; pos < builders.size(); ++pos)
	//	if (selected == builders[pos]->getImage())
	//	{
	//		builders[pos]->Stop();
	//		break;
	//	}
	//chk_STEM.EnableWindow(TRUE);
	//chk_TEM.EnableWindow(TRUE);
}



void CDMDialog::OnBnClickedBtnPause()
{
}

void CDMDialog::OnEdtChangedSlices()
{
	UpdateData();
	if (s_Slices == "")
	{
		s_Slices = "0";
		edt_Slices.SetWindowTextA(s_Slices);
		edt_Slices.SetSel(-1);
	}
	numSlices = boost::lexical_cast<int>(s_Slices);
}

void CDMDialog::DoWork()
{
	boost::lock_guard<boost::mutex> lock(workmtx);
	while (!IsStopping())
	{
		boost::lock_guard<boost::mutex> lock(buildermtx);
		std::list<boost::shared_ptr<StackBuilder>>::iterator it = builders.begin();
		while (it != builders.end())
		{
			boost::shared_ptr<StackBuilder> currentbuilder = *it; // crash here on closing DM (in debug mode)
			bool isopen = currentbuilder->IsImageOpen();
			unsigned long id = currentbuilder->GetImageID();
			if (isopen)
				++it;
			else
			{
				//erase from list control
				removeListID(id);
				// erase from list
				it = builders.erase(it);
			}
		}

		// spam this!!
		//DMresult << "Number watched: " << builders.size() << DMendl;
		Sleep(100);
	}
}

void CDMDialog::removeListID(unsigned long id)
{
	int  nItem = -1;
	int n = lst_Images.GetItemCount();

	for (int i = 0; i < n; ++i)
	{
		nItem = lst_Images.GetNextItem(nItem, LVNI_ALL);
		ASSERT(nItem != -1);

		if (nItem < 0 || nItem >= n)
			return;

		std::string sID(lst_Images.GetItemText(nItem, 1));
		unsigned long cID = boost::lexical_cast<unsigned long>(sID);

		if (cID == id)
		{
			// I think this ID is just what we currently have in 'i'
			lst_Images.DeleteItem(nItem);
		}
	}
}