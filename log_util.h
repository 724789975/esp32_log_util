#ifndef __LOG_UTIL_H__
#define __LOG_UTIL_H__

#include "Arduino.h"

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_NONE 8

namespace LOG_UTIL
{
	class LogUtil
	{
	public:
		static LogUtil &Instance()
		{
			static LogUtil instance;
			return instance;
		}
		void SetLogTarget(Stream &stream)
		{
			m_pTarget = &stream;
		};
		void SetLogLevel(uint8_t level)
		{
			m_btLogLevel = level;
		};
		~LogUtil() = default;
		LogUtil(const LogUtil &) = delete;
		LogUtil &operator=(const LogUtil &) = delete;

	public:
		int Log(uint8_t level, const char *format, ...)
		{
			if (level < m_btLogLevel)
			{
				return 0;
			}
			
			if (m_pTarget != nullptr)
			{
				char loc_buf[64];
				char *temp = loc_buf;
				va_list arg;
				va_list copy;
				va_start(arg, format);
				va_copy(copy, arg);
				int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
				va_end(copy);
				if (len < 0)
				{
					va_end(arg);
					return 0;
				};
				if (len >= sizeof(loc_buf))
				{
					temp = (char *)malloc(len + 1);
					if (temp == NULL)
					{
						va_end(arg);
						return 0;
					}
					len = vsnprintf(temp, len + 1, format, arg);
				}
				va_end(arg);
				len = m_pTarget->write((uint8_t *)temp, len);
				if (temp != loc_buf)
				{
					free(temp);
				}
				return len;
			}
			return -1;
		}
	private:
		Stream *m_pTarget{nullptr};
		uint8_t m_btLogLevel{LOG_LEVEL_DEBUG};
		LogUtil() = default;
	};


}; // namespace LOG_UTIL

#define log_set_level(level) LOG_UTIL::LogUtil::Instance().SetLogLevel(level);

#define log_set_target(stream) LOG_UTIL::LogUtil::Instance().SetLogTarget(stream);

#define log_debug(log_name, fmt, arg...)                                                           \
    do                                                                                                 \
    {                                                                                                  \
        LOG_UTIL::LogUtil::Instance().Log(LOG_LEVEL_DEBUG, "[%s][%d]@%d>D:" fmt "\n", log_name, (int)millis(), __LINE__, ##arg); \
    } while (0);

#define log_info(log_name, fmt, arg...)                                                           \
    do                                                                                                 \
    {                                                                                                  \
        LOG_UTIL::LogUtil::Instance().Log(LOG_LEVEL_INFO, "[%s][%d]@%d>D:" fmt "\n", log_name, (int)millis(), __LINE__, ##arg); \
    } while (0);

#define log_error(log_name, fmt, arg...)                                                           \
    do                                                                                                 \
	{                                                                                                  \
        LOG_UTIL::LogUtil::Instance().Log(LOG_LEVEL_ERROR, "[%s][%d]@%d>E:" fmt "\n", log_name, (int)millis(), __LINE__, ##arg); \
    } while (0);

#endif // __LOG_UTIL_H__