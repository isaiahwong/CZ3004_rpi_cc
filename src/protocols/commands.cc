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

    if (cached.size_approx() == 0) {
        printRed("Done copy commands -> cache");
        Action a;
        while (commands.try_dequeue(a)) cached.enqueue(a);
    }

    for (Action a : action->data) {
        commands.enqueue(a);
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
        // Restore commands queue if needed
        if (commands.size_approx() == 0 && cached.size_approx() > 0) {
            printRed("Restoring commands");
            Action a;
            while (cached.try_dequeue(a)) commands.enqueue(a);
        }

    queue:
        commands.wait_dequeue(a);
        retries = 0;
        // restore = true;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(instructionDelay));
        while (true) {
            if (a.type.compare(Action::TYPE_MOVE) == 0) {
                this->publish(Commands::CMD_MOVEMENT, a);
            } else if (a.type.compare(Action::TYPE_CAPTURE) == 0) {
                this->publish(Commands::CMD_CAMERA_CAPTURE, a);
            } else {
                printRed("Unknown command in series");
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

            // Do not save commands or delay if it's an action strategy
            if (a.action.compare(Action::ACTION_STRATEGY)) continue;

            // We retry for calibrate and failed status
            if (a.type.compare(Action::TYPE_CAPTURE) == 0 &&
                (statusResponse.status != 1 ||
                 a.action.find(Action::ACTION_CALIBRATE) !=
                     std::string::npos)) {
                // Back commands up in the event of interleave
                if (cached.size_approx() == 0) {
                    printRed("Done copy commands -> cache");
                    Action a;
                    while (commands.try_dequeue(a)) cached.enqueue(a);
                }

                // Wait for android commands
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(failCaptureDelay));
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
