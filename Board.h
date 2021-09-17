#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>
using namespace std;

class Board {
public:
    class Tile;
private:
    int numMines, numFlags;
    vector<Tile*> tiles;
public:
    class Tile {
        bool mine, flag, visible;
        int x, y, adjacents;
        sf::Sprite sprite;
    public:
        Tile(bool mine, int x, int y) {
            this->mine = mine;
            this->flag = false;
            this->visible = false;
            this->x = x;
            this->y = y;
            sprite.setPosition(32 * x, 32 * y);
            this->adjacents = 0;
        }
        int getX() {return x;}
        int getY() {return y;}
        int getAdjacents() {return adjacents;}
        sf::Sprite* getSprite() {return &sprite;}
        bool getVisibility() {return visible;}
        bool hasMine() {return mine;}
        bool hasFlag() {return flag;}
        void addMine() {mine = true;}
        void changeVisibility() {visible = true;}
        void changeFlag() {flag = !flag;}
        void changeAdjacents(int newAdjacents) {adjacents = newAdjacents;}
        Tile(const Tile &tile) {
            this->mine = tile.mine;
            this->flag = tile.flag;
            this->visible = tile.visible;
            this->x = tile.x;
            this->y = tile.y;
            this->adjacents = tile.adjacents;
        }
        Tile& operator= (const Tile& tile) {
            if (&tile != this) {
                this->mine = tile.mine;
                this->flag = tile.flag;
                this->visible = tile.visible;
                this->x = tile.x;
                this->y = tile.y;
                this->adjacents = tile.adjacents;
            }
            return *this;
        }
        ~Tile() {}
    };

    Board() { //50 randoms
        numMines = 50;
        numFlags = 0;
        int x = 0;
        int y = 0;
        for (int i = 0; i < 400; i++) {
            Tile *newTile = new Tile(false, x, y);
            if (x < 25) {
                x++;
            }
            if (y < 16 && x == 25) {
                x = 0;
                y++;
            }
            tiles.push_back(newTile);
        }
        std::random_device random;
        std::mt19937 num(random());
        std::uniform_int_distribution<> distribute(1, 400);
        while (numMines > 0) {
            for (int i = 0; i < 400; i++) {
                int slot = distribute(num);
                int winner = 69;
                if (slot == winner && !tiles[i]->hasMine()) {
                    tiles[i]->addMine();
                    numMines--;
                }
            }
        }
        numMines = 50;
    }

    explicit Board(int boardNumber) { //test boards
        numMines = 0;
        numFlags = 0;
        string boardName = "boards/testboard" + to_string(boardNumber) +".brd";
        char value;
        ifstream file(boardName);
        sf::Texture texture;
        texture.loadFromFile("images/tile_hidden.png");
        int x = 0;
        int y = 0;
        while (file >> value) {
            if (value == '1') {
                numMines++;
                Tile *newTile = new Tile(true, x, y);
                tiles.push_back(newTile);
            }
            else {
                Tile *newTile = new Tile(false, x, y);
                tiles.push_back(newTile);
            }
            if (x < 25) {
                x++;
            }
            if (y < 16 && x == 25) {
                x = 0;
                y++;
            }
        }
    }

    vector<Tile*> *getTiles() {return &tiles;}
    Tile* findMatch(int x, int y) {
        for (auto & tile : tiles) {
            int newX = tile->getX();
            int newY = tile->getY();
            if (newX == x && newY == y) {
                return tile;
            }
        }
        return nullptr;
    }
    int getNumFlags() {return numFlags;}
    int getNumMines() {return numMines;}
    void AddAFlag() {numFlags++;}
    void RemoveAFlag() {numFlags--;}

    Board(const Board &board) {
        numFlags = board.numFlags;
        numMines = board.numMines;
        for (auto tile : board.tiles) {
            tiles.push_back(tile);
        }
    }

    Board& operator= (const Board &board) {
        if (&board != this) {
            numFlags = board.numFlags;
            numMines = board.numMines;
            for (auto tile : board.tiles) {
                tiles.push_back(tile);
            }
        }
        return *this;
    }

    ~Board(){}
};