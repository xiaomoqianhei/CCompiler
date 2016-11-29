#pragma once

#include <cassert>
#include <string>

namespace Yradex
{
	namespace CCompiler
	{
		namespace Config
		{
			constexpr static unsigned max_num_length = 10;

			// DEBUG, WARNING, ERROR, FATAL
			const static std::string error_level = "DEBUG";
			const static bool lexical_analyzer_debug = false;
			const static bool parser_debug = false;
			const static bool pseudo_table_debug = false;
			const static bool generator_debug = false;
		};

		template <typename T>
		struct EnumHasher
		{
			size_t operator()(T t) const
			{
				return static_cast<typename std::underlying_type<T>::type>(t);
			}
		};

		

	}
}
