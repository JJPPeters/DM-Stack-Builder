template <typename T>
void DMImage<T>::GetData(std::vector<T> &destination)
{
	int n = getWidth() * getHeight() * getDepth();
	if (destination.size() < n)
		throw std::invalid_argument("GetData - Array size must be larger than or equal to image size.");

	Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
	T* idata = static_cast<T*>(iLocker.get());

	std::copy(idata, idata + n, destination.begin());
}

template <typename T>
void DMImage<T>::GetSliceData(std::vector<T> &destination, int front, int back)
{
	int n = getWidth() * getHeight() * (back - front);
	if (destination.size() < n)
		throw std::invalid_argument("GetData - Array size must be larger than or equal to slices size.");

	Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
	T* idata = static_cast<T*>(iLocker.get());

	std::copy(idata, idata + n, destination.begin());
}

template <typename T>
void DMImage<T>::SetData(std::vector<T> &newdata)
{
	int n = getWidth() * getHeight() * getDepth();
	if (newdata.size() > n)
		throw std::invalid_argument("GetData - Array size must be smaller than or equal to image size.");

	Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
	T* idata = static_cast<T*>(iLocker.get());

	std::copy(newdata.begin(), newdata.end(), idata);
}

template <typename T>
void DMImage<T>::SetSliceData(std::vector<T> &newdata, int front, int back)
{
	int n = getWidth() * getHeight() * (back - front);
	if (newdata.size() > n)
		throw std::invalid_argument("GetData - Array size must be smaller than or equal to slices size.");

	Gatan::PlugIn::ImageDataLocker iLocker = Gatan::PlugIn::ImageDataLocker(Image);
	T* idata = static_cast<T*>(iLocker.get());

	std::copy(newdata.begin(), newdata.end(), idata + getWidth() * getHeight() * front);
}