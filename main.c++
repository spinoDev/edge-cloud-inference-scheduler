#include <iostream>
#include <vector>
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
// void advanceStage(Request& r) {
//     switch (r.stage) {
//         case Stage::P_PRE:
//             r.stage = Stage::P_PROC;
//             break;

//         case Stage::P_PROC:
//             r.stage = Stage::P_POST;
//             break;

//         case Stage::P_POST:
//             r.stage = Stage::D_PRE;
//             break;

//         case Stage::D_PRE:
//             r.stage = Stage::D_PROC;
//             break;

//         case Stage::D_PROC:
//             r.stage = Stage::D_POST;
//             break;

//         case Stage::D_POST:
//             r.tokensGenerated++;

//             if (r.tokensGenerated >= r.outputLength) {
//                 r.stage = Stage::FINISHED;
//             } else {
//                 r.stage = Stage::D_PRE;
//             }
//             break;

//         case Stage::FINISHED:
//             break;
//     }
// }
void startTask(Request& r, bool& edgeBusy, vector<bool>& cloudBusy) {
    if (r.stage == Stage::P_PRE) {
        cout << "Starting P_PRE\n";
        edgeBusy = true;
    }
    else if (r.stage == Stage::P_PROC) {
        cout << "Starting P_PROC\n";
        cloudBusy[r.cloud] = true;
    }
    else if (r.stage == Stage::P_POST) {
        cout << "Starting P_POST\n";
        edgeBusy = true;
    }
}
void handleTDN(Request& r, bool& edgeBusy, vector<bool>& cloudBusy) {

    switch (r.stage) {

        case Stage::P_PRE:
            edgeBusy = false;
            r.stage = Stage::P_PROC;
            break;

        case Stage::P_PROC:
            cloudBusy[r.cloud] = false;
            r.stage = Stage::P_POST;
            break;

        case Stage::P_POST:
            edgeBusy = false;
            r.stage = Stage::D_PRE;
            break;

        default:
            break;
    }
}
int main() {

    Request r;

    r.id = 0;
    r.inputLength = 100;
    r.cloud = 0;
    r.stage = Stage::P_PRE;
    r.tokensGenerated = 0;
    r.outputLength = 4;
    
    bool edgeBusy = false;
    int numberOfClouds = 2;
    vector<bool> cloudBusy(numberOfClouds, false);

    // while (r.stage != Stage::FINISHED) {
    // advanceStage(r, edgeBusy, cloudBusy);

    // cout << "tokens generated: "
    //      << r.tokensGenerated << endl;
    // }
    startTask(r, edgeBusy, cloudBusy);

    cout << "TDN: P_PRE finished\n";
    handleTDN(r, edgeBusy, cloudBusy);

    startTask(r, edgeBusy, cloudBusy);

    cout << "TDN: P_PROC finished\n";
    handleTDN(r, edgeBusy, cloudBusy);

    startTask(r, edgeBusy, cloudBusy);

    cout << "TDN: P_POST finished\n";
    handleTDN(r, edgeBusy, cloudBusy);

    cout << "Current stage: "
     << static_cast<int>(r.stage) << '\n';

    return 0;
}