#pragma once

#include "PseudoTable.h"
#include "MipsOperator.h"

namespace Yradex
{
	namespace CCompiler
	{

		class Generator
		{
		private:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		private:
			PseudoTable &_table;
			std::basic_ostream<char_type> &_stream;

			size_t _arg_count = 0;
			int _lower_address_of_saved_s_register_to_fp;
			string_type _separator = "\t\t";

		private:
			void _generate_data();
			void _generate_text();
			void _generate_function();
			void _generate_instruction(PseudoInstruction &ins);
			void _rearrange_instruction(PseudoInstruction &ins);

			void _try_save_argument_register();

			string_type _get_argument_1(PseudoInstruction &ins);
			string_type _get_argument_2(PseudoInstruction &ins);

			//template<typename T>
			void _print_instruction(MipsOperator op)
			{
				_stream << _separator << op << std::endl;
			}

			template<typename T>
			void _print_instruction(MipsOperator op, const T &arg1)
			{
				_stream << _separator << op << _separator << arg1 << std::endl;
			}

			template<typename T, typename U>
			void _print_instruction(MipsOperator op, const T &arg1, const U &arg2)
			{
				_stream << _separator << op
					<< _separator << arg1 << ","
					<< _separator << arg2
					<< std::endl;
			}

			template<typename T, typename U, typename V>
			void _print_instruction(MipsOperator op, const T &arg1, const U &arg2, const V &arg3)
			{
				_stream << _separator << op
					<< _separator << arg1 << ","
					<< _separator << arg2 << ","
					<< _separator << arg3
					<< std::endl;
			}

		public:
			Generator(PseudoTable &table, std::basic_ostream<char_type> &stream)
				:_table(table), _stream(stream)
			{
			}

			void generate();


		};


	}
}