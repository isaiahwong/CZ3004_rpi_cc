#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>
#include <thread>

class Protocol {
   private:
    // pointer to the thread
    std::unique_ptr<std::thread> pThread = nullptr;

    // callback function to run thread instance
    static void exec(Protocol* pThread);

   public:
    void start();
    void join();

   protected:
    virtual void run() = 0;
    std::string genFnName(std::string _class, std::string fn);
    std::string genError(std::string prefix, std::string msg);
};

#endif
