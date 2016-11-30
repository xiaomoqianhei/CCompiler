#pragma once

#include "Utility.h"

#include <iostream>
#include <unordered_map>

namespace Yradex
{
	namespace CCompiler
	{
		enum class MipsOperator
		{
			add,
			b,
			beq,
			bne,
			bltz,
			blez,
			bgtz,
			bgez,
			div,
			jal,
			jr,
			la,
			lw,
			mul,
			nop,
			sub,
			sw,
			syscall,
		};

		template <typename C>
		std::basic_ostream<C>& operator<<(std::basic_ostream<C> &s, MipsOperator op)
		{
			static std::unordered_map<MipsOperator, std::string, EnumHasher<MipsOperator>> map = {
				{ MipsOperator::add, "add" },
				{ MipsOperator::b, "b" },
				{ MipsOperator::beq, "beq" },
				{ MipsOperator::bne, "bne" },
				{ MipsOperator::bltz, "bltz" },
				{ MipsOperator::blez, "blez" },
				{ MipsOperator::bgez, "bgez" },
				{ MipsOperator::bgtz, "bgtz" },
				{ MipsOperator::div, "div" },
				{ MipsOperator::jal, "jal" },
				{ MipsOperator::jr, "jr" },
				{ MipsOperator::la, "la" },
				{ MipsOperator::lw, "lw" },
				{ MipsOperator::mul, "mul" },
				{ MipsOperator::nop, "nop" },
				{ MipsOperator::sub, "sub" },
				{ MipsOperator::sw, "sw" },
				{ MipsOperator::syscall, "syscall" },
			};

			s << map.at(op);
			return s;
		}
	}
}