#include "TopoBuilder.h"

HRESULT TopoBuilder::RenderURL(PCWSTR sURL)
{
	HRESULT hr = S_OK;

	hr = CreateMediaSource(sURL);
	if (FAILED(hr)) return hr;

	hr = CreateTopology();
	return hr;
}

IMFTopology* TopoBuilder::GetTopology()
{
	return Topology;
}

HRESULT TopoBuilder::CreateMediaSource(PCWSTR sURL)
{
	HRESULT hr = S_OK;
	MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;
	CComPtr<IMFSourceResolver> sourceResolver;
	CComPtr<IUnknown> source;

	hr = MFCreateSourceResolver(&sourceResolver);
	if (FAILED(hr)) return hr;

	hr = sourceResolver->CreateObjectFromURL(
		sURL, MF_RESOLUTION_MEDIASOURCE |
		MF_RESOLUTION_CONTENT_DOES_NOT_HAVE_TO_MATCH_EXTENSION_OR_MIME_TYPE,
		NULL, &objectType, &source);
	if (FAILED(hr)) return hr;

	Source = source;
	if (Source == NULL) hr = E_NOINTERFACE;

	return hr;
}

HRESULT TopoBuilder::ShutdownSource()
{
	HRESULT hr = S_OK;

	if (Source != NULL)
	{
		hr = Source->Shutdown();
		Source.Release();
	}
	else
		hr = E_UNEXPECTED;

	return hr;
}

HRESULT TopoBuilder::CreateTopology()
{
	HRESULT hr = S_OK;
	CComPtr<IMFPresentationDescriptor> presDescriptor;
	DWORD sourceStreamsCount = 0;

	Topology.Release();

	hr = MFCreateTopology(&Topology);
	if (FAILED(hr)) return hr;

	hr = Source->CreatePresentationDescriptor(&presDescriptor);
	if (FAILED(hr)) return hr;

	hr = presDescriptor->GetUINT64(MF_PD_DURATION, (UINT64*)&duration);

	hr = presDescriptor->GetStreamDescriptorCount(&sourceStreamsCount);
	if (FAILED(hr)) return hr;

	for (DWORD x = 0; x < sourceStreamsCount; x++) 
	{
		hr = AddBranchToPartialTopology(presDescriptor, x);
		if (FAILED(hr)) 
		{
			hr = presDescriptor->DeselectStream(x);
			if (FAILED(hr)) break;
		}
	}

	return hr;
}

HRESULT TopoBuilder::AddBranchToPartialTopology(IMFPresentationDescriptor* presDescriptor, DWORD index)
{
	HRESULT hr = S_OK;
	CComPtr<IMFStreamDescriptor> streamDescriptor;
	CComPtr<IMFTopologyNode> sourceNode;
	CComPtr<IMFTopologyNode> outputNode;
	BOOL IsSelected = FALSE;

	if (Topology == NULL) return E_UNEXPECTED;

	hr = presDescriptor->GetStreamDescriptorByIndex(index, &IsSelected, &streamDescriptor);
	if (FAILED(hr)) return hr;

	if (IsSelected) 
	{
		hr = CreateSourceStreamNode(presDescriptor, streamDescriptor, sourceNode);
		if (FAILED(hr)) return hr;

		hr = CreateOutputNode(streamDescriptor, outputNode);
		if (FAILED(hr)) return hr;

		hr = Topology->AddNode(sourceNode);
		if (FAILED(hr)) return hr;

		hr = Topology->AddNode(outputNode);
		if (FAILED(hr)) return hr;

		hr = sourceNode->ConnectOutput(0, outputNode, 0);
	}

	return hr;
}

HRESULT TopoBuilder::CreateSourceStreamNode(IMFPresentationDescriptor* presDescriptor, IMFStreamDescriptor* streamDescriptor, CComPtr<IMFTopologyNode>& node)
{
	HRESULT hr = S_OK;
	node = NULL;

	if (presDescriptor == NULL) return E_UNEXPECTED;
	if (streamDescriptor == NULL) return E_UNEXPECTED;
	
	hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &node);
	if (FAILED(hr)) return hr;

	hr = node->SetUnknown(MF_TOPONODE_SOURCE, Source);
	if (FAILED(hr))
	{
		node = NULL;
		return hr;
	}

	hr = node->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, presDescriptor);
	if (FAILED(hr))
	{
		node = NULL;
		return hr;
	}

	hr = node->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, streamDescriptor);
	if (FAILED(hr)) node = NULL;

	return hr;
}

HRESULT TopoBuilder::CreateOutputNode(IMFStreamDescriptor* streamDescriptor, CComPtr<IMFTopologyNode>& node)
{
	HRESULT hr = S_OK;
	CComPtr<IMFMediaTypeHandler> handler;
	CComPtr<IMFActivate> renderActivate;
	GUID majorType = GUID_NULL;
	node = NULL;

	if (streamDescriptor == NULL) return E_UNEXPECTED;

	hr = streamDescriptor->GetMediaTypeHandler(&handler);
	if (FAILED(hr)) return hr;

	hr = handler->GetMajorType(&majorType);
	if (FAILED(hr)) return hr;

	if (majorType == MFMediaType_Audio)
		hr = MFCreateAudioRendererActivate(&renderActivate);
	else
		hr = E_FAIL;

	if (FAILED(hr)) return hr;

	hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &node);
	if (FAILED(hr)) return hr;

	hr = node->SetObject(renderActivate);
	if (FAILED(hr)) node = NULL;

	return hr;
}
