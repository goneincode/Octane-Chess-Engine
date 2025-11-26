#ifndef UCI_H
#define UCI_H

#include "Board.h"
#include "Search.h"

class UCI {
public:
    // Starts the UCI loop. This blocks until "quit" is received.
    void loop();

private:
    Board board;
    Search search;

    void handlePosition(const std::string& line);
    void handleGo(const std::string& line);
};

#endif // UCI_H
