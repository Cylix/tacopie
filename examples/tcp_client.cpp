// MIT License
//
// Copyright (c) 2016-2017 Simon Ninon <simon.ninon@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <tacopie/tacopie>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <signal.h>

#ifdef _WIN32
#include <Winsock2.h>

//! Windows netword DLL init
struct _INIT_WSA_ {

	_INIT_WSA_() {
		WORD version = MAKEWORD(2, 2);
		WSADATA data;

		if (WSAStartup(version, &data) != 0) {
			std::cerr << "WSAStartup() failure" << std::endl;
		}
	};

	~_INIT_WSA_() {
		WSACleanup();
	};

} __INIT_WSA__;


#endif /* _WIN32 */


std::condition_variable cv;

void
signint_handler(int) {
	cv.notify_all();
}

void
on_new_message(tacopie::tcp_client& client, const tacopie::tcp_client::read_result& res) {
	if (res.success) {
		std::string str;
		str.insert(str.begin(), res.buffer.begin(), res.buffer.end());
		std::cout << "Client recv data:" << str << std::endl;
		//client.async_write({ res.buffer, nullptr });
		client.async_read({ 1024, std::bind(&on_new_message, std::ref(client), std::placeholders::_1) });
	}
	else {
		std::cout << "Client disconnected" << std::endl;
		client.disconnect();
	}
}

int
main(void) {
	tacopie::tcp_client client;
	client.connect("localhost", 3001);
	client.async_read({ 1024, std::bind(&on_new_message, std::ref(client), std::placeholders::_1) });

	std::string str = "hello world!";
	std::vector<char> buff;
	buff.insert(buff.begin(), str.begin(), str.end());

	client.async_write({ buff, nullptr });
	signal(SIGINT, &signint_handler);

	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait(lock);

	return 0;
}
