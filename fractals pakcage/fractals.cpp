#include <cstdlib>
#include <iostream>
#include <string>
#include <windows.h>
#include <tuple>
#include <conio.h>
#include <vector>
#include <sstream>
using namespace std;

struct appConfig{
    int state = 0;
    int matrixSize = 255;
    double zoom = 1;
    double xOffset = -1;
    double yOffset = 0;
    bool zooming = false;
    int columns;
    int rows;
    appConfig(CONSOLE_SCREEN_BUFFER_INFO csbi): columns(csbi.srWindow.Right - csbi.srWindow.Left + 1), rows(csbi.srWindow.Bottom - csbi.srWindow.Top + 1){}
};

struct menuObject{
        bool isSelected = false;
        char type = 'b';
        std::string label;
        std::vector<std::string> options; 
        int currentOption = 0;
        
        menuObject(string l, char t, const vector<string>& opt = {}) 
        : label(l), type(t), options(opt){}

        string getStr(){
            if (this->type == 'b'){
                if (this->isSelected){
                    return "*" + this->label;
                }else{
                    return " " + this->label;
                }
            }else if(this->type == 'l'){
                if (this->isSelected){
                    return "*" + this->label + (this->currentOption == 0 ? "  " : "< ") + this->options[this->currentOption] + (this->currentOption == this->options.size() - 1 ? "  " : " >");
                }else{
                    return " " + this->label + (this->currentOption == 0 ? "  " : "< ") + this->options[this->currentOption] + (this->currentOption == this->options.size() - 1 ? "  " : " >");
                }
            }
            return "";
        }

        void changeOption(char d){
            if (this->type == 'l'){
                if (d == 'n'){
                    this->currentOption = (this->currentOption - 1) % this->options.size();
                }else if(d =='p'){
                    this->currentOption = (this->currentOption + 1) % this->options.size();
                }
            }
        }

        string getValue(){
            if(this->type == 'l'){
                return this->options[this->currentOption];
            }
        }
    };

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void printFractal(appConfig &config){
    string stringToOut = "";
    string line;

    double maxIterations = 500;
    int prewColor;
    int zoomIteration = 0;

    string s(max(0,(config.columns - config.matrixSize * 2)/2), ' ');

    fwrite("\033[2J", 1, 4, stdout);
    while (true){
        stringToOut = "";
        
        for (int i = 0; i < config.matrixSize ; ++i){
            line = "";    
            for (int j = 0; j < config.matrixSize ; ++j){
                double x0 = (j - config.matrixSize/2.0 ) / (config.matrixSize/4.0) / config.zoom + config.xOffset;
                double y0 = (i - config.matrixSize/2.0 ) / (config.matrixSize/4.0) / config.zoom + config.yOffset;
                
                double x = 0, y = 0;
                int iteration = 0;

                while (x*x + y*y <= 4 && iteration < maxIterations) {
                    double xtemp = x*x - y*y + x0;
                    y = 2*x*y + y0;
                    x = xtemp;
                    iteration++;
                }

                if (iteration == maxIterations){
                    if (prewColor != 0){
                        line += "\033[48;5;0m  ";
                        prewColor = 0;
                    }
                    else{
                        line += "  ";
                    }
                }
                else {
                    int color = (iteration * 255 / maxIterations);
                    if(color != prewColor){
                        line += "\033[48;2;" + to_string(i*(255/config.matrixSize)) + ";" + to_string(j*(255/config.matrixSize)) +";" + to_string(color) + "m  "; 
                        prewColor = color;
                    }
                    else{
                        line += "  ";
                    }
                }
            }
            stringToOut += s + line + "\n" + "\033[0m";
        }
            
            string s(max(0,(config.rows - config.matrixSize)/2), '\n');

            stringToOut = s + stringToOut;
            
            stringToOut = "\033[H" + stringToOut + "\033[0m";
            fwrite(stringToOut.c_str(), 1, stringToOut.size(), stdout);

            if (config.zooming){
                    if (zoomIteration < 42){
                    config.zoom *= 1.1;
                    config.xOffset -= 0.07889 / config.zoom;
                    ++zoomIteration;
                }
                else{
                    config.zoom = 1;
                    config.xOffset = -1;
                    zoomIteration = 0;
                }
            }

            if (_kbhit()) { 
                char key = _getch(); 
                if (key == 'w') config.yOffset -= 0.1 / config.zoom;
                if (key == 's') config.yOffset += 0.1 / config.zoom;
                if (key == 'a') config.xOffset -= 0.1 / config.zoom;
                if (key == 'd') config.xOffset += 0.1 / config.zoom;
                if (key == '+') config.zoom *= 1.1;
                if (key == '-') config.zoom /= 1.1;
                if (key == 'z') config.zooming ? config.zooming = false : config.zooming = true;
                if (key == 27) {config.zoom = 1; config.xOffset = -1; config.yOffset = 0;} 
                if (key == 'x') {config.state = 0; break;}
            }
        }
}

void printMainMenu(appConfig &config){
    
    menuObject fractalType("Fractal type", 'l',vector<string> {"Mandelbrot set"});
    menuObject resolution("Image resolution", 'l', vector<string> {"16","32","64","128","255"});
    menuObject startBtn("Start", 'b');

    vector<menuObject> menuList = {fractalType, resolution, startBtn};

    int selectedItem = 0;
    string outputStr;

    outputStr = "";
    menuList[selectedItem].isSelected = true;

    for (int i  = 0; i < menuList.size(); ++i){
        string s(max(0,(config.columns - menuList[i].getStr().size())/2), ' ');
        outputStr += s +  menuList[i].getStr() + "\n";
    }
    
    string s(max(0,(config.rows - split(outputStr, '\n').size())/2), '\n');

    outputStr = s + outputStr;

    fwrite("\033[2J", 1, 4, stdout);
    outputStr = "\033[H" + outputStr + "\033[0m";
    fwrite(outputStr.c_str(), 1, outputStr.size(), stdout);

    while (true){
        if (_kbhit()){
            char key = _getch();
            if (key == 72) {menuList[selectedItem].isSelected = false; selectedItem = (selectedItem - 1 + menuList.size()) % menuList.size(); menuList[selectedItem].isSelected = true;}
            if (key == 80) {menuList[selectedItem].isSelected = false; selectedItem = (selectedItem + 1) % menuList.size(); menuList[selectedItem].isSelected = true;}
            if (key == 75 && menuList[selectedItem].type == 'l') menuList[selectedItem].changeOption('n');
            if (key == 77 && menuList[selectedItem].type == 'l') menuList[selectedItem].changeOption('p');
            if (key == '\r' && menuList[selectedItem].label == "Start") {config.state = 1; config.matrixSize = stoi(menuList[1].getValue()) ; break;}
            if (key == 27) {config.state = 2; break;}
            
            outputStr = "";
            for (int i  = 0; i < menuList.size(); ++i){
                string s(max(0,(config.columns - menuList[i].getStr().size())/2), ' ');
                outputStr += s +  menuList[i].getStr() + "\n";
            }
            
            string s(max(0,(config.rows - split(outputStr, '\n').size())/2), '\n');

            outputStr = s + outputStr;

            fwrite("\033[2J", 1, 4, stdout);
            outputStr = "\033[H" + outputStr + "\033[0m";
            fwrite(outputStr.c_str(), 1, outputStr.size(), stdout);
        }
    }
}

int main(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return 1;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return 1;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) return 1;
    
    SetConsoleOutputCP(CP_UTF8);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    appConfig config(csbi);

    while (true)
    {
        if (config.state == 0){
            printMainMenu(config);
        }else if(config.state == 1){
            printFractal(config);
        }else if(config.state == 2){
            break;
        }
    }
    
    return 0;
    }
