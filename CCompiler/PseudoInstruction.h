#pragma once

#include "Utility.h"
#include "Variable.h"
#include "Error.h"
#include "SymbolTable.h"

#include <iomanip>

namespace Yradex
{
	namespace CCompiler
	{

		class PseudoTable;

		enum class PseudoOperator
		{
			add,
			sub,
			mul,
			div,
			b,
			beq,
			bne,
			bltz,
			blez,
			bgtz,
			bgez,
			read,
			print,
			call,
			ret,
			label,
			assign,
			load,
			store,
			arg,
			nop,
		};

		namespace PseudoOperatorUtility
		{
			inline bool is_end_of_basic_block(PseudoOperator op)
			{
				switch (op)
				{
				case PseudoOperator::b:
				case PseudoOperator::beq:
				case PseudoOperator::bne:
				case PseudoOperator::bltz:
				case PseudoOperator::blez:
				case PseudoOperator::bgtz:
				case PseudoOperator::bgez:
				case PseudoOperator::call:
				case PseudoOperator::ret:
				case PseudoOperator::label:
					return true;
				default:
					break;
				}
				return false;
			}
		}

		class PseudoInstruction
		{
			template <typename C> 
			friend std::basic_ostream<C>& operator<<(std::basic_ostream<C> &s, const PseudoInstruction &pi);
			
		private:
			PseudoOperator _operator_;
			std::shared_ptr<Variable> _left_argument;
			std::shared_ptr<Variable> _right_argument;
			std::shared_ptr<Variable> _result;

		public:
			PseudoInstruction(PseudoOperator op, const std::shared_ptr<Variable> &a,
				const std::shared_ptr<Variable> &b, const std::shared_ptr<Variable> &res);
			PseudoInstruction(const PseudoInstruction &ins);
			PseudoInstruction& operator=(PseudoInstruction ins)
			{
				swap(ins);
			}
			~PseudoInstruction();

			PseudoOperator get_operator() const
			{
				return _operator_;
			}
			std::shared_ptr<Variable> get_left_argument() const
			{
				return _left_argument;
			}
			std::shared_ptr<Variable> get_right_argument() const
			{
				return _right_argument;
			}
			std::shared_ptr<Variable> get_result() const
			{
				return _result;
			}

			void swap(PseudoInstruction &ins);
			void clear()
			{
				PseudoInstruction nop = PseudoInstruction(PseudoOperator::nop, Variable::null, Variable::null, Variable::null);
				swap(nop);
			}

			Error check_validation() const;
			PseudoInstruction to_literal(PseudoTable &pseudo_table);

		};

		template <typename C>
		std::basic_ostream<C>& operator<<(std::basic_ostream<C> &s, PseudoOperator op)
		{
			static std::unordered_map<PseudoOperator, std::string, EnumHasher<PseudoOperator>> map = {
				{ PseudoOperator::add, "add" },
				{ PseudoOperator::sub, "sub" },
				{ PseudoOperator::mul, "mul" },
				{ PseudoOperator::div, "div" },
				{ PseudoOperator::b, "b" },
				{ PseudoOperator::beq, "beq" },
				{ PseudoOperator::bne, "bne" },
				{ PseudoOperator::bltz, "bltz" },
				{ PseudoOperator::blez, "blez" },
				{ PseudoOperator::bgtz, "bgtz" },
				{ PseudoOperator::bgez, "bgez" },
				{ PseudoOperator::read, "read" },
				{ PseudoOperator::print, "print" },
				{ PseudoOperator::call, "call" },
				{ PseudoOperator::ret, "ret" },
				{ PseudoOperator::label, "label" },
				{ PseudoOperator::assign, "assign" },
				{ PseudoOperator::load, "load" },
				{ PseudoOperator::store, "store" },
				{ PseudoOperator::arg, "arg" },
				{ PseudoOperator::nop, "nop" },
			};

			return s << map[op];
		}

		template <typename C>
		std::basic_ostream<C>& operator<<(std::basic_ostream<C> &s, const PseudoInstruction &pi)
		{
			return s << std::setw(10) << std::left << pi._operator_
				<< std::setw(40) << std::left << *pi._left_argument
				<< std::setw(40) << std::left << *pi._right_argument
				<< std::setw(40) << std::left << *pi._result;
		}

	}
}
