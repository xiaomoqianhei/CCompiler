#pragma once

#include "Utility.h"

#include <vector>
#include <memory>

namespace Yradex
{
	namespace CCompiler
	{
		class FunctionIdentifier
		{
		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;

		private:
			string_type _function_name;

		public:
			static const FunctionIdentifier global;

		public:
			FunctionIdentifier(const string_type &name)
				: _function_name(name)
			{
			}

			FunctionIdentifier& operator=(const FunctionIdentifier &f)
			{
				_function_name = f._function_name;
				return *this;
			}

			bool operator==(const FunctionIdentifier &f) const
			{
				return _function_name == f._function_name;
			}

			bool operator!=(const FunctionIdentifier &f) const
			{
				return _function_name != f._function_name;
			}

			const string_type get_name() const
			{
				return _function_name;
			}
		};

		class FunctionHasher
		{
		public:
			size_t operator()(const FunctionIdentifier &f) const
			{
				return std::hash<FunctionIdentifier::string_type>()(f.get_name());
			}
		};



	}
}
