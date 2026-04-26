#include <cstdlib>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <sstream>
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/screen.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/input.hpp"
#include <chrono>
#include <algorithm>
#include <thread>
using namespace std;

struct appConfig{
    int state = 0;
    int matrixSize = 255;
    double zoom = 1;
    double xOffset = -1;
    double yOffset = 0;
    int columns;
    int rows;
    int fractalType = 0;
    double maxIterations = 500;
    int maxMatrixSize = 255;
    bool autoScale = false;
    appConfig(int rows, int columns): columns(columns), rows(rows){}
    appConfig(){};
    
    void reset(){
        this->state = 0;
        this->matrixSize = 255;
        this->zoom = 1;
        this->xOffset = -1;
        this->yOffset = 0;
        this->fractalType = 0;
        this->maxIterations = 500;
        this->autoScale = false;
    }

    bool isEqual(appConfig config){
        if (this->zoom != config.zoom)return false;
        else if (this->xOffset != config.xOffset) return false;
        else if (this->yOffset != config.yOffset) return false;
        else if (this->matrixSize != config.matrixSize) return false;
        return true;
    }

    void setMatrixSize(int newSize){
        if(newSize <= maxMatrixSize){
            this->matrixSize = newSize;
        }
    }
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
                if (d == 'p'){
                    this->currentOption = (this->currentOption + options.size() - 1) % this->options.size();
                }else if(d =='n'){
                    this->currentOption = (this->currentOption + 1) % this->options.size();
                }
            }
        }

        string getValue(){
            if(this->type == 'l'){
                return this->options[this->currentOption];
            }
            return "";
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

int juliaSet(appConfig &config, int i, int j){
    double zx = (j - config.matrixSize/2.0 ) / (config.matrixSize/4.0) / config.zoom + config.xOffset;
    double zy = (i - config.matrixSize/2.0 ) / (config.matrixSize/4.0) / config.zoom + config.yOffset;
    
    double cx = -0.7, cy = 0.27015;
    int iteration = 0;

    while (zx*zx + zy*zy <= 4 && iteration < config.maxIterations) {
        double xtemp = zx*zx - zy*zy + cx;
        zy = 2*zx*zy + cy;
        zx = xtemp;
        iteration++;
    }
    return iteration;
}

int mandelbrotSet(appConfig &config, int i, int j){
    double x0 = (j - config.matrixSize/2.0 ) / (config.matrixSize/4.0) / config.zoom + config.xOffset;
    double y0 = (i - config.matrixSize/2.0 ) / (config.matrixSize/4.0) / config.zoom + config.yOffset;
    
    double x = 0, y = 0;
    int iteration = 0;

    while (x*x + y*y <= 4 && iteration < config.maxIterations) {
        double xtemp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtemp;
        iteration++;
    }
    return iteration;
}

void printFractals(appConfig &config){
    string stringToOut = "";
    string line;

    appConfig cachedConfig;
    cachedConfig.zoom = config.zoom - 1;
    cachedConfig.xOffset = config.xOffset - 1;
    cachedConfig.yOffset = config.yOffset - 1;

    int prewColor = -1;

    string s(max(0,(config.columns - config.matrixSize * 2)/2), ' ');

    fwrite("\033[2J", 1, 4, stdout);
    while (true){
        stringToOut = "";
        Term::Screen screen = Term::screen_size();
        if (config.rows != static_cast<int>(screen.rows()) || config.columns != static_cast<int>(screen.columns())){
            config.rows = screen.rows();
            config.columns = screen.columns();
            if (config.autoScale) config.setMatrixSize(min((config.columns / 2) - 2, config.rows - 2));
        }
        
        if (!config.isEqual(cachedConfig)){

            for (int i = 0; i < config.matrixSize ; ++i){
                line = ""; 
                prewColor = -1;   
                for (int j = 0; j < config.matrixSize ; ++j){
                    
                    int iteration = 0;  

                    switch (config.fractalType)
                    {
                    case 0:
                        iteration = mandelbrotSet(config, i, j);
                        break;
                    
                    case 1:
                        iteration = juliaSet(config, i, j);
                        break;

                    default:
                        break;
                    }

                    if (iteration == config.maxIterations){
                        if (prewColor != 0){
                            line += "\033[48;5;0m  ";
                            prewColor = 0;
                        }
                        else{
                            line += "  ";
                        }
                    }
                    else {
                        int color = (iteration * 255 / config.maxIterations);
                        if(color != prewColor){
                            line += "\033[48;2;" + to_string(i*(255/config.matrixSize)) + ";" + to_string(j*(255/config.matrixSize)) + ";" + to_string(color) + "m  "; 
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
                fflush(stdout);
                
                cachedConfig.zoom = config.zoom;
                cachedConfig.xOffset = config.xOffset;
                cachedConfig.yOffset = config.yOffset;

        }

            Term::Event event = Term::read_event();
            if (event.type() == Term::Event::Type::Key) {
            Term::Key key(event);
            if (key == 'w') config.yOffset -= 0.1 / config.zoom;
            if (key == 's') config.yOffset += 0.1 / config.zoom;
            if (key == 'a') config.xOffset -= 0.1 / config.zoom;
            if (key == 'd') config.xOffset += 0.1 / config.zoom;
            if (key == '+') config.zoom *= 1.1;
            if (key == '-' ) config.zoom /= 1.1;
            if (key == 'x') {config.zoom = 1; config.xOffset = -1; config.yOffset = 0;} 
            if (key == Term::Key::Esc) {config.state = 0; return;}
            while(!Term::read_event().empty()){
                
            }
            }
            
            this_thread::sleep_for(chrono::milliseconds(10));
        }   
        
}

void printMainMenu(appConfig &config){
    
    menuObject fractalType("Fractal type", 'l',vector<string> {"Mandelbrot set", "Julia set"});
    menuObject resolution("Image resolution", 'l', vector<string> {"auto","16","32","64"});
    menuObject startBtn("Start", 'b');

    vector<menuObject> menuList = {fractalType, resolution, startBtn};

    int selectedItem = 0;
    string outputStr;

    outputStr = "";
    menuList[selectedItem].isSelected = true;

    for (int i  = 0; i < menuList.size(); ++i){
        string s(max(0,static_cast<int>((config.columns - menuList[i].getStr().size())/2)), ' ');
        outputStr += s +  menuList[i].getStr() + "\n";
    }
    
    string s(max(0,static_cast<int>((config.rows - split(outputStr, '\n').size())/2)), '\n');

    outputStr = s + outputStr;

    fwrite("\033[2J", 1, 4, stdout);
    fflush(stdout);
    outputStr = "\033[H" + outputStr + "\033[0m";
    fwrite(outputStr.c_str(), 1, outputStr.size(), stdout);
    fflush(stdout);

    while (true){
        Term::Screen screen = Term::screen_size();
        if (config.rows != static_cast<int>(screen.rows()) || config.columns != static_cast<int>(screen.columns())){
            config.rows = screen.rows();
            config.columns = screen.columns();
        }

        Term::Event event = Term::read_event();
        if (event.type() == Term::Event::Type::Key) {
            Term::Key key(event);
            if (key == Term::Key::ArrowUp) {menuList[selectedItem].isSelected = false; selectedItem = (selectedItem - 1 + menuList.size()) % menuList.size(); menuList[selectedItem].isSelected = true;}
            if (key == Term::Key::ArrowDown) {menuList[selectedItem].isSelected = false; selectedItem = (selectedItem + 1) % menuList.size(); menuList[selectedItem].isSelected = true;}
            if (key == Term::Key::ArrowRight && menuList[selectedItem].type == 'l') menuList[selectedItem].changeOption('n');
            if (key == Term::Key::ArrowLeft && menuList[selectedItem].type == 'l') menuList[selectedItem].changeOption('p');
            if (key == Term::Key::Enter && menuList[selectedItem].label == "Start") {
                config.state = 1; 
                if (menuList[1].getValue() == "auto"){
                    config.autoScale = true;
                    config.matrixSize = min(static_cast<int>(screen.rows()) - 2, static_cast<int>(screen.columns()) - 2);
                }else{
                    config.matrixSize = stoi(menuList[1].getValue());
                }
                config.fractalType = menuList[0].currentOption;
                break;
            }
            if (key == Term::Key::Esc) {config.state = 2; break;}
            
            outputStr = "";
            for (int i  = 0; i < menuList.size(); ++i){
                string s(max(0,static_cast<int>((config.columns - menuList[i].getStr().size())/2)), ' ');
                outputStr += s +  menuList[i].getStr() + "\n";
            }
            
            string s(max(0,static_cast<int>((config.rows - split(outputStr, '\n').size())/2)), '\n');

            outputStr = s + outputStr;

            fwrite("\033[2J", 1, 4, stdout);
            fflush(stdout);
            outputStr = "\033[H" + outputStr + "\033[0m";
            fwrite(outputStr.c_str(), 1, outputStr.size(), stdout);
            fflush(stdout);
        }
    }
}

int main(){

    Term::terminal.setOptions(Term::Option::NoClearScreen, Term::Option::NoSignalKeys, Term::Option::Cursor, Term::Option::Raw);

    Term::Screen screen = Term::screen_size();
    appConfig config(screen.rows(), screen.columns());

    while (true)
    {
        if (config.state == 0){
            printMainMenu(config);
        }else if(config.state == 1){
            printFractals(config); 
            config.reset();      
        }else if(config.state == 2){
            break;
        }
    }


    Term::terminal.setOptions(Term::Option::ClearScreen, Term::Option::SignalKeys, Term::Option::Cursor, Term::Option::Cooked);
    return 0;
}
