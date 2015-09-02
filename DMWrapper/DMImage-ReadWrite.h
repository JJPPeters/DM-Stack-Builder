template <typename T>
void DMImage::GetSliceData(std::vector<T> &destination, int front, int back)
{
	//DMresult << "GET: Array size: " << destination.size() << ", width = " << getWidth() << ", height = " << getHeight() << ", front = " << front << ", back = " << back << DMendl;

	int n = getWidth() * getHeight() * (back - front);
	if (destination.size() != n)
		throw std::invalid_argument("Array sizes must match");

	Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
	T* idata = static_cast<T*>(iLocker.get());

	std::copy(idata, idata + n, destination.begin());
}

template <typename T>
void DMImage::SetSliceData(std::vector<T> &newdata, int front, int back)
{
	//DMresult << "SET: Array size: " << newdata.size() << ", width = " << getWidth() << ", height = " << getHeight() << ", front = " << front << ", back = " << back << DMendl;
	
	int n = getWidth() * getHeight() * (back - front);
	if (newdata.size() != n)
		throw std::invalid_argument("Array sizes must match");

	Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
	T* idata = static_cast<T*>(iLocker.get());

	std::copy(newdata.begin(), newdata.end(), idata + getWidth() * getHeight() * front);
}

//template <typename T>
//void DMImage::SetRealData(const std::vector<T> &source, int offset)
//{
//	if (source.size() > (width * height * depth) - offset)
//		throw std::invalid_argument("Array sizes must match");
//
//	if (Image.IsDataTypeFloat())
//		SetRealLockerData<float, T>(source, offset);
//	else if (Image.IsDataTypeInteger())
//		SetRealLockerData<int, T>(source, offset);
//	else if (Image.IsDataTypeSignedInteger())
//		SetRealLockerData<int, T>(source, offset);
//	else if (Image.IsDataTypeUnsignedInteger())
//		SetRealLockerData<uint, T>(source, offset);
//	else
//		throw std::invalid_argument("Data type not supported");
//}
//
//template <typename T>
//void DMImage::SetRealData(const std::vector<T> &source)
//{
//	SetRealData(source, 0);
//}
//
//template <typename U, typename T>
//void DMImage::SetRealLockerData(const std::vector<T> &source, int offset)
//{
//
//	Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
//	U* idata = static_cast<U*>(iLocker.get());
//	for (int j = 0; j < (source.size()); j++)
//		idata[j+offset] = static_cast<U>(source[j]);
//	iLocker.~ImageDataLocker();
//	Image.DataChanged();
//}