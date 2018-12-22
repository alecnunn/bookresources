
#include "..\Engine\Advanced2D.h"

#pragma once


namespace Advanced2D {

    class Console {

    private:
        bool showing;
        Sprite *panel;
        Font *font;
        int currentLine;
        std::vector<std::string> textlines;
        std::vector<std::string>::iterator iter;

    public:
        Console();
        virtual ~Console();
        bool init();
        void draw();
        void clear();
        void print(std::string text, int line = -1);
        bool isShowing() { return this->showing; }
        void show() { this->showing = true; }
        void hide() { this->showing = false; }
        void setShowing(bool value) { this->showing = value; }
    };

};
