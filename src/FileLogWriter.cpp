#include "Herald/FileWriterFactory.hpp"
#include "Herald/ILogWriter.hpp"
#include "Herald/ILogWriterBuilder.hpp"
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace Herald
{
	class FileLogWriter : public ILogWriter
	{
	  public:
		FileLogWriter()
		    : stopWorker(false)
		{
			workerThread = std::thread(&FileLogWriter::processQueue, this);
		}

		virtual ~FileLogWriter() override
		{
			{
				std::lock_guard<std::mutex> lock(queueMutex);
				stopWorker = true; // Signal the worker to stop
			}
			cv.notify_all(); // Wake up the worker thread
			if (workerThread.joinable()) {
				workerThread.join(); // Wait for the worker thread to finish
			}
			close();
		}

		virtual ILogWriter & addConfigPair(const std::string & key, const std::string & value) override
		{
			if (key == "filename") {
				if (value == "stdout") {
					stream = &std::cout; // Refer to stdout
				} else if (value == "stderr") {
					stream = &std::cerr; // Refer to stderr
				} else {
					fileStream = std::make_unique<std::ofstream>(value, std::ios::app); // Open regular file
					if (fileStream->is_open()) {
						stream = fileStream.get();
					} else {
						throw std::runtime_error("Failed to open file: " + value);
					}
				}
			}
			return *this;
		}

		virtual void write(const std::string & msg) override
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			messageQueue.push(msg);
			cv.notify_one(); // Signal the worker thread that there is a new message
		}

	  private:
		std::ostream *                 stream     = nullptr; // Points to the active stream (stdout, stderr, or file)
		std::unique_ptr<std::ofstream> fileStream = nullptr; // Manages file output when writing to a file
		std::queue<std::string>        messageQueue;         // Queue to store log messages
		std::thread                    workerThread;         // Worker thread that processes the message queue
		std::mutex                     queueMutex;           // Mutex to protect access to the message queue
		std::condition_variable        cv;                   // Condition variable to signal the worker thread
		bool                           stopWorker;           // Flag to stop the worker thread

		void close()
		{
			if (fileStream && fileStream->is_open()) {
				fileStream->close();
			}
		}

		void processQueue()
		{
			while (true) {
				std::unique_lock<std::mutex> lock(queueMutex);
				cv.wait(lock, [this] { return !messageQueue.empty() || stopWorker; });

				// Process all messages in the queue
				while (!messageQueue.empty()) {
					auto msg = std::move(messageQueue.front());
					messageQueue.pop();

					// Unlock the queue while writing to avoid blocking other producers
					lock.unlock();
					if (stream) {
						(*stream) << msg << std::endl;
					}
					lock.lock();
				}

				// Exit the loop if the worker is signaled to stop
				if (stopWorker && messageQueue.empty()) {
					break;
				}
			}
		}
	};

	class FileLogWriterBuilder : public ILogWriterBuilder
	{
	  public:
		FileLogWriterBuilder() = default;
		virtual ILogWriterBuilder & addConfigPair(const std::string & key, const std::string & value) override
		{
			configPairs.push_back({key, value});
			return *this;
		}

		virtual ILogWriterPtr build() override
		{
			auto logWriter = std::make_shared<FileLogWriter>();
			for (const auto & [key, value] : configPairs) {
				logWriter->addConfigPair(key, value);
			}
			return logWriter;
		}

	  private:
		std::vector<std::pair<std::string, std::string>> configPairs;
	};

	ILogWriterBuilderPtr createFileWriterBuilder() { return std::make_unique<FileLogWriterBuilder>(); }
} // namespace Herald