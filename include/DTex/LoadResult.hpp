#pragma once

#include <variant>
#include <cassert>
#include <string>

#include "Typedefs.hpp"

namespace DTex
{
	template<typename T>
	class LoadResult
	{
	public:
		LoadResult() = delete;
		LoadResult(const LoadResult&) = default;
		LoadResult(LoadResult&&) = default;
		constexpr explicit LoadResult(ResultInfo result, std::string&& errorMessage);
		constexpr explicit LoadResult(T&& data);

		LoadResult& operator=(const LoadResult&) = default;
		LoadResult& operator=(LoadResult&&) = default;

		constexpr ResultInfo GetResultInfo() const;
		const std::string& GetErrorMessage() const;

		constexpr T& GetValue();
		constexpr const T& GetValue() const;
	private:
		ResultInfo resultInfo;
		std::variant<T, std::string> variant;
	};

	template<typename T>
	inline constexpr LoadResult<T>::LoadResult(ResultInfo result, std::string&& errorMessage) :
		resultInfo(result),
		variant(std::move(errorMessage))
	{
	}

	template<typename T>
	inline constexpr LoadResult<T>::LoadResult(T&& in) :
		resultInfo(ResultInfo::Success),
		variant(std::move(in))
	{
	}

	template<typename T>
	inline constexpr ResultInfo LoadResult<T>::GetResultInfo() const
	{
		return resultInfo;
	}

	template<typename T>
	inline const std::string& LoadResult<T>::GetErrorMessage() const
	{
		return std::get<std::string&>(variant);
	}

	template<typename T>
	inline constexpr T& LoadResult<T>::GetValue()
	{
		return std::get<T>(variant);
	}

	template<typename T>
	inline constexpr const T& LoadResult<T>::GetValue() const
	{
		return std::get<T>(variant);
	}
}