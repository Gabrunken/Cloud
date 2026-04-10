#pragma once

#include <ctime>
#include <string>

namespace Logger
{
	enum MessageGravity
	{
		None,
		Fatal,
		Error,
		Warning,
		Note,
		Success
	};

	struct Message
	{
	public:
		std::string caller;
		std::string message;
		MessageGravity gravity = None;
		struct tm timestamp = {};

		Message() = default;

		Message
		(
			const std::string& caller,
			const std::string& message,
			MessageGravity gravity,
			const struct tm& timestamp
		) : caller(caller), message(message), gravity(gravity), timestamp(timestamp)
		{}
	};

	bool Initialize(const std::string& outputFilePath);
	//The caller must be formatted like this, as a convention: "<Namespace/Class>::<Function>"
	void PushMessage(const std::string& caller, const std::string& message, MessageGravity gravity);
	void Terminate();
}