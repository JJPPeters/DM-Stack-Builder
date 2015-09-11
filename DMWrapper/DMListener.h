#include "DMListenable.h"
#include "DMROI.h"
#include "DMout.h"
#include "boost/shared_ptr.hpp"
#include <boost/enable_shared_from_this.hpp>

#include "boost/thread/mutex.hpp"
#include "boost/thread.hpp"

class DMListener
{
private:
	bool Active;

protected:
	std::string fPrefix;
	std::vector<DMListenable*> Listeners;

	boost::mutex _mtx;
	boost::mutex activemtx;

	// so event handler can be closed
	ulong_ptr_t EventHandler;

	DMListener(const char_range &prefix) : fPrefix(prefix.begin(), prefix.end()), Active(false) {}

	DMListener(const DMListener& other) : EventHandler(other.EventHandler), Active(other.Active), fPrefix(other.fPrefix), Listeners(other.Listeners) {}

	~DMListener() {}

public:
	// Add a DMListenable class to be called on the changed functions
	void AddListenable(DMListenable* listenable);

	//
	void RemoveListenable(DMListenable* listenable);

	//
	void ClearListenables() { boost::lock_guard<boost::mutex> lock(activemtx); Listeners.clear(); }

	//
	bool IsActive() { boost::lock_guard<boost::mutex> lock(activemtx); return Active; }

	//
	void SetActive(bool state) { boost::lock_guard<boost::mutex> lock(activemtx); Active = state; }

	//
	void SetEventHandler(ulong_ptr_t eh) { EventHandler = eh; }

	//
	ulong_ptr_t GetEventHandler() { return EventHandler; }
};


class DMImageListener : public boost::enable_shared_from_this<DMImageListener>, public DMListener
{
private:
	boost::shared_ptr<DMImageListener> get_this_shared() { return shared_from_this(); }

public:
	// create instance without any listeners added
	DMImageListener(const char_range &prefix) : DMListener(prefix) { DMresult << "Created. Is active: " << IsActive() << DMendl; };

	// 
	DMImageListener(const DMImageListener& other) : DMListener(other) { DMresult << "Copied. Is active: " << IsActive() << DMendl; }

	//
	~DMImageListener() {}

	// create and return an instance of this class with a listener added to the images
	static boost::shared_ptr<DMImageListener> AddImageListener_Ptr(const DM::Image &img, const std::string &prefix);

	// This is what will happen when the data changes.
	void OnDataChanged(uint32 img_event_flags, const DM::Image &img);

	// 
	void AddImageEventListener(DM::Image &img);

	//
	void RemoveImageEventListener(DM::Image &image);
};


class DMDisplayListener : public boost::enable_shared_from_this<DMDisplayListener>, public DMListener
{
private:
	boost::shared_ptr<DMDisplayListener> get_this_shared() { return shared_from_this(); }

	std::vector<DMROI*> ROIs;

public:
	// create instance without any listeners added
	DMDisplayListener(const char_range &prefix) : DMListener(prefix) {}

	// 
	DMDisplayListener(const DMDisplayListener& other) : DMListener(other), ROIs(other.ROIs) {}

	~DMDisplayListener() {}

	// create and return an instance of this class with a listener added to the image display
	static boost::shared_ptr<DMDisplayListener> AddImageDisplayListener_Ptr(const DM::ImageDisplay &img_disp, const std::string &prefix);

	// This is what will happen when the ROI changes.
	void OnROIChanged(uint32 img_disp_event_flags, const DM::ImageDisplay &img_disp, uint32 roi_change_flag, uint32 roi_disp_change_flags, const DM::ROI &roi);

	//
	void AddDisplayEventListener(DM::ImageDisplay &disp);

	//
	void RemoveDisplayEventListener(DM::ImageDisplay &disp);

	// Add an ROI to listen to
	void AddROI(DMROI* roi);

	// Add an ROI to listen to
	void RemoveROI(DMROI* roi);

	//
	void ClearROIs() { ROIs.clear(); }
};