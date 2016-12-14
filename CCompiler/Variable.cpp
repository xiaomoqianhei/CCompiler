#include "Variable.h"

#include <sstream>

namespace Yradex
{
	namespace CCompiler
	{


		std::shared_ptr<Variable> Variable::get_temp_variable(Symbol type, const FunctionIdentifier & function)
		{
			static long id = 0;

			std::basic_ostringstream<char_type> stream;
			stream << "$Tmp" << id++;
			string_type name = stream.str();

			return Variable::new_variale(name, type, function, true);
		}

		std::shared_ptr<Variable> Variable::get_string_variable()
		{
			static long id = 0;

			std::basic_ostringstream<char_type> stream;
			stream << "$Str" << id++;
			string_type name = stream.str();

			return Variable::new_string_variable(name, FunctionIdentifier::global);
		}

		std::shared_ptr<Variable> Variable::get_label()
		{
			static long id = 0;

			std::basic_ostringstream<char_type> stream;
			stream << "$Lbl" << id++;
			string_type name = stream.str();

			return Variable::new_label(name, FunctionIdentifier::global);
		}

		std::shared_ptr<Variable> Variable::get_label(const string_type & name)
		{
			return Variable::new_label(name, FunctionIdentifier::global);
		}

	}
}
