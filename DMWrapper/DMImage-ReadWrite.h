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