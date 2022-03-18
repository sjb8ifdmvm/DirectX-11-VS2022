#include "AdapterReader.h"

//�C�|�˸m�e��
std::vector<AdapterData> AdapterReader::adapters;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
	if (adapters.size() > 0)//�w�g��l��
		return adapters;

	Microsoft::WRL::ComPtr<IDXGIFactory1> pFactory;

	//�إ� DXGIFactory ����
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(pFactory.GetAddressOf()));

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"DXGIFactory�C�|�A�t���Ыإ���");
		ErrorLogger::Log(hr, L"Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	//�M���A�t��
	IDXGIAdapter1* pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters1(index, &pAdapter)))
	{
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return adapters;
}

AdapterData::AdapterData(IDXGIAdapter1* pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc1(&this->description);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"IDXGIAdapter�y�z�������");
		ErrorLogger::Log(hr, L"Failed to Get Description for IDXGIAdapter.");
	}
}