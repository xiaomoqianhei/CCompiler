#include "Variable.h"

#include <sstream>

namespace Yradex
{
	namespace CCompiler
	{
		const std::shared_ptr<Variable> Variable::null = 
			std::make_shared<Variable>("", Symbol::eof, Variable::Type::const_variable, 0, 0, string_type(), false);

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

			return Variable::new_const_variable(name, Symbol::string, 0, FunctionIdentifier::global, false);
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
