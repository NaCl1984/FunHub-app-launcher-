#!/usr/bin/env python3
import json
import os
from pathlib import Path
import random 
import shutil
import threading
import time
from pynput import keyboard
import sys
from colorama import init, Cursor, Fore, Back, Style
import ctypes
import numpy as np

init(autoreset=True)

if sys.platform == 'win32':
    import msvcrt
    kernel32 = ctypes.windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)
else:
    import termios
    import tty
    fd = sys.stdin.fileno()
    oldSettings = termios.tcgetattr(fd)
    tty.setcbreak(fd)

stop_event = threading.Event()
print_lock = threading.Lock()

MIN_TERMINAL_COLS, MIN_TERMINAL_ROWS = 150, 60
EASY_FIELD_SIZE, MEDIUM_FIELD_SIZE, HARD_FIELD_SIZE, VERY_HARD_FIELD_SIZE = 9, 16, 30, 49

backgroundColor = 250
numbersColors = [12, 10, 9, 4, 94, 14, 0,15]

userStatsPath = Path.home() / ".funhub" / "minesweeper" / "userStats.json"

os.makedirs(os.path.dirname(userStatsPath),  exist_ok=True)

if not os.path.exists(userStatsPath):
    with open(userStatsPath, 'w') as f:
        json.dump({"easy":0, "medium":0, "hard":0, "very hard":0}, f, ensure_ascii=False, indent=4)

gameField = []
gameFieldSize = 0
mines = 0
selectedCell = 0, 0

gameField = np.array([])

logo = r'''
  __  __ _                                          
 |  \/  (_)_ _  ___ ____ __ _____ ___ _ __  ___ _ _ 
 | |\/| | | ' \/ -_|_-< V  V / -_) -_) '_ \/ -_) '_|
 |_|  |_|_|_||_\___/__/\_/\_/\___\___| .__/\___|_|  
                                     |_|            
                                     
''' 

class cellObject():
    def __init__(self):
        self.isMine = False
        self.isFlag = False
        self.isOpen = False
        self.isSelected = False
        self.neighborMines = 0

    def getStr(self):
        if not self.isOpen and not self.isFlag:
            return f"\x1b[38;5;{backgroundColor};48;5;{254}m{' # ' if not self.isSelected else '[#]'}\x1b[0m" 
        elif not self.isOpen and self.isFlag:
            return f"\x1b[38;5;{11};48;5;{backgroundColor}m{' F ' if not self.isSelected else '[F]'}\x1b[0m" 
        elif self.isOpen and not self.isMine and self.neighborMines > 0:
            return f"\x1b[38;5;{numbersColors[self.neighborMines - 1]};48;5;{backgroundColor}m{f' {self.neighborMines} ' if not self.isSelected else f'[{self.neighborMines}]'}\x1b[0m" 
        elif self.isOpen and not self.isMine and self.neighborMines == 0:
            return f"\x1b[38;5;{0};48;5;{backgroundColor}m{'   ' if not self.isSelected else '[ ]'}\x1b[0m" 
        elif self.isOpen and self.isMine:
            return f"\x1b[38;5;{0};48;5;{9}m{' * ' if not self.isSelected else '[*]'}\x1b[0m" 

    def openCell(self):
        if not self.isOpen and not self.isFlag:
            self.isOpen = True
            return True
        return False
    
    def placeFlag(self):
        if not self.isOpen:
            if self.isFlag:
                self.isFlag = False
            else:
                self.isFlag = True

def printField(startedTime=0, isWin = False, isDefeat = False):
    with print_lock:
        global gameFieldSize, logo, mines
        termCols, termRows = os.get_terminal_size()
        resulStr = ''
        logo = logo if gameFieldSize < 40 else 'Minesweeper'
        centeredLogo = ''

        for i in logo.splitlines():
            centeredLogo += ' ' * int((os.get_terminal_size().columns - len(i))//2) + i + '\n'

        for r in range(gameFieldSize):
            row_str = ''
            for c in range(gameFieldSize):
                cell = gameField[r][c]
                char = cell.getStr()
                row_str += char 
            resulStr += row_str + '\n'

        centeredResulStr = ''

        for i in resulStr.splitlines():
            centeredResulStr += ' ' * int((termCols - (gameFieldSize * 3))//2) + i + '\n'

        centeredResulStr = centeredLogo + '\n' + centeredResulStr
        upperSpace = (termRows - len(centeredResulStr.splitlines()))//2
        
        info = "time: " + getTime(startedTime, getStr=True) + ' ' + f'mines:{mines}'
        info = " " * ((os.get_terminal_size().columns - len(info))//2) + info
        centeredResulStr += '\n' + info 

        endGameText = ''
        if isWin:
            endGameText = 'You win!'
        elif isDefeat:
            endGameText = 'You lose!'
        
        if not endGameText:
            centeredResulStr = '\n' * upperSpace + centeredResulStr + '\n' * (termRows - upperSpace - len(centeredResulStr.splitlines()) - 1) + '↑/↓/←/→ – move, Enter – enter, R - restart, F - place flag, Ctrl + C – quit'
        else:
            endGameText = ' ' * int((termCols - len(endGameText))//2) + endGameText
            centeredResulStr = '\n' * upperSpace + centeredResulStr + '\n\n' + endGameText +'\n' * (termRows - upperSpace - len(centeredResulStr.splitlines()) - 3) + '↑/↓/←/→ – move, Enter – enter, R - restart, F - place flag, Ctrl + C – quit'
    

        sys.stdout.write(Cursor.POS(1, 1))    
        sys.stdout.write(centeredResulStr)
        sys.stdout.flush()
        
def changeSelection(direction):
    global selectedCell
    cols, rows = gameField.shape
    
    gameField[selectedCell[0]][selectedCell[1]].isSelected = False
    
    if direction == 'up':
        selectedCell = (selectedCell[0] - 1) % cols, selectedCell[1]
    elif direction == 'down':
        selectedCell = (selectedCell[0] + 1) % cols, selectedCell[1]
    elif direction == 'right':
        selectedCell = selectedCell[0], (selectedCell[1] + 1) % rows
    elif direction == 'left':
        selectedCell = selectedCell[0], (selectedCell[1] - 1) % rows

    gameField[selectedCell[0]][selectedCell[1]].isSelected = True

def selectDificulti():
    global gameField, gameFieldSize, mines, selectedCell
    try:
        difilculties = ['Easy', 'Medium', 'Hard', 'Very hard']
        selectedDiff = 0
        prew_cols, prew_rows = 0, 0

        with open(userStatsPath, 'r', encoding='utf-8') as f:
            userStats = json.load(f)

        def printMenu():
            menuToPrint = ''
            termCols, termRows = os.get_terminal_size()
            centeredStr = ''
            nonlocal prew_cols, prew_rows, selectedDiff
            
            if termCols != prew_cols or termRows != prew_rows:
                prew_cols, prew_rows = termCols, termRows
                sys.stdout.write('\033[2J\033[H')
                sys.stdout.flush()

            for i in logo.splitlines():
                centeredStr += ' ' * int((termCols - len(i))//2) + i + '\n'

            menuToPrint += centeredStr
            
            difList = ''
            for i in difilculties:
                if i == difilculties[selectedDiff]:
                    difList += f'*{i}' + (" - " + timeToStr(userStats[i.lower()]) if userStats[i.lower()] != 0 else '') + '\n'
                else:
                    difList += f' {i}' + (" - " + timeToStr(userStats[i.lower()]) if userStats[i.lower()] != 0 else '') + '\n'
            
            centeredStr = ''
            
            for i in difList.splitlines():
                centeredStr += ' ' * int((termCols - len(i))//2) + i + '\n'

            menuToPrint += centeredStr

            upperSpace = (termRows - len(menuToPrint.splitlines()))//2

            menuToPrint = '\n' * upperSpace + menuToPrint + '\n' * (termRows - upperSpace - len(menuToPrint.splitlines()) - 1) + '↑/↓/ – move, Enter – select, Ctrl + C – quit'

            sys.stdout.write(Cursor.POS(1, 1))    
            sys.stdout.write(menuToPrint)
            sys.stdout.flush()

        def setDif():
            global gameField, gameFieldSize, mines, gameField
            gameField = [[cellObject() for _ in range(gameFieldSize)] for _ in range(gameFieldSize)]
            gameField = np.array(gameField)
            

        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()
        printMenu()
        with keyboard.Events() as events:
            while True:
                
                event = events.get(1.0)
                if event is None:
                    continue
                
                if isinstance(event, keyboard.Events.Press):
                    key = event.key
                    if key == keyboard.Key.up:
                        selectedDiff = (selectedDiff - 1) % len(difilculties)
                        printMenu()
                    elif key == keyboard.Key.down:
                        selectedDiff = (selectedDiff + 1) % len(difilculties)
                        printMenu()    
                    elif key == keyboard.Key.enter:
                        if difilculties[selectedDiff] == 'Easy':
                            gameFieldSize = EASY_FIELD_SIZE
                            mines = 10
                        elif difilculties[selectedDiff] == 'Medium':
                            gameFieldSize = MEDIUM_FIELD_SIZE
                            mines = 40
                        elif difilculties[selectedDiff] == 'Hard':
                            gameFieldSize = HARD_FIELD_SIZE
                            mines = 99
                        elif difilculties[selectedDiff] == 'Very hard':
                            gameFieldSize = VERY_HARD_FIELD_SIZE
                            mines = 150
                        setDif()
                        gameField[gameFieldSize//2][gameFieldSize//2].isSelected = True
                        selectedCell = gameFieldSize//2, gameFieldSize//2
                        return True

    except KeyboardInterrupt:
        os._exit(0)
    except Exception as e:
        print(e)
        os._exit(1)

def forceTerminalSize(cols, rows):
    if sys.platform == 'win32':
        os.system(f'mode con: cols={cols} lines={rows}')
        
        kernel32 = ctypes.windll.kernel32
        hOut = kernel32.GetStdHandle(-11)
        res = ctypes.wintypes._COORD(cols, rows)
        kernel32.SetConsoleScreenBufferSize(hOut, res)
    else:
        sys.stdout.write(f"\x1b[8;{rows};{cols}t")
        sys.stdout.flush()

def checkTerminalSize():
    cols, rows = shutil.get_terminal_size()
    if cols < MIN_TERMINAL_COLS or rows < MIN_TERMINAL_ROWS:
        forceTerminalSize(MIN_TERMINAL_COLS, MIN_TERMINAL_ROWS)
        cols, rows = shutil.get_terminal_size()
        if cols < MIN_TERMINAL_COLS or rows < MIN_TERMINAL_ROWS:
            return False
    return True

def placeMines():
    global gameField, mines, gameFieldSize, selectedCell
    placedMines = []
    coordinates = [(y, x) for y in range(gameFieldSize) for x in range(gameFieldSize)]
    random.shuffle(coordinates)
    for i in range(mines):
        mine = coordinates[i] if coordinates[i] != selectedCell and coordinates[i] not in placedMines else coordinates[mines + i]
        gameField[mine].isMine = True
        placedMines.append(mine)
    
def checkDefeat():
    global gameField
    for i in gameField:
        for j in i:
            if j.isOpen and j.isMine:
                return True
    return False        
    
def checkWin():
    global gameField, mines
    openedCell = 0
    for i in gameField:
        for j in i:
            if j.isOpen and j.isMine:
                return False
            if j.isOpen:
                openedCell += 1
    if openedCell >= (gameField.size - mines):
        return True
    return False

def checkNeighbors(tuplePlace):
    global gameField, gameFieldSize
    y, x = tuplePlace
    curCell = gameField[y, x]
    neighborsCells = [(y-1, x), (y-1, x+1), (y, x+1), (y+1, x+1), (y+1, x), (y+1, x-1), (y, x-1), (y-1, x-1)]
    flagedMines = 0

    if not curCell.isMine:
        if curCell.neighborMines == 0:
            for i in neighborsCells:
                if (i[0] >= 0 and i[1] >= 0) and (i[0] < gameFieldSize and i[1] < gameFieldSize):
                    if gameField[i].isMine:
                        curCell.neighborMines += 1

        for i in neighborsCells:
            if (i[0] >= 0 and i[1] >= 0) and (i[0] < gameFieldSize and i[1] < gameFieldSize):
                if gameField[i].isMine:
                    if gameField[i].isFlag:
                        flagedMines += 1
        
        if curCell.neighborMines == 0:
            for i in neighborsCells:
                if (i[0] >= 0 and i[1] >= 0) and (i[0] < gameFieldSize and i[1] < gameFieldSize):
                    if not gameField[i].isMine:
                        if gameField[i].openCell() and gameField[i].neighborMines == 0 :
                            checkNeighbors(i)
    
        if curCell.neighborMines == flagedMines:
            for i in neighborsCells:
                if (i[0] >= 0 and i[1] >= 0) and (i[0] < gameFieldSize and i[1] < gameFieldSize):
                    if gameField[i].openCell():
                        y, x = i
                        neighborsCells = [(y-1, x), (y-1, x+1), (y, x+1), (y+1, x+1), (y+1, x), (y+1, x-1), (y, x-1), (y-1, x-1)]
                        for j in neighborsCells:
                            if (j[0] >= 0 and j[1] >= 0) and (j[0] < gameFieldSize and j[1] < gameFieldSize):
                                if gameField[j].isMine:
                                    gameField[i].neighborMines += 1
                        printField()

                        

def restartProcess():
    global gameField, selectedCell
    for j in gameField:
        for i in j:
            i.isMine = False
            i.isFlag = False
            i.isOpen = False
            i.isSelected = False
            i.neighborMines = 0
    sys.stdout.write('\033[2J\033[H')
    sys.stdout.flush()
    gameField[selectedCell].isSelected = False
    selectedCell = gameFieldSize//2, gameFieldSize//2
    gameField[selectedCell].isSelected = True
    printField()

def getTime(gettedTime, getStr=False):
    if gettedTime != 0:
        if not getStr:
            return time.monotonic() - gettedTime
        else:
            return timeToStr(time.monotonic() - gettedTime)
    else:
        return '00:00'

def timeToStr(gettedTime):
    minuts = int((gettedTime % 3600) // 60)
    seconds = int(gettedTime % 60)
    return f'{minuts:02d}:{seconds:02d}'

def updateView(gameTime):
    while not stop_event.is_set():
        printField(startedTime=gameTime)
        time.sleep(0.1)

def flush_input():
    try:
        import termios
        termios.tcflush(sys.stdin, termios.TCIFLUSH)
    except ImportError:
        while msvcrt.kbhit():
            msvcrt.getch()

def main():
    global selectedCell, gameField
    moves = 0
    isGameEnd = False
    timeStarted = 0
    try:
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()
        checkTerminalSize()
        selectDificulti()
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()
        printField()
        with keyboard.Events() as events:
            while True:
                event = events.get(1.0)
                if event is None:
                    continue

                if isinstance(event, keyboard.Events.Press):
                    key = event.key

                    if not isGameEnd:
                        if key == keyboard.Key.up:
                            changeSelection('up')
                            printField()
                        elif key == keyboard.Key.down:
                            changeSelection('down')
                            printField()
                        elif key == keyboard.Key.right:
                            changeSelection('right')
                            printField()
                        elif key == keyboard.Key.left:
                            changeSelection('left')
                            printField()
                        
                        elif key == keyboard.Key.enter:
                            if moves <= 0:
                                placeMines()
                                timeStarted = time.monotonic()
                                timerUpdateThread = threading.Thread(target=updateView, args=(timeStarted,), daemon=True)
                                stop_event.clear()
                                timerUpdateThread.start()
                            
                            moves += 1
                            gameField[selectedCell].openCell()
                            checkNeighbors(selectedCell)

                            if checkDefeat():
                                printField(isDefeat=True)
                                isGameEnd = True
                                stop_event.set()
                            
                            elif checkWin():
                                printField(isWin=True, startedTime=timeStarted)
                                with open(userStatsPath, 'r', encoding='utf-8') as f:
                                    records = json.load(f)
                                
                                newTime = getTime(timeStarted)
                                if gameFieldSize == EASY_FIELD_SIZE:
                                    records['easy'] = (newTime if records['easy'] > newTime else records['easy']) if records['easy'] != 0 else newTime
                                elif gameFieldSize == MEDIUM_FIELD_SIZE:
                                    records['medium'] = (newTime if records['medium'] > newTime else records['medium']) if records['medium'] != 0 else newTime
                                elif gameFieldSize == HARD_FIELD_SIZE:
                                    records['hard'] = (newTime if records['hard'] > newTime else records['hard']) if records['hard'] != 0 else newTime
                                elif gameFieldSize == VERY_HARD_FIELD_SIZE:
                                    records['very hard'] = (newTime if records['very hard'] > newTime else records['very hard']) if records['very hard'] != 0 else newTime

                                with open(userStatsPath, 'w') as f:
                                    json.dump(records, f, ensure_ascii=False, indent=4)

                                isGameEnd = True
                                stop_event.set()
                        
                    if (hasattr(key, 'vk') and key.vk == 82) or key == keyboard.KeyCode.from_char('r') or (hasattr(key, 'char') and key.char == 'r'):
                        restartProcess()
                        timeStarted = 0
                        moves = 0
                        isGameEnd = False
                        stop_event.set()
                        printField()

                    elif (hasattr(key, 'vk') and key.vk == 70) or  key == keyboard.KeyCode.from_char('f') or (hasattr(key, 'char') and key.char == 'f'):
                        gameField[selectedCell].placeFlag()
                            
    except KeyboardInterrupt:
        flush_input()
        os._exit(0)

    except Exception as e:
        flush_input()
        print(e)
        os._exit(1)
    finally:
        if sys.platform != 'win32':
            termios.tcsetattr(fd,termios.TCSADRAIN, oldSettings)
        flush_input()

if __name__ == "__main__":
    main()