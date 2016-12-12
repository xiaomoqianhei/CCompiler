#pragma once

#include "Utility.h"
#include "Variable.h"

#include <iomanip>

namespace Yradex
{
	namespace CCompiler
	{

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

		struct PseudoInstruction
		{
			PseudoOperator operator_;
			std::shared_ptr<Variable> argument_1;
			std::shared_ptr<Variable> argument_2;
			std::shared_ptr<Variable> result;

		public:
			PseudoInstruction(PseudoOperator op, const std::shared_ptr<Variable> &a, 
				const std::shared_ptr<Variable> &b, const std::shared_ptr<Variable> &res)
				:operator_(op), argument_1(a), argument_2(b), result(res)
			{
				argument_1->increase_ref();
				argument_2->increase_ref();
			}

			~PseudoInstruction()
			{
				argument_1->decrease_ref();
				argument_2->decrease_ref();
			}

			void clear()
			{
				operator_ = PseudoOperator::nop;
				argument_1 = Variable::null;
				argument_2 = Variable::null;
				result = Variable::null;
			}
				
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
			return s << std::setw(10) << std::left << pi.operator_
				<< std::setw(40) << std::left << *pi.argument_1
				<< std::setw(40) << std::left << *pi.argument_2
				<< std::setw(40) << std::left << *pi.result;
		}

	}
}
