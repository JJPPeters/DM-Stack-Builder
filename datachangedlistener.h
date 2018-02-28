#pragma once
#include "stdafx.h"
#include "activeimagelistener.h"

class ActiveImageListener;

class DataChangedListener : public boost::enable_shared_from_this<DataChangedListener>
{
private:
	ulong_ptr_t DataChangedHandler;

	ActiveImageListener* parent;

public:
	DataChangedListener(ActiveImageListener* par) : parent(par) {}
	~DataChangedListener() {};

	void AddDataChangedListener(const DM::Image &image);

	void OnDataChanged(uint32 img_event_flags, const DM::Image &image);

	void RemoveDataChangedListener(const DM::Image &image);
};

