#include "commands.h"

Commands::Commands(int instructionDelay, int failCaptureDelay,
                   int cameraRetries) {
    this->instructionDelay = instructionDelay;
    this->failCaptureDelay = failCaptureDelay;
    this->cameraRetries = cameraRetries;
}

void Commands::run() {
    std::thread *receiveActionsThread = new std::thread(
        static_cast<void (*)(void *c)>(Commands::onExecuteActions), this);

    // Push to protocol subthreads
    subThreads.push_back(UniqueThreadPtr(receiveActionsThread));
}

void Commands::resetCommands() {
    Action a;

    while (commands.try_dequeue(a))
        ;
    printRed("Commands cleared");
}

void Commands::resetCache() {
    Action a;
    while (cached.try_dequeue(a))
        ;
    printRed("Cache cleared");
}

void Commands::resetResumeQ() {
    Response r;
    while (resumeQ.try_dequeue(r))
        ;
    printRed("Resume Q cleared");
}

void Commands::resetStatues() {
    Response r;
    while (statuses.try_dequeue(r))
        ;
    printRed("Statuses Q cleared");
}

void Commands::onAction(void *b, Action *action) {
    static_cast<Commands *>(b)->onAction(action);
}

void Commands::onAction(Action *action) {
    resetCommands();
    commands.enqueue(*action);
}

void Commands::onSeriesActions(void *b, Action *action) {
    static_cast<Commands *>(b)->onSeriesActions(action);
}

void Commands::onSeriesActions(Action *action) {
    if (action->data.size() < 1) return;
    resetCommands();
    // Enqueue
    for (Action a : action->data) {
        commands.enqueue(a);
    }
}

void Commands::onSeriesInterleave(void *b, Action *action) {
    static_cast<Commands *>(b)->onSeriesInterleave(action);
}

/**
 * @brief Used to interleave commands
 *
 * @param action
 */
void Commands::onSeriesInterleave(Action *action) {
    if (action->data.size() < 1) return;
    printRed("Running series interleave");

    // backup commands
    tryBackupCommands();

    for (Action a : action->data) commands.enqueue(a);

    // Attempts to resume failed capture from commands queue
    // resetResumeQ();
    Response noop(1);
    resumeQ.enqueue(noop);
}

/**
 * @brief Attempts to back up commands. Does not back up if cache is not empty
 *
 * @param action
 */
void Commands::tryBackupCommands() {
    if (cached.size_approx() == 0) {
        printRed("Done copy commands -> cache");
        Action a;
        while (commands.try_dequeue(a)) cached.enqueue(a);
    }
}

void Commands::onResponse(void *b, Response *response) {
    static_cast<Commands *>(b)->onResponse(response);
}

void Commands::onResponse(Response *response) {
    if (response == nullptr) return;

    // Notify status of message
    statuses.enqueue(*response);
}

void Commands::onReset(void *b, Action *_) {
    static_cast<Commands *>(b)->onReset();
}

void Commands::onReset() {
    resetCommands();
    resetCache();
    resetResumeQ();
    resetStatues();
}

void Commands::onExecuteActions(void *b) {
    static_cast<Commands *>(b)->onExecuteActions();
}

void Commands::onExecuteActions() {
    Action a;
    Response statusResponse;

    // TODO return stop

    int retries = 0, MAX_RETRIES = cameraRetries;
    bool restore = true;
    while (true) {
    queue:
        // Restore commands queue if needed
        if (commands.size_approx() == 0 && cached.size_approx() > 0) {
            printRed("Restoring commands");
            Action a;
            while (cached.try_dequeue(a)) commands.enqueue(a);
        }
        // Clear blocking response queues
        resetResumeQ();
        resetStatues();

        commands.wait_dequeue(a);
        retries = 0;
        // restore = true;

        int minDelay = std::min(instructionDelay, a.delay);

        std::this_thread::sleep_for(std::chrono::milliseconds(minDelay));
        while (true) {
            if (a.type.compare(Action::TYPE_MOVE) == 0) {
                this->publish(Commands::CMD_MOVEMENT, a);
            } else if (a.type.compare(Action::TYPE_CAPTURE) == 0) {
                this->publish(Commands::CMD_CAMERA_CAPTURE, a);
            } else if (a.type.compare(Action::TYPE_NOOP) == 0) {
                goto queue;
            } else {
                printRed("Unknown command");
                goto queue;
            }

            bool didReceive = statuses.wait_dequeue_timed(
                statusResponse, std::chrono::seconds(6));

            // Break queue if successful
            if (didReceive && statusResponse.status == 1) break;

            // Print notification for debug
            printRed("No Response");
            // Retry only for Image Rec
            if (a.type.compare(Action::TYPE_CAPTURE) != 0) break;

            if (retries++ < MAX_RETRIES) continue;
            printRed("Max retries, skipping command");
            statusResponse.status = 0;
            statusResponse.result = "-1";
            break;
            // goto queue;
        }

        try {
            // Send response to android to notify success
            // Echo back the coordinate given
            Response response(a.type, a.action, statusResponse.result,
                              statusResponse.name, statusResponse.status,
                              a.coordinate, a.prev_coordinate,
                              statusResponse.distance);

            this->publish(Commands::CMD_RESPONSE, response);
            print("Command Executed\n\n");

            // Remove this after image
            // Insight code. Strategy will always run the offset direction.
            // We run the offset direction before moving on with queue
            if (a.action.compare(Action::ACTION_STRATEGY) == 0) {
                continue;
                // Action offsetCmd;
                // bool didReceiveOffset = commands.try_dequeue(offsetCmd);
                // if (didReceiveOffset &&
                //     offsetCmd.type.compare(Action::TYPE_MOVE) == 0)
                //     this->publish(Commands::CMD_MOVEMENT, offsetCmd);
            }

            // We stop the thread command queue for calibration and failed
            // status
            if (a.type.compare(Action::TYPE_CAPTURE) == 0) {
                Response noop;
                // Back commands up in the event of interleave
                tryBackupCommands();
                printRed("Waiting for mitigation");
                // Blocking wait for interleave commands
                bool rdidReceive = resumeQ.wait_dequeue_timed(
                    noop, std::chrono::milliseconds(failCaptureDelay));
                std::cout << failCaptureDelay << rdidReceive << std::endl;
            }

        } catch (const std::exception &exc) {
            printRed("onExecuteActions Exeception: ");
            std::cout << exc.what() << std::endl;
            continue;
        }
    }
}

void Commands::onCameraStrategy() {
    // If cached is not empty, we do not clear it
    if (cached.size_approx() > 0) return;
    // If commands is empty, we don't cache it
    if (commands.size_approx() == 0) return;
    printRed("Running camera strategy");
    // empty commands
    Action a;
    while (commands.try_dequeue(a)) cached.enqueue(a);
    printRed("Done copy commands -> cache");
}