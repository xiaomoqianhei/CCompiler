#include "PseudoTable.h"

namespace Yradex
{
	namespace CCompiler
	{
		void PseudoTable::switch_function(const FunctionIdentifier &f)
		{
			if (f == FunctionIdentifier::global)
			{
				_debug("global: ");
			}
			else
			{
				_debug("function: " + f.get_name());
			}
			_current_function = f;
		}

		Error PseudoTable::insert_instruction(const PseudoInstruction & i)
		{
			if (i.operator_ == PseudoOperator::call)
			{
				_symbol_table.set_leaf_function(_current_function, false);
			}

			if (i.operator_ == PseudoOperator::assign)
			{
				if (i.result->is_const())
				{
					return Error::assign_to_const;
				}
			}
			else if (i.operator_ == PseudoOperator::arg)
			{
				if (i.result->get_variable_type() == VariableType::array)
				{
					return Error::array_as_argument;
				}
			}

			_instruction_map[_current_function].push_back(i);
			_debug(i);
			return Error::null;
		}


	}
}