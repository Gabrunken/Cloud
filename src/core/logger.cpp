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

#include <tracy/Tracy.hpp>

static bool initialized = false;
static FILE* fileOutput;

static std::queue<Logger::Message> messageQueue;
static std::thread thread;
static std::condition_variable cv;
static bool exitFlag;

static std::mutex mutex;

static void WriteMessages()
{
	tracy::SetThreadName("Logger Thread");

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
		formattedMessage.append(" -- ");

		switch (message.gravity)
		{
		case Logger::Fatal:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_RED GTUI_ESC_ENABLE_BOLD);
			#endif
			formattedMessage.append("FATAL -- ");
			break;
		case Logger::Error:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_BRIGHT_RED);
			#endif
			formattedMessage.append("Error -- ");
			break;
		case Logger::Warning:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_YELLOW);
			#endif
			formattedMessage.append("Warning -- ");
			break;
		case Logger::Note:
			//printf(GTUI_ESC_BG_DEFAULT); //No need
			formattedMessage.append("Note -- ");
			break;
		case Logger::Success:
			#ifdef SHOW_CONSOLE
			printf(GTUI_ESC_BG_GREEN);
			#endif
			formattedMessage.append("Success -- ");
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

bool Logger::Initialize(const std::string& outputFilePath)
{
	mutex.lock();
	if (initialized)
	{
		messageQueue.emplace("Logger::Initialize", "Cannot initialize if already initialized", Warning, GetCurrentTimeStamp());
		mutex.unlock();

		cv.notify_one();
		return true;
	}

	#ifdef SHOW_CONSOLE
	gtuiInitialize();
	#endif

	fileOutput = fopen(outputFilePath.c_str(), "ab+");
	if (!fileOutput)
	{
		#ifdef SHOW_CONSOLE
		printf("Logger::Initialize -- FATAL -- Cannot open or create the log file at filepath: %s\n", outputFilePath.c_str());
		#endif
		mutex.unlock();
		return false;
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

	return true;
}

void Logger::Terminate()
{
	mutex.lock();
	if (!initialized)
	{
		#ifdef SHOW_CONSOLE
		printf("Logger::Terminate -- Warning -- Cannot terminate this module if it has not been initialized.\n");
		#endif
		mutex.unlock();
		return;
	}

	exitFlag = true;
	mutex.unlock();

	cv.notify_all();
	thread.join(); //Will block the caller thread and wait for WriteMessages to return.

	#ifdef SHOW_CONSOLE
	printf(GTUI_ESC_BG_GREEN GTUI_ESC_ENABLE_BOLD "Logger::Terminate -- Success -- logger terminated successfully.\n" GTUI_ESC_BG_DEFAULT GTUI_ESC_DISABLE_BOLD);
	gtuiTerminate();
	#endif

	char* exitMessage = "Logger::Terminate -- Success -- logger terminated successfully.\n";
	fwrite(exitMessage, 1, strlen(exitMessage), fileOutput);

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
		#ifdef SHOW_CONSOLE
		printf("Logger::PushMessage -- Error -- Cannot push a message if this module has not been initialized.\n");
		#endif
		mutex.unlock();
		return;
	}

	messageQueue.emplace(Message(caller, message, gravity, GetCurrentTimeStamp()));
	mutex.unlock();

	cv.notify_one();
}