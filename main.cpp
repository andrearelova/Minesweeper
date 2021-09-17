#include "Board.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
using namespace std;

void OtherButtons(sf::RenderWindow &window);
void ChangeFace(const string& condition , sf::RenderWindow &window);
void FlagCounter(sf::RenderWindow &window, int flagsLeft);
void CheckNeighbors (Board::Tile *tile, Board &board);
void NeighborRecursion(Board::Tile* loopTile, Board &board);
vector<Board::Tile*> GetNeighbors(Board::Tile* tile, Board &board);

int main() {
    map<int, sf::Sprite> adjacents;
    for (int i = 1; i <= 8; i++) {
        string fileName = "images/number_" + to_string(i) + ".png";
        sf::Texture texture;
        texture.loadFromFile(fileName);
        sf::Sprite sprite;
        sprite.setTexture(texture);
        adjacents.insert(make_pair(i, sprite));
    }

    string condition = "default";
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minesweeper");
    map<string, sf::Sprite> spritesOnBoard;
    auto *board = new Board();
    auto debug = false;
    auto lost = false;
    auto won = false;
    sf::Event event{};
    int flagsWinCheck = 0;
    int revealedTilesWinCheck = 0;
    int temp1 = 0;
    int temp2 = 0;

    while (window.isOpen()) {
        window.clear(sf::Color::White);
        vector<Board::Tile*> tiles = *board->getTiles();
        if (revealedTilesWinCheck + flagsWinCheck == 400) {
            won = true;
            lost = false;
            condition = "win";
        }
        if (lost) {
            condition = "lose";
            ChangeFace(condition, window);

        }
        if (won) {
            condition = "win";
            ChangeFace(condition, window);
        }
        ChangeFace(condition, window);
        OtherButtons(window);

        //assign mine pics to the board
        int count = 0;
        for (auto &myTile : tiles) {
            int x = (myTile->getX() * 32);
            int y = (myTile->getY() * 32);

            CheckNeighbors(myTile, *board);
            int numNeighbors = myTile->getAdjacents();
            sf::Texture number;
            sf::Sprite numberSprite;
            if (numNeighbors > 0 && myTile->getVisibility()) {
                string fileName = "images/number_" + to_string(numNeighbors) + ".png";
                number.loadFromFile(fileName);
                numberSprite.setTexture(number);
                numberSprite.setPosition(x, y);
            }

            sf::Texture texture;
            if (myTile->getVisibility()) { //if visible tile
                texture.loadFromFile("images/tile_revealed.png");
            } else {
                texture.loadFromFile("images/tile_hidden.png");
            }


            sf::Texture flag;
            sf::Sprite flagSprite;
            if (myTile->hasFlag() && !myTile->getVisibility()) {
                flag.loadFromFile("images/flag.png");
                flagSprite.setTexture(flag);
            }
            flagSprite.setPosition(x, y);

            sf::Texture mine;
            sf::Sprite mineSprite;
            if (myTile->hasMine() && (myTile->getVisibility() || debug || won || lost)) {
                mine.loadFromFile("images/mine.png");
                mineSprite.setTexture(mine);
            }
            mineSprite.setPosition(x, y);

            sf::Sprite mySprite;
            mySprite.setTexture(texture);
            mySprite.setPosition(x, y);
            window.draw(mySprite);
            window.draw(numberSprite);
            window.draw(flagSprite);
            window.draw(mineSprite);

            string key = "hiddenTile" + to_string(count);
            count++;
            spritesOnBoard.insert(make_pair(key, mySprite));
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && !won && !lost) {
                    map<string, sf::Sprite>::iterator current;
                    int myCount = 0;
                    for (current = spritesOnBoard.begin(); current != spritesOnBoard.end(); current++) { //if left clicked any tile
                        string myKey = "hiddenTile" + to_string(myCount);
                        Board::Tile currentTile = *tiles[myCount];
                        sf::Sprite sprite = spritesOnBoard[myKey];
                        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        sf::FloatRect bounds = sprite.getGlobalBounds();


                        if (bounds.contains(mouse) && !currentTile.getVisibility()) {
                            if (tiles[myCount]->hasFlag()) {
                                board->RemoveAFlag();
                            }
                            else {
                                board->AddAFlag();
                            }
                            tiles[myCount]->changeFlag();
                        }
                        myCount++;
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left ) {
                    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    //check for tests 1-3
                    int i;
                    for (i = 1; i <= 3; i++) {
                        float left = 494.0f + (i * 64.0f);
                        float top = 512.0f;
                        sf::FloatRect bounds(left, top, 64.0f, 64.0f);

                        if (bounds.contains(mouse)) {
                            delete board;
                            board = new Board(i);
                            condition = "default";
                            won = false;
                            lost = false;
                            revealedTilesWinCheck = 0;
                            flagsWinCheck = 0;
                            break;
                        }
                    }

                    //check for smiley
                    sf::FloatRect smileBounds(367.0f, 512.0f, 64.0f, 64.0f);
                    if (smileBounds.contains(mouse)) {
                        delete board;
                        board = new Board();
                        condition = "default";
                        won = false;
                        lost = false;
                        revealedTilesWinCheck = 0;
                        flagsWinCheck = 0;
                        break;
                    }

                    //check for debug
                    sf::FloatRect debugBounds(494.0f, 512.0f, 64.0f, 64.0f);
                    if (debugBounds.contains(mouse)) {
                        debug = !debug;
                        break;
                    }

                    if (!won && !lost) {
                        //check if click on tile
                        map<string, sf::Sprite>::iterator current;
                        int myCount = 0;
                        for (current = spritesOnBoard.begin();
                             current != spritesOnBoard.end(); current++) { //if left clicked any tile
                            string myKey = "hiddenTile" + to_string(myCount);
                            Board::Tile currentTile = *tiles[myCount];
                            sf::Sprite sprite = spritesOnBoard[myKey];
                            sf::FloatRect bounds = sprite.getGlobalBounds();

                            if (bounds.contains(mouse) && !currentTile.getVisibility() && !currentTile.hasFlag()) {
                                tiles[myCount]->changeVisibility();
                                if (currentTile.hasMine()) {
                                    lost = true;
                                }
                                else {
                                    Board::Tile *loopTile = tiles[myCount];
                                    NeighborRecursion(loopTile, *board);
                                }
                                break;
                            }
                            myCount++;
                        }
                    }
                }
            }
        }
        int flagsLeft = board->getNumMines() - board->getNumFlags();
        FlagCounter(window, flagsLeft);

        temp1 = 0;
        temp2 = 0;
        for (auto & tile : tiles) {
            if (tile->getVisibility()) {
                temp1++;
            }
            if (tile->hasFlag() && tile->hasMine()) {
                temp2++;
            }
        }
        revealedTilesWinCheck = temp1;
        flagsWinCheck = temp2;

        window.display();
    }
}

void ChangeFace(const string& condition , sf::RenderWindow &window) {
    sf::Sprite sprite;
    sf::Texture texture;
    if (condition == "win") {
        texture.loadFromFile("images/face_win.png");
        sprite.setTexture(texture);
    }
    else if (condition == "lose") {
        texture.loadFromFile("images/face_lose.png");
        sprite.setTexture(texture);
    }
    else if (condition == "default") {
        texture.loadFromFile("images/face_happy.png");
        sprite.setTexture(texture);
    }
    sprite.setPosition(367.0f, 512.0f);
    window.draw(sprite);
}

void OtherButtons(sf::RenderWindow &window) {
    sf::Sprite sprite;
    sf::Texture texture;
    texture.loadFromFile("images/debug.png");
    sprite.setTexture(texture);
    sprite.setPosition(494.0f, 512.0f);
    window.draw(sprite);

    for (int i = 1; i <= 3; i++) {
        string name = "images/test_" + to_string(i) + ".png";
        texture.loadFromFile(name);
        sprite.setTexture(texture);
        float x = 494.0f + (i * 64.0f);
        sprite.setPosition(x, 512.0f);
        window.draw(sprite);
    }
}

void FlagCounter(sf::RenderWindow &window, int flagsLeft) {
    map<string, sf::Texture> numbers;
    sf::Texture texture;
    sf::Sprite sprite;
    int startX = 0;
    int startY = 0;
    for (int i = 0; i < 10; i++) {
        startX = i * 21;
        texture.loadFromFile("images/digits.png", sf::IntRect(startX, startY, 21, 32));
        string key = to_string(i);
        numbers.insert(make_pair(key, texture));
    }
    startX = 10 * 21;
    texture.loadFromFile("images/digits.png", sf::IntRect(startX, startY, 21, 32));
    numbers.insert(make_pair("-", texture));
    if (flagsLeft != 0) {
        if (flagsLeft < 0) { //draw "-" first
            sprite.setTexture(numbers["-"]);
            sprite.setPosition(0.0f, 512.0f);
            window.draw(sprite);
        }
        float x;
        float y = 512.0f;
        if (flagsLeft < 0) {
            x = 21.0f;
            flagsLeft *= -1;
        }
        else {
            x = 0.0f;
        }
        vector<int> displayNums;
        while (flagsLeft > 0) {
            int d = flagsLeft % 10;
            displayNums.emplace_back(d);
            flagsLeft /= 10;
        }
        for (int i = 0; i < displayNums.size(); i++) {
            x += (i * 21.0f);
            int index = displayNums.size() - 1 - i;
            int use = displayNums[index];
            string mapKey = to_string(use);
            sprite.setTexture(numbers[mapKey]);
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }
    else {
        sprite.setTexture(numbers["0"]);
        sprite.setPosition(0.0f, 512.0f);
        window.draw(sprite);
    }
}

void CheckNeighbors (Board::Tile *tile, Board &board) { //adds number of mines next to it, not tiles in general
    int x = tile->getX();
    int y = tile->getY();
    vector<Board::Tile*> neighbors;

    //neighbor 1
    int newX = x;
    int newY = y + 1;
    if (newY < 16 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    //neighbor 2
    newY = y - 1;
    if (newY < 16 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    //neighbor 3
    newX = x - 1;
    newY = y - 1;
    if (newX >= 0 && newY < 16 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    //neighbor 4
    newY = y;
    if (newX >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    //neighbor 5
    newY = y + 1;
    if (newY < 16 && newY >= 0 && newX >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    //neighbor 6
    newY = y;
    newX = x + 1;
    if (newX < 25) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    //neighbor 7
    newY = y - 1;
    if (newX < 25 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    //neighbor 8
    newY = y + 1;
    if (newX < 25 && newY < 16) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (neighbor->hasMine())
        neighbors.push_back(neighbor);
    }

    tile->changeAdjacents(neighbors.size());
}

vector<Board::Tile*> GetNeighbors(Board::Tile* tile, Board &board) { //returns EMPTY neighbors
    int x = tile->getX();
    int y = tile->getY();
    vector<Board::Tile*> neighbors;

    //neighbor 1
    int newX = x;
    int newY = y + 1;
    if (newY < 16 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }

    //neighbor 2
    newY = y - 1;
    if (newY < 16 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }

    //neighbor 3
    newX = x - 1;
    newY = y - 1;
    if (newX >= 0 && newY < 16 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }

    //neighbor 4
    newY = y;
    if (newX >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }

    //neighbor 5
    newY = y + 1;
    if (newY < 16 && newY >= 0 && newX >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }

    //neighbor 6
    newY = y;
    newX = x + 1;
    if (newX < 25) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }

    //neighbor 7
    newY = y - 1;
    if (newX < 25 && newY >= 0) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }

    //neighbor 8
    newY = y + 1;
    if (newX < 25 && newY < 16) {
        Board::Tile *neighbor = board.findMatch(newX, newY);
        if (!neighbor->hasMine())
            neighbors.push_back(neighbor);
    }
    return neighbors;
}

void NeighborRecursion(Board::Tile* loopTile, Board &board) {
    if (loopTile->getAdjacents() == 0) { //if you have 0 neighbor bombs, reveal the ones around u
        vector<Board::Tile*> neighbors = GetNeighbors(loopTile, board);
        for (auto & neighbor : neighbors) {
            if (!neighbor->hasMine() && !neighbor->getVisibility()) {
                neighbor->changeVisibility();
                NeighborRecursion(neighbor, board);
            }
        }
    }
}