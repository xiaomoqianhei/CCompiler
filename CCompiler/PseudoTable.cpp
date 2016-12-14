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

		Error PseudoTable::insert_instruction(const PseudoInstruction & ins)
		{
			// leaf
			if (ins.get_operator() == PseudoOperator::call)
			{
				_symbol_table.set_leaf_function(_current_function, false);
			}

			Error error = ins.check_validation();
			if (error != Error::null)
			{
				return error;
			}

			PseudoInstruction new_ins(ins);
			new_ins.to_literal(*this);

			_instruction_map[_current_function].push_back(new_ins);
			_debug(ins);
			return Error::null;
		}

	}
}
