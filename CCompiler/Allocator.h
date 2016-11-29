#pragma once

#include "PseudoTable.h"

#include <map>

namespace Yradex
{
	namespace CCompiler
	{
		class MemoryPool
		{
		public:
			enum Type
			{
				leaf,
				default_,
			};

		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		private:
			std::map<size_t, std::shared_ptr<Variable>> _register_pool;
			size_t _max_used = 15;
			int _sp = -8;

		public:
			MemoryPool(Type type);

			VariableAddress push_to_stack(size_t size = 1);
			int get_sp();

			VariableAddress push(const std::shared_ptr<Variable> &v);
			VariableAddress push_beyond_max(const std::shared_ptr<Variable> &v);
			void pop_register(const std::shared_ptr<Variable> &v);
			bool is_in_register(const std::shared_ptr<Variable> &v);
			size_t get_max_used_register();

		};


		class Allocator
		{
		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		public:
			void allocate(PseudoTable &table);
		};

	}
}

