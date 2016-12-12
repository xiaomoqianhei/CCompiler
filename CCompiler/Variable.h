#pragma once

#include "Utility.h"
#include "Symbol.h"
#include "FunctionIdentifier.h"

#include <memory>
#include <sstream>

namespace Yradex
{
	namespace CCompiler
	{
		struct VariableAddress
		{
			bool in_register;
			int position;

			VariableAddress() {}

			VariableAddress(bool in_register_, int position_)
				:in_register(in_register_), position(position_)
			{

			}

			std::string to_string() const
			{
				static std::unordered_map<int, std::string> register_map = {
					{ 0, "$zero" },
					{ 1, "$at" },
					{ 2, "$v0" },
					{ 3, "$v1" },
					{ 4, "$a0" },
					{ 5, "$a1" },
					{ 6, "$a2" },
					{ 7, "$a3" },
					{ 8, "$t0" },
					{ 9, "$t1" },
					{ 10, "$t2" },
					{ 11, "$t3" },
					{ 12, "$t4" },
					{ 13, "$t5" },
					{ 14, "$t6" },
					{ 15, "$t7" },
					{ 16, "$s0" },
					{ 17, "$s1" },
					{ 18, "$s2" },
					{ 19, "$s3" },
					{ 20, "$s4" },
					{ 21, "$s5" },
					{ 22, "$s6" },
					{ 23, "$s7" },
					{ 24, "$t8" },
					{ 25, "$t9" },
					{ 26, "$k0" },
					{ 27, "$k1" },
					{ 28, "$gp" },
					{ 29, "$sp" },
					{ 30, "$sp" },
					{ 31, "$ra" },
				};

				assert(in_register);
				return register_map.at(position);
			}
		};

		enum class VariableType
		{
			variable,
			const_variable,
			array,
			label,
		};

		class Variable
		{
		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;

		private:
			const string_type _name;
			const Symbol _return_type;
			const size_t _array_length;
			const VariableType _variable_type;
			int _value;
			const bool _is_temp;
			VariableAddress _addr;
			FunctionIdentifier _function;
			int _ref = 0;

		public:
			static const std::shared_ptr<Variable> null;

		public:
			Variable(const string_type &name, Symbol return_type, VariableType variable_type,
				size_t length, int value, const FunctionIdentifier &function, bool is_temp)
				: _name(name), _return_type(return_type), _array_length(length),
				_variable_type(variable_type), _value(value), _is_temp(is_temp), _function(function)
			{
			}

			static std::shared_ptr<Variable>
				new_const_variable(const string_type &name, Symbol type,
					int value, const FunctionIdentifier &function, bool is_temp)
			{
				return std::make_shared<Variable>(name, type, VariableType::const_variable, 0, value, function, is_temp);
			}
			static std::shared_ptr<Variable>
				new_variale(const string_type &name, Symbol type,
					const FunctionIdentifier &function, bool is_temp)
			{
				return std::make_shared<Variable>(name, type, VariableType::variable, 0, 0, function, is_temp);
			}
			static std::shared_ptr<Variable>
				new_array_variable(const string_type &name, Symbol type,
					size_t length, const FunctionIdentifier &function)
			{
				return std::make_shared<Variable>(name, type, VariableType::array, length, 0, function, false);
			}
			static std::shared_ptr<Variable>
				new_label(const string_type &name, const FunctionIdentifier &function)
			{
				return std::make_shared<Variable>(name, Symbol::eof, VariableType::label, 0, 0, function, false);
			}

			static std::shared_ptr<Variable> get_temp_variable(Symbol type, const FunctionIdentifier &function);
			static std::shared_ptr<Variable> get_string_variable();
			static std::shared_ptr<Variable> get_label();
			static std::shared_ptr<Variable> get_label(const string_type &name);

			~Variable() {}

			bool operator==(const Variable &v) const
			{
				return _name == v._name && _function == v._function;
			}

			explicit operator bool() const
			{
				return _return_type != Symbol::eof;
			}

			const string_type& get_name() const
			{
				return _name;
			}
			const Symbol get_type() const
			{
				return _return_type;
			}

			const FunctionIdentifier& get_function() const
			{
				return _function;
			}
			void set_function(const FunctionIdentifier &f)
			{
				_function = f;
			}

			size_t get_length() const
			{
				return _array_length;
			}

			void set_address(const VariableAddress &va)
			{
				_addr = va;
			}

			bool is_const() const
			{
				return _variable_type == VariableType::const_variable;
			}
			bool is_temp() const
			{
				return _is_temp;
			}

			VariableType get_variable_type()
			{
				return _variable_type;
			}

			int get_value() const
			{
				return _value;
			}

			bool in_register() const
			{
				return _addr.in_register;
			}

			int position() const
			{
				return _addr.position;
			}

			void increase_ref()
			{
				_ref++;
			}

			void decrease_ref()
			{
				_ref--;
			}

			int get_ref() const
			{
				return _ref;
			}

			std::string address_as_string()
			{
				static std::unordered_map<int, std::string> register_map = {
					{ 0, "$zero" },
					{ 1, "$at" },
					{ 2, "$v0" },
					{ 3, "$v1" },
					{ 4, "$a0" },
					{ 5, "$a1" },
					{ 6, "$a2" },
					{ 7, "$a3" },
					{ 8, "$t0" },
					{ 9, "$t1" },
					{ 10, "$t2" },
					{ 11, "$t3" },
					{ 12, "$t4" },
					{ 13, "$t5" },
					{ 14, "$t6" },
					{ 15, "$t7" },
					{ 16, "$s0" },
					{ 17, "$s1" },
					{ 18, "$s2" },
					{ 19, "$s3" },
					{ 20, "$s4" },
					{ 21, "$s5" },
					{ 22, "$s6" },
					{ 23, "$s7" },
					{ 24, "$t8" },
					{ 25, "$t9" },
					{ 26, "$k0" },
					{ 27, "$k1" },
					{ 28, "$gp" },
					{ 29, "$sp" },
					{ 30, "$sp" },
					{ 31, "$ra" },
				};

				if (in_register())
				{
					return register_map.at(position());
				}
				else
				{
					if (get_function() == FunctionIdentifier::global)
					{
						std::ostringstream stream;
						stream << position();
						return stream.str();
					}
					std::ostringstream stream;
					stream << position() << "($fp)";
					return stream.str();
				}
			}

		};

		class VariableHasher
		{
		public:
			size_t operator()(const std::shared_ptr<Variable> &v) const
			{
				return std::hash<Variable::string_type>()(v->get_name()) * 31 + FunctionHasher()(v->get_function());
			}
		};

		template <typename C>
		std::basic_ostream<C>& operator<<(std::basic_ostream<C>& s, const Variable &v)
		{
			return s << v.get_name();
		}

		template <typename C>
		std::basic_ostream<C>& operator<<(std::basic_ostream<C>& s, const std::shared_ptr<Variable> &v)
		{
			return s << v->get_name();
		}

		template <typename C>
		std::basic_ostream<C>& operator<<(std::basic_ostream<C>& s, const VariableAddress &va)
		{
			return s << va.to_string();
		}
	}
}

