
#include <blockingconcurrentqueue.h>

#include "protocol.h"

using BlockingQueueAction = moodycamel::BlockingConcurrentQueue<Action>;
using BlockingQueueRes = moodycamel::BlockingConcurrentQueue<Response>;

class Commands final : public Protocol {
   private:
    /**
     * @brief Blocking queue of commands
     *
     */
    BlockingQueueAction commands;

    /**
     * @brief Store commands temporarily
     *
     */
    BlockingQueueAction cached;

    BlockingQueueRes statuses;

    int instructionDelay = 500;

    void resetCommands();
    void resetCache();

   public:
    inline static const std::string CMD_CAMERA_CAPTURE = "CMD_CAMERA_CAPTURE";
    inline static const std::string CMD_MOVEMENT = "CMD_MOVEMENT";
    inline static const std::string CMD_RESPONSE = "CMD_RESPONSE";

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onResponse(void* c, Response* response);

    void onResponse(Response* response);

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onExecuteActions(void* b);

    void onExecuteActions();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onSeriesActions(void* b, Action* action);

    void onSeriesActions(Action* action);

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onAction(void* b, Action* action);

    void onAction(Action* action);

    void onCameraStrategy();

    static void onReset(void* b, Action* noop);

    void onReset();

   public:
    Commands(int instructionDelay);

    void run();
};