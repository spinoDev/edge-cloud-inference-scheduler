#include <iostream>
#include <vector>
#include<queue>
#include <climits>
using namespace std;
enum class Stage {
    P_PRE,
    P_PROC,
    P_POST,
    D_PRE,
    D_PROC,
    D_POST,
    FINISHED
};

struct Request {
    int id;
    int inputLength;
    int cloud; // = -1 for cloud selection in future.
    int tokensGenerated;
    int outputLength;
    int startTime;
    int finishTime;
    Stage stage;
};
Stage nextStage(Stage current) {

    switch (current) {
        case Stage::P_PRE:  return Stage::P_PROC;
        case Stage::P_PROC: return Stage::P_POST;
        case Stage::P_POST: return Stage::D_PRE;
        case Stage::D_PRE:  return Stage::D_PROC;
        case Stage::D_PROC: return Stage::D_POST;
        case Stage::D_POST: return Stage::D_PRE;
        default:            return Stage::FINISHED;
    }
}

int stageDuration(Stage stage) {
    switch (stage) {
        case Stage::P_PRE:  return 2;
        case Stage::P_PROC: return 5;
        case Stage::P_POST: return 2;
        case Stage::D_PRE:  return 1;
        case Stage::D_PROC: return 3;
        case Stage::D_POST: return 1;
        default:            return 0;
    }
}
int nextEventTime(
    Request* edgeRunning,
    vector<Request*>& cloudRunning
) {
    int nextTime = INT_MAX;

    if (edgeRunning != nullptr)
        nextTime = min(nextTime, edgeRunning->finishTime);

    for (Request* r : cloudRunning) {
        if (r != nullptr)
            nextTime = min(nextTime, r->finishTime);
    }

    return nextTime;
}
void startTask(
    Request& r,
    Request*& edgeRunning,
    vector<Request*>& cloudRunning,
    int currentTime
) {
    if (r.stage == Stage::P_PRE) {

        if (edgeRunning != nullptr) {
            cout << "Edge busy - request "
                 << r.id << " cannot start\n";
            return;
        }

        edgeRunning = &r;
    }

    else if (r.stage == Stage::P_PROC) {

        if (cloudRunning[r.cloud] != nullptr) {
            cout << "Cloud " << r.cloud
                 << " busy - request "
                 << r.id << " cannot start\n";
            return;
        }

        cloudRunning[r.cloud] = &r;
    }

    else if (r.stage == Stage::P_POST) {

        if (edgeRunning != nullptr) {
            cout << "Edge busy - request "
                 << r.id << " cannot start\n";
            return;
        }

        edgeRunning = &r;
    }

    // Task successfully started
    cout << "Starting " << r.id << " stage "
         << static_cast<int>(r.stage) << "\n";

    r.startTime = currentTime;
    r.finishTime = currentTime + stageDuration(r.stage);

    cout << "  start = " << r.startTime
         << ", finish = " << r.finishTime << "\n";
}
void handleTDN(
    Request& r,
    Request*& edgeRunning,
    vector<Request*>& cloudRunning,
    int currentTime
) {
    switch (r.stage) {

        case Stage::P_PRE:
            edgeRunning = nullptr;
            r.stage = Stage::P_PROC;
            break;

        case Stage::P_PROC:
            cloudRunning[r.cloud] = nullptr;
            r.stage = Stage::P_POST;
            break;

        case Stage::P_POST:
            edgeRunning = nullptr;
            r.stage = Stage::D_PRE;
            break;

        default:
            break;
    }
}

void schedule(
    queue<Request*>& readyQueue,
    Request*& edgeRunning,
    vector<Request*>& cloudRunning,int currentTime
) {
    if (readyQueue.empty())
        return;

    Request* next = readyQueue.front();
    readyQueue.pop();

    startTask(*next, edgeRunning, cloudRunning,currentTime);
}
int main() {

    int numberOfClouds = 2;
    Request* edgeRunning = nullptr;
    vector<Request*> cloudRunning(numberOfClouds, nullptr);
    vector<Request> requests(3);
    int currentTime = 0;

    for (int i = 0; i < requests.size(); i++) {
        requests[i].id = i;
        requests[i].inputLength = 100;
        requests[i].cloud = i % numberOfClouds;
        requests[i].tokensGenerated = 0;
        requests[i].outputLength = 4;
        requests[i].stage = Stage::P_PRE;
    }



    // while (r.stage != Stage::FINISHED) {
    // advanceStage(r, edgeBusy, cloudBusy);

    // cout << "tokens generated: "
    //      << r.tokensGenerated << endl;
    // }
    queue<Request*> readyQueue;
    Request& r = requests[0];
    Request& r1 = requests[1];
    readyQueue.push(&r);
    readyQueue.push(&r1);
    Request* next = readyQueue.front();
    readyQueue.pop();
    startTask(*next, edgeRunning, cloudRunning, currentTime);

    // Find when the next running task finishes
    int nextTime = nextEventTime(edgeRunning, cloudRunning);

    // Jump simulated time forward
    currentTime = nextTime;

    cout << "Current time: " << currentTime << "\n";
    

    cout << "TDN: P_PRE finished\n";
    handleTDN(r, edgeRunning, cloudRunning, currentTime);

    schedule(readyQueue, edgeRunning, cloudRunning,currentTime);

    cout << "TDN: P_PROC finished\n";
    handleTDN(r, edgeRunning, cloudRunning, currentTime);

    schedule(readyQueue, edgeRunning, cloudRunning,currentTime);

    cout << "TDN: P_POST finished\n";
    handleTDN(r, edgeRunning, cloudRunning, currentTime);

    cout << "Current stage: "
     << static_cast<int>(r.stage) << '\n';

    return 0;
}
