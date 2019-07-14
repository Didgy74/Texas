#pragma once

#include "DTex/ResultInfo.hpp"

#include <cstdint>
#include <variant>
#include <string_view>
#include <cstring>

namespace DTex
{
	template<typename T>
	class LoadInfo
	{
	public:
		LoadInfo() = delete;
		LoadInfo(const LoadInfo&);
		LoadInfo(LoadInfo&&);
		constexpr explicit LoadInfo(ResultInfo result, std::string_view&& errorMessage);
		explicit LoadInfo(T&& data);

		~LoadInfo();

		LoadInfo& operator=(const LoadInfo&);
		LoadInfo& operator=(LoadInfo&&);

		ResultInfo GetResultInfo() const;
		const std::string_view& GetErrorMessage() const;

		/*
			Returns the loaded struct.

			Warning! Using this method when IsSuccessful() returns false, will result in undefined behavior.
		*/
		T& GetValue()
		{
			return value;
		}

		/*
			Returns the loaded struct.

			Warning! Using this method when IsSuccessful() returns false, will result in undefined behavior.
		*/
		const T& GetValue() const
		{
			return value;
		}

		/*
			Returns true if GetResultInfo() returns ResultInfo::Success.
		*/
		bool IsSuccessful() const
		{
			return resultInfo == ResultInfo::Success;
		}
		/*
			Does the same as IsSuccessful().
		*/
		operator bool() const
		{
			return resultInfo == ResultInfo::Success;
		}

	private:
		ResultInfo resultInfo;
		union
		{
			std::string_view errorMessage;
			T value;
		};
	};

	template<typename T>
	inline LoadInfo<T>::LoadInfo(const LoadInfo& rhs) :
		resultInfo(rhs.resultInfo)
	{
		if (resultInfo == ResultInfo::Success)
			value = rhs.value;
		else
			errorMessage = rhs.errorMessage;
	}

	template<typename T>
	inline LoadInfo<T>::LoadInfo(LoadInfo&& rhs) :
		resultInfo(rhs.resultInfo)
	{
		if (resultInfo == ResultInfo::Success)
			value = std::move(rhs.value);
		else
			errorMessage = std::move(rhs.errorMessage);
	}

	template<typename T>
	inline constexpr LoadInfo<T>::LoadInfo(ResultInfo result, std::string_view&& errorMessage) :
		resultInfo(result),
		errorMessage(errorMessage)
	{
	}

	template<typename T>
	inline LoadInfo<T>::LoadInfo(T&& in) :
		resultInfo(ResultInfo::Success),
		value(std::move(in))
	{
	}

	template<typename T>
	LoadInfo<T>::~LoadInfo()
	{
		if (resultInfo == ResultInfo::Success)
			value.~T();
	}

	template<typename T>
	LoadInfo<T>& LoadInfo<T>::operator=(const LoadInfo& rhs)
	{
		resultInfo = rhs.resultInfo;
		if (resultInfo == ResultInfo::Success)
			value = rhs.value;
		else
			errorMessage = rhs.errorMessage;
	}

	template<typename T>
	LoadInfo<T>& LoadInfo<T>::operator=(LoadInfo&& rhs)
	{
		resultInfo = rhs.resultInfo;
		if (resultInfo == ResultInfo::Success)
			value = std::move(rhs.value);
		else
			errorMessage = std::move(rhs.errorMessage);
	}

	template<typename T>
	inline ResultInfo LoadInfo<T>::GetResultInfo() const
	{
		return resultInfo;
	}

	template<typename T>
	inline const std::string_view& LoadInfo<T>::GetErrorMessage() const
	{
		return errorMessage;
	}
}