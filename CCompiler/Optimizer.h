#pragma once

#include "Utility.h"
#include "PseudoTable.h"
#include "Allocator.h"

namespace Yradex
{
	namespace CCompiler
	{

		class Optimizer
		{
		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		private:
			PseudoTable &_pseudo_table;

		private:
			void _optimize(const FunctionIdentifier &f);

			void _calculate_constant_instruction();


		public:
			Optimizer(PseudoTable &pseudo_table)
				:_pseudo_table(pseudo_table)
			{
			}

			void optimize();

		};
	}
}
