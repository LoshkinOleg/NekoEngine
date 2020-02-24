#pragma once

/*
 MIT License

 Copyright (c) 2017 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <atomic>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <mutex>
#include <thread>
#include <functional>
#include <utilities/service_locator.h>
#include <condition_variable>

namespace neko
{
//-----------------------------------------------------------------------------
<<<<<<< HEAD
// LogType
//-----------------------------------------------------------------------------
/// \brief To differentiate log messages
enum class LogType : std::uint8_t
{
	DEBUG = 1, //For regular debug messages
	WARNING, //For non-critical errors
	CRITICAL, //For critical errors
	LENGTH
};

//-----------------------------------------------------------------------------
// LogCategory
//-----------------------------------------------------------------------------
/// \brief To sort log messages into different categories
enum class LogCategory : std::uint8_t
{
	NONE = 1,
	ENGINE,
	MATH,
	GRAPHICS,
	IO,
	SOUND,
	LENGTH
=======
// LogTypes
//-----------------------------------------------------------------------------
/// \brief To differentiate log messages
enum class LogTypes : char
{
    DEBUG, //For regular debug messages
    WARNING, //For non-critical errors
    ERROR //For critical errors
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.
};

//-----------------------------------------------------------------------------
// LogMessage
//-----------------------------------------------------------------------------
/// \brief Struct representing a log message with its type
struct LogMessage
{
<<<<<<< HEAD
	LogType type = LogType::DEBUG;
	LogCategory category = LogCategory::NONE;
	std::string log;

	explicit LogMessage(std::string log)
		: log(std::move(log))
	{
		Generate();
	}

	explicit LogMessage(const LogType& logType, std::string log)
		: type(logType), log(std::move(log))
	{
		Generate();
	}

	explicit LogMessage(const LogCategory& category, const LogType& logType,
	                    std::string log)
		: type(logType), category(category), log(std::move(log))
	{
		Generate();
	}

	void Generate();
	void Display() const
	{
		(type != LogType::CRITICAL ? std::cout : std::cerr) << log;
	}
=======
    LogTypes type = LogTypes::DEBUG;
    std::string msg;

    explicit LogMessage(const std::string& log)
            : msg(log)
    {}

    explicit LogMessage(const LogTypes& logType, const std::string& log)
            : type(logType), msg(log)
    {}
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.
};

//-----------------------------------------------------------------------------
// LogManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class LogManagerInterface
{
<<<<<<< HEAD
protected:
	~LogManagerInterface() = default;
public:
	/**
	 * \brief Generate a log message.
	 * @param logType the type of the log message
	 * @param log the log message
	 */
	virtual void Log(LogType logType, const std::string& log) = 0;

	/**
	 * \brief Generate a log message.
	 * @param logType the type of the log message
	 * @param category the category of the log message
	 * @param log the log message
	 */
	virtual void Log(LogCategory category, LogType logType,
	                 const std::string& log) = 0;

	/**
	 * \brief Retrieves the log history
	 */
	virtual const std::vector<LogMessage>& GetLogs() = 0;
=======
public:
    /**
     * \brief Generate a log message.
     * @param logType the type of the log message
     * @param log the log message
     */
    virtual void Log(LogTypes logType, const std::string& log) = 0;

    /**
     * \brief Retrieves the log history
     */
    virtual const std::vector<std::string>& GetLogs() = 0;
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.
};

//-----------------------------------------------------------------------------
// NullLogManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullLogManager final : public LogManagerInterface
{
<<<<<<< HEAD
	void Log([[maybe_unused]] LogType logType,
	         [[maybe_unused]] const std::string& log) override
	{
		Log(LogCategory::NONE, logType, log);
	}

	void Log([[maybe_unused]] LogCategory category,
	         [[maybe_unused]] LogType logType,
	         [[maybe_unused]] const std::string& log) override
	{
		const LogMessage logMessage(category, logType, log);
		logMessage.Display();
		std::cout << "[WARNING] LogManager is null! History will NOT be saved\n";
	}

	const std::vector<LogMessage>& GetLogs() override
	{
		std::cerr << "Impossible to get log history from a null LogManager\n";
		assert(false);
#ifdef _MSC_VER
		return {};
#endif
	}
=======
public:
    void Log([[maybe_unused]]LogTypes logType, [[maybe_unused]]const std::string& log) override
    {};

    const std::vector<std::string>& GetLogs() override
    {
        std::cerr << "Impossible to get log history from a null LogManager\n";
        assert(false);
    };
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.
};

//-----------------------------------------------------------------------------
// LogManager
//-----------------------------------------------------------------------------
/// \brief Creates and stores log messages
<<<<<<< HEAD
class LogManager final : public LogManagerInterface
{
protected:
	//-----------------------------------------------------------------------------
	// LogManagerStatus
	//-----------------------------------------------------------------------------
	/// \brief To get the status of the engine
	enum LogManagerStatus : std::uint8_t
	{
		IS_RUNNING = 1u << 0u, //To check if the LogManager is running
		IS_EMPTY = 1u << 1u, //To check if the LogManager has tasks
		IS_LOG_WAITING = 1u << 2u, //To check if the LogManager is waiting for a task
		IS_WRITING = 1u << 3u //To check if the LogManager is writing its output to a file
	};
public:
	LogManager();
	~LogManager();

	void LogLoop();
	void Wait();
	void Destroy();

	void Log(LogType logType, const std::string& log) override;

	void Log(LogCategory category, LogType logType,
		const std::string& log) override;

	const std::vector<LogMessage>& GetLogs() override
	{
		return logHistory_;
	}

	void WriteToFile();
private:
	std::atomic<std::uint8_t> status_;

	std::vector<LogMessage> logHistory_;

	std::unique_ptr<std::thread> logThread_;
	std::mutex logMutex_;
	std::vector<std::function<void()>> tasks_;
	std::condition_variable conditionVariable_;
=======
class LogManager : public LogManagerInterface
{
public:
    ~LogManager() = default;

    void Log(LogTypes logType, const std::string& log) override;

    const std::vector<std::string>& GetLogs() override
    { return logHistory_; }

    void WriteToFile();

    void Close();

private:
    std::vector<std::string> logHistory_;

    std::mutex logMutex_;
    std::queue<std::function<void()>> tasks_;
    std::unique_ptr<std::thread> logThread_;
    std::condition_variable itemInQueue_;
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.
};

//-----------------------------------------------------------------------------
// Service Locator definition
//-----------------------------------------------------------------------------
using Log = Locator<LogManagerInterface, NullLogManager>;

//-----------------------------------------------------------------------------
// Shorthands
//-----------------------------------------------------------------------------
/**
 * \brief Generate a debug type log message
 */
void LogDebug(const std::string& msg);

/**
<<<<<<< HEAD
 * \brief Generate a debug type log message
 */
void LogDebug(LogCategory category, const std::string& msg);

/**
=======
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.
 * \brief Generate a warning type log message
 */
void LogWarning(const std::string& msg);

/**
<<<<<<< HEAD
 * \brief Generate a warning type log message
 */
void LogWarning(LogCategory category, const std::string& msg);

/**
 * \brief Generate an error type log message
 */
void LogError(const std::string& msg);

/**
 * \brief Generate an error type log message
 */
void LogError(LogCategory category, const std::string& msg);
=======
 * \brief Generate an error type log message
 */
void LogError(const std::string& msg);
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.

/**
 * \brief Retrieves the log history
 */
<<<<<<< HEAD
const std::vector<LogMessage>& GetLogs();
}
=======
const std::vector<std::string>& GetLogs();
}
>>>>>>> Created a simple LogManager able to handle multiple type of messages and write the log results into a file.
