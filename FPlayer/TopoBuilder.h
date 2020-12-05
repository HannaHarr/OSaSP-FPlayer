#pragma once

#include "Common.h"
#include "MF.h"

class TopoBuilder
{
public:

	TopoBuilder() {};
	~TopoBuilder() { ShutdownSource(); };

	/// <summary>
	/// Создает топологию по URL
	/// </summary>
	/// <param name="sURL"> строка пути к файлу </param>
	/// <returns></returns>
	HRESULT RenderURL(PCWSTR sURL);

	/// <summary>
	/// Получить созданную топологию
	/// </summary>
	/// <returns></returns>
    IMFTopology* GetTopology();

    /// <summary>
    /// Продолжительность файла
    /// </summary>
    MFTIME duration;

private:

    /// <summary>
    /// Топология
    /// </summary>
    CComQIPtr<IMFTopology> Topology;

    /// <summary>
    /// MF источник
    /// </summary>
    CComQIPtr<IMFMediaSource> Source;

    /// <summary>
    /// Создать MF источник
    /// </summary>
    /// <param name="sURL"></param>
    /// <returns></returns>
    HRESULT CreateMediaSource(PCWSTR sURL);

    /// <summary>
    /// Закрыть MF источник
    /// </summary>
    /// <returns></returns>
    HRESULT ShutdownSource();

    /// <summary>
    /// Создать топологию
    /// </summary>
    /// <returns></returns>
    HRESULT CreateTopology();

    /// <summary>
    /// Добавить ветвь топологии для одного потока
    /// </summary>
    /// <param name="presDescriptor"></param>
    /// <param name="index"></param>
    /// <returns></returns>
    HRESULT AddBranchToPartialTopology(
        IMFPresentationDescriptor* presDescriptor,
        DWORD index);

    /// <summary>
    /// Создать узел источника данных
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
    /// Создать узел приемника данных
    /// </summary>
    /// <param name="streamDescriptor"></param>
    /// <param name="node"></param>
    /// <returns></returns>
    HRESULT CreateOutputNode(
        IMFStreamDescriptor* streamDescriptor,
        CComPtr<IMFTopologyNode>& node);
};

