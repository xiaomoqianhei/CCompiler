#include "PseudoInstruction.h"

#include "PseudoTable.h"

namespace Yradex
{
	namespace CCompiler
	{

		PseudoInstruction::PseudoInstruction(PseudoOperator op, const std::shared_ptr<Variable>& a, const std::shared_ptr<Variable>& b, const std::shared_ptr<Variable>& res)
			:_operator_(op), _left_argument(a), _right_argument(b), _result(res)
		{
			_left_argument->increase_ref();
			_right_argument->increase_ref();
			_result->increase_ref();
		}
		PseudoInstruction::PseudoInstruction(const PseudoInstruction & ins)
			:_operator_(ins._operator_), _left_argument(ins._left_argument),
			_right_argument(ins._right_argument), _result(ins._result)
		{
			_left_argument->increase_ref();
			_right_argument->increase_ref();
			_result->increase_ref();
		}
		PseudoInstruction::~PseudoInstruction()
		{
			_left_argument->decrease_ref();
			_right_argument->decrease_ref();
			_result->decrease_ref();
		}
		void PseudoInstruction::swap(PseudoInstruction & ins)
		{
			std::swap(_operator_, ins._operator_);
			std::swap(_left_argument, ins._left_argument);
			std::swap(_right_argument, ins._right_argument);
			std::swap(_result, ins._result);
		}
		Error PseudoInstruction::check_validation() const
		{
			// array
			if (_left_argument->get_variable_type() == Variable::Type::array && _operator_ != PseudoOperator::load)
			{
				return Error::array_without_index;
			}
			if (_right_argument->get_variable_type() == Variable::Type::array)
			{
				return Error::array_without_index;
			}
			if (_result->get_variable_type() == Variable::Type::array && _operator_ != PseudoOperator::store)
			{
				return Error::array_without_index;
			}

			// const
			if (_operator_ == PseudoOperator::assign)
			{
				if (_result->is_const())
				{
					return Error::assign_to_const;
				}
			}

			return Error::null;
		}
		PseudoInstruction PseudoInstruction::to_literal(PseudoTable &pseudo_table)
		{
			PseudoInstruction new_ins(*this);

			if (_left_argument != Variable::null() && _left_argument->get_variable_type() == Variable::Type::const_variable
				&& _right_argument->get_type() == Symbol::int_symbol)
			{
				_left_argument = pseudo_table.get_literal(_left_argument->get_value());
			}

			if (_right_argument != Variable::null() && _right_argument->get_variable_type() == Variable::Type::const_variable
				&& _right_argument->get_type() == Symbol::int_symbol)
			{
				_right_argument = pseudo_table.get_literal(_right_argument->get_value());
			}

			return new_ins;
		}
		void PseudoInstruction::swap_arguments()
		{
			_left_argument.swap(_right_argument);
		}
	}
}

