#include <core/logger.hpp>

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <core/utils.hpp>

#ifdef SHOW_CONSOLE
#define GTUI_IMPLEMENTATION
#include <platform/gtui.h>
#endif

static bool initialized = false;
static FILE* fileOutput;

static std::queue<Logger::Message> messageQueue;
static std::thread thread;
static std::condition_variable cv;
static bool exitFlag;

std::mutex mutex;

static void WriteMessages()
{
	while (true)
	{
		Logger::Message message;

		{
			std::unique_lock<std::mutex> lock(mutex);

			cv.wait(lock, [] { return !messageQueue.empty() || exitFlag; });

			if (messageQueue.empty() && exitFlag)
				break;

			message = messageQueue.front();
			messageQueue.pop();
		}

		std::string formattedMessage;

		char timestamp[100] = { 0 };
		std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S -- ", &message.timestamp);

		formattedMessage.append(timestamp);
		formattedMessage.append(message.caller);
		formattedMessage.append("::");

		switch (message.gravity)
		{
		case Logger::Fatal:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_RED GTUI_ESC_ENABLE_BOLD);
			#endif
			formattedMessage.append("FATAL: ");
			break;
		case Logger::Error:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_BRIGHT_RED);
			#endif
			formattedMessage.append("Error: ");
			break;
		case Logger::Warning:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_YELLOW);
			#endif
			formattedMessage.append("Warning: ");
			break;
		case Logger::Note:
			//printf(GTUI_ESC_BG_DEFAULT); //No need
			formattedMessage.append("Note: ");
			break;
		case Logger::Success:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_GREEN);
			#endif
			formattedMessage.append("Success: ");
			break;
		default:
			break;
		}

		formattedMessage.append(message.message);
		formattedMessage.append("\n");

		#ifdef SHOW_CONSOLE
		printf("%s" GTUI_ESC_BG_DEFAULT GTUI_ESC_DISABLE_BOLD, formattedMessage.c_str());
		#endif

		fwrite(formattedMessage.c_str(), 1, formattedMessage.size(), fileOutput);
	}
}

static FORCE_INLINE struct tm GetCurrentTimeStamp()
{
	struct tm timestamp;
	time_t now = time(nullptr);
	#ifdef _WIN32
	localtime_s(&timestamp, &now); //Thread safe functions to retrieve the formatted timestamp
	#else
	localtime_r(&now, &timestamp);
	#endif
	return timestamp;
}

void Logger::Initialize(const std::string& outputFilePath)
{
	mutex.lock();
	if (initialized)
	{
		messageQueue.emplace("Logger", "cannot initialize if already initialized", Error, GetCurrentTimeStamp());
		mutex.unlock();

		cv.notify_one();
		return;
	}

	#ifdef SHOW_CONSOLE
	gtuiInitialize();
	#endif

	fileOutput = fopen(outputFilePath.c_str(), "ab+");
	if (!fileOutput)
	{
		std::string message = "failed to open or create the file at path: ";
		message.append(outputFilePath);
		messageQueue.emplace("Logger", message, Error, GetCurrentTimeStamp());
		mutex.unlock();

		cv.notify_one();
		return;
	}

	thread = std::thread(WriteMessages);

	char* startupMessage =
		"******************\n"
		"*                *\n"
		"* LOGGER STARTED *\n"
		"*                *\n"
		"******************\n";

	fwrite(startupMessage, 1, strlen(startupMessage), fileOutput);

	#ifdef SHOW_CONSOLE
	printf(GTUI_ESC_BG_GREEN GTUI_ESC_ENABLE_BOLD);
	printf("%s", startupMessage);
	#endif

	initialized = true;
	mutex.unlock();
}

void Logger::Terminate()
{
	mutex.lock();
	if (!initialized)
	{
		messageQueue.emplace("Logger", "cannot terminate if not initialized", Error, GetCurrentTimeStamp());
		mutex.unlock();

		cv.notify_one();
		return;
	}

	exitFlag = true;
	mutex.unlock();

	#ifdef SHOW_CONSOLE
	gtuiTerminate();
	#endif

	#ifdef SHOW_CONSOLE
	printf(GTUI_ESC_BG_DEFAULT GTUI_ESC_DISABLE_BOLD);
	#endif

	cv.notify_all();
	thread.join(); //Will block the caller thread and wait for WriteMessages to return.

	if (fileOutput)
	{
		fclose(fileOutput);
		fileOutput = nullptr;
	}

	initialized = false;
}

void Logger::PushMessage(const std::string& caller, const std::string& message, MessageGravity gravity)
{
	mutex.lock();

	if (!initialized)
	{
		messageQueue.emplace(Message("Logger", "you must first initialize logger to push a message", Error, GetCurrentTimeStamp()));
		mutex.unlock();

		cv.notify_one();
		return;
	}

	messageQueue.emplace(Message(caller, message, gravity, GetCurrentTimeStamp()));
	mutex.unlock();

	cv.notify_one();
}