#pragma once

#include"Utility.h"

#include <iostream>
#include <unordered_map>

namespace Yradex
{
	namespace CCompiler
	{
		enum class Error
		{
			null,
			invalid_source_file,
			invalid_identifier,
			invalid_type_identifier,
			function_main_not_found,
			invalid_array_length,
			missing_character,
			invalid_character,
			reached_end_of_file,
			illegal_leading_zero,
			num_too_long,
			string_not_end,
			illegal_character,
			invalid_integer,
			while_identifier_required,
			condition_required,
			invalid_operator,
			invalid_argument,
			invalid_factor,
			identifier_already_defined,
			identifier_not_defined,
			function_not_return,
			illegal_return_type,
			identifier_is_not_array,
			assign_to_const,
			array_without_index,
			illegal_argument_list,
			use_return_value_of_void_function,
			devide_by_zero,
		};

		template <typename C>
		std::basic_ostream<C>& operator<<(std::basic_ostream<C> &s, Error e)
		{
			static std::unordered_map<Error, std::string, EnumHasher<Error>> map = {
				{ Error::null, "null" },
				{ Error::invalid_source_file, "invalid_source_file" },
				{ Error::invalid_identifier, "invalid_identifier" },
				{ Error::invalid_type_identifier, "invalid_type_identifier" },
				{ Error::function_main_not_found, "function_main_not_found" },
				{ Error::invalid_array_length, "invalid_array_length" },
				{ Error::missing_character, "missing_character" },
				{ Error::invalid_character, "invalid_character" },
				{ Error::reached_end_of_file, "reached_end_of_file" },
				{ Error::illegal_leading_zero, "illegal_leading_zero" },
				{ Error::num_too_long, "num_too_long" },
				{ Error::string_not_end, "string_not_end" },
				{ Error::illegal_character, "illegal_character" },
				{ Error::invalid_integer, "invalid_integer" },
				{ Error::while_identifier_required, "while_identifier_required" },
				{ Error::condition_required, "condition_required" },
				{ Error::invalid_operator, "invalid_operator" },
				{ Error::invalid_argument, "invalid_argument" },
				{ Error::invalid_factor, "invalid_factor" },
				{ Error::identifier_already_defined, "identifier_already_defined" },
				{ Error::identifier_not_defined, "identifier_not_defined" },
				{ Error::function_not_return, "function_not_return" },
				{ Error::illegal_return_type, "illegal_return_type" },
				{ Error::identifier_is_not_array, "identifier_is_not_array" },
				{ Error::assign_to_const, "assign_to_const" },
				{ Error::array_without_index, "array_without_index" },
				{ Error::illegal_argument_list, "illegal_argument_list" },
				{ Error::use_return_value_of_void_function, "use_return_value_of_void_function" },
				{ Error::devide_by_zero, "devide_by_zero" },
			};

			s << map.at(e);
			return s;
		}


	}
}



