#pragma once

#include "Common.h"
#include "MF.h"

class TopoBuilder
{
public:

	TopoBuilder() {};
	~TopoBuilder() { ShutdownSource(); };

	/// <summary>
	/// ������� ��������� �� URL
	/// </summary>
	/// <param name="sURL"> ������ ���� � ����� </param>
	/// <returns></returns>
	HRESULT RenderURL(PCWSTR sURL);

	/// <summary>
	/// �������� ��������� ���������
	/// </summary>
	/// <returns></returns>
    IMFTopology* GetTopology();

    /// <summary>
    /// ����������������� �����
    /// </summary>
    MFTIME duration;

private:

    /// <summary>
    /// ���������
    /// </summary>
    CComQIPtr<IMFTopology> Topology;

    /// <summary>
    /// MF ��������
    /// </summary>
    CComQIPtr<IMFMediaSource> Source;

    /// <summary>
    /// ������� MF ��������
    /// </summary>
    /// <param name="sURL"></param>
    /// <returns></returns>
    HRESULT CreateMediaSource(PCWSTR sURL);

    /// <summary>
    /// ������� MF ��������
    /// </summary>
    /// <returns></returns>
    HRESULT ShutdownSource();

    /// <summary>
    /// ������� ���������
    /// </summary>
    /// <returns></returns>
    HRESULT CreateTopology();

    /// <summary>
    /// �������� ����� ��������� ��� ������ ������
    /// </summary>
    /// <param name="presDescriptor"></param>
    /// <param name="index"></param>
    /// <returns></returns>
    HRESULT AddBranchToPartialTopology(
        IMFPresentationDescriptor* presDescriptor,
        DWORD index);

    /// <summary>
    /// ������� ���� ��������� ������
    /// </summary>
    /// <param name="presDescriptor"></param>
    /// <param name="streamDescriptor"></param>
    /// <param name="node"></param>
    /// <returns></returns>
    HRESULT CreateSourceStreamNode(
        IMFPresentationDescriptor* presDescriptor,
        IMFStreamDescriptor* streamDescriptor,
        CComPtr<IMFTopologyNode>& node);

    /// <summary>
    /// ������� ���� ��������� ������
    /// </summary>
    /// <param name="streamDescriptor"></param>
    /// <param name="node"></param>
    /// <returns></returns>
    HRESULT CreateOutputNode(
        IMFStreamDescriptor* streamDescriptor,
        CComPtr<IMFTopologyNode>& node);
};

