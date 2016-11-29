#pragma once

#include "Utility.h"
#include "Error.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <algorithm>

namespace Yradex
{
	namespace CCompiler
	{
		class BasicErrorHandler
		{
		public:
			enum class Level
			{
				debug = 0,
				warning = 1,
				error = 2,
				fatal = 3,
			};

		public:
			using string_type = std::string;

		private:
			static const std::unordered_map <Level, string_type, EnumHasher<Level>> _level_map;

		private:
			Level _level;
			size_t _error_count;

		private:
			BasicErrorHandler();

			BasicErrorHandler(const BasicErrorHandler&) = delete;
			BasicErrorHandler& operator=(const BasicErrorHandler&) = delete;

			Level _current_level() const
			{
				return _level;
			}

		public:
			static BasicErrorHandler& instance()
			{
				static BasicErrorHandler h;
				return h;
			}


			bool should_raise(Level level) const
			{
				return level >= _current_level();
			}

			size_t get_error_count() const
			{
				return _error_count;
			}

			void raise(Error error, Level level);
			
			void raise(Error error, std::string filename, size_t line_num, size_t char_num, Level level);

			void raise(Error error, const string_type &info,
				std::string filename, size_t line_num, size_t char_num, Level level);

			void debug(const string_type &info);

		};

	}
}

