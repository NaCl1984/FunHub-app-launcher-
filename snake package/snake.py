#!/usr/bin/env python3
from collections import deque
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
pause_event = threading.Event()

MIN_TERMINAL_COLS, MIN_TERMINAL_ROWS = 150, 50
SMALL_FIELD_SIZE, MEDIUM_FIELD_SIZE, BIG_FIELD_SIZE, LARGE_FIELD_SIZE = 10, 20, 30, 40
SLOW_SNAKE_SPEED, MODERATE_SNAKE_SPEED, FAST_SNAKE_SPEED = 0.15, 0.1, 0.085

userStatsPath = Path.home() / ".funhub" / "snake" / "userRecords.json"

os.makedirs(os.path.dirname(userStatsPath),  exist_ok=True)

if not os.path.exists(userStatsPath):
    with open(userStatsPath, 'w') as f:
        json.dump({"small":{"slow":0, "moderate":0, "fast":0}, "medium":{"slow":0, "moderate":0, "fast":0}, "big":{"slow":0, "moderate":0, "fast":0}, "large":{"slow":0, "moderate":0, "fast":0}}, f, ensure_ascii=False)

gameField = []
gameFieldSize = 0
snakeSpeed = 0
moveDirection = 'right'
snakeColor = 'red'
snakeColorsDict = {'red':(9, 1, 124),'yellow':(178 ,172 , 166) ,'blue':(12, 4, 20), 'light blue':(39, 33, 27), 'purple':(99, 56, 55)}

gameField = np.array([])

logo = r'''
  ██████  ███▄    █  ▄▄▄       ██ ▄█▀▓█████ 
▒██    ▒  ██ ▀█   █ ▒████▄     ██▄█▒ ▓█   ▀ 
░ ▓██▄   ▓██  ▀█ ██▒▒██  ▀█▄  ▓███▄░ ▒███   
  ▒   ██▒▓██▒  ▐▌██▒░██▄▄▄▄██ ▓██ █▄ ▒▓█  ▄ 
▒██████▒▒▒██░   ▓██░ ▓█   ▓██▒▒██▒ █▄░▒████▒
▒ ▒▓▒ ▒ ░░ ▒░   ▒ ▒  ▒▒   ▓▒█░▒ ▒▒ ▓▒░░ ▒░ ░
░ ░▒  ░ ░░ ░░   ░ ▒░  ▒   ▒▒ ░░ ░▒ ▒░ ░ ░  ░
░  ░  ░     ░   ░ ░   ░   ▒   ░ ░░ ░    ░   
      ░           ░       ░  ░░  ░      ░  ░
                                                                    
''' 

class cellObject():
    def __init__(self):
        self.isApple = False
        self.isSnakeHead = False
        self.isSnakeBody = False
        self.isStone = False
        self.grassPattern = (random.choice(["  ","  ", " #", "# ", "▒▒", "▓▓", "* ", " *", '. ', ' .', '▒▓', '▒ ']))
        self.grassColors = (random.choice([2, 28, 34]), random.choice([70, 64, 10]))

    def changeGrassPattern(self):
        self.grassPattern = (random.choice(["  ", " #", "# ", "▒▒", "▓▓", "* ", " *", '..']))
        self.grassColors = (random.choice([ 2, 34]), random.choice([70, 76, 10]))

    def getStr(self, direction=None):
        snakeHeadColor = snakeColorsDict[snakeColor][0]
        snakeBodyBackgroundColor = snakeColorsDict[snakeColor][1]
        snakeBodyFountColor = snakeColorsDict[snakeColor][2]
        appleBackgroundColor = 88
        appleFontColor = 34
        stoneBacgroundColor = 245
        stoneFontColor = 240
        if self.isSnakeHead and direction:
            match direction:
                case 'up':
                    return f"\033[38;5;0m\033[48;5;{snakeHeadColor}m" + "''" + "\033[0m"
                case 'down':
                    return f"\033[38;5;0m\033[48;5;{snakeHeadColor}m" + ".." + "\033[0m"
                case 'right':
                    return f"\033[38;5;0m\033[48;5;{snakeHeadColor}m" + " :" + "\033[0m"
                case 'left':
                    return f"\033[38;5;0m\033[48;5;{snakeHeadColor}m" + ": " + "\033[0m"
        elif self.isSnakeBody:
            return f"\033[38;5;{snakeBodyFountColor}m\033[48;5;{snakeBodyBackgroundColor}m" + "##" + "\033[0m"
        elif self.isApple:
            return f"\033[38;5;{appleFontColor}m\033[48;5;{appleBackgroundColor}m" + "@/" + "\033[0m"
        elif self.isStone:
            return f"\033[38;5;{stoneFontColor}m\033[48;5;{stoneBacgroundColor}m" + "L " + "\033[0m"
        else:
            return f"\033[38;5;{self.grassColors[1]}m\033[48;5;{self.grassColors[0]}m" + self.grassPattern + "\033[0m"

class snakeObject():
    def __init__(self):
        self.coordinateHistory = deque([])
        self.direction = 'right'
        self.bodyCount = 2

    def setNewSnakePosition(self, gameField):
        if not checkWin(gameFieldSize, self) and not checkDefeat(gameFieldSize, self): 
            if gameField[self.coordinateHistory[0]].isApple:
                self.bodyCount += 1 
                gameField[self.coordinateHistory[0]].isApple = False
                placeApple(gameField)

            gameField[self.coordinateHistory[0]].isSnakeHead = True
            gameField[self.coordinateHistory[1]].isSnakeHead = False
            gameField[self.coordinateHistory[1]].isSnakeBody = True

            if self.bodyCount < len(self.coordinateHistory) - 1:
                gameField[self.coordinateHistory.pop()].isSnakeBody = False

    def move(self, gameField):
        y, x = self.coordinateHistory[0]
        match self.direction:
            case 'up':
                self.coordinateHistory.appendleft((y - 1, x))  
                self.setNewSnakePosition(gameField)
            
            case 'down':
                self.coordinateHistory.appendleft((y + 1, x))
                self.setNewSnakePosition(gameField)
            
            case 'right':
                self.coordinateHistory.appendleft((y, x + 1))
                self.setNewSnakePosition(gameField)
            
            case 'left':
                self.coordinateHistory.appendleft((y, x - 1))
                self.setNewSnakePosition(gameField)

    def changeDirection(self, newDirection):
        turnAroundPositions = {'up':'down', 'down':'up', 'left':'right', 'right':'left'}
        if newDirection != turnAroundPositions[self.direction]:
            self.direction = newDirection

def printField(snake, isWin = False, isDefeat = False, isPaused=False):
    with print_lock:
        global gameFieldSize, logo
        termCols, termRows = os.get_terminal_size()
        resulStr = ''
        newlogo = logo if gameFieldSize < 40 else 'snake'
        centeredLogo = ''

        for i in newlogo.splitlines():
            centeredLogo += ' ' * int((os.get_terminal_size().columns - len(i))//2) + i + '\n'

        countApples = snake.bodyCount - 2

        counter = 'Apples eat: ' + str(countApples)
        counter = ' ' * int((os.get_terminal_size().columns - len(counter))//2) + counter + '\n'

        for r in range(gameFieldSize):
            row_str = ''
            for c in range(gameFieldSize):
                cell = gameField[r][c]
                char = cell.getStr() if not cell.isSnakeHead else cell.getStr(snake.direction)
                row_str += char 
            resulStr += row_str + '\n'

        centeredResulStr = ''

        for i in resulStr.splitlines():
            centeredResulStr += ' ' * int((termCols - (gameFieldSize * 2))//2) + i + '\n'

        centeredResulStr = centeredLogo + '\n' + counter + '\n' + centeredResulStr
        upperSpace = (termRows - len(centeredResulStr.splitlines()))//2 - 1

        endGameText = ''
        if isWin:
            endGameText = 'You win!'
        elif isDefeat:
            endGameText = 'You lose!'
        elif isPaused:
            endGameText = 'Game is paused...'
        
        if not endGameText:
            centeredResulStr = '\n' * upperSpace + centeredResulStr + '\n' * (termRows - upperSpace - len(centeredResulStr.splitlines()) - 1) + '↑/↓/←/→ – move, ESC - pause, R - restart, Ctrl + C – quit'
        else:
            endGameText = ' ' * int((termCols - len(endGameText))//2) + endGameText
            centeredResulStr = '\n' * upperSpace + centeredResulStr + '\n\n' + endGameText +'\n' * (termRows - upperSpace - len(centeredResulStr.splitlines()) - 3) + '↑/↓/←/→ – move, ESC - pause, R - restart, Ctrl + C – quit'
    

        sys.stdout.write(Cursor.POS(1, 1))    
        sys.stdout.write(centeredResulStr)
        sys.stdout.flush()

def settingsMenu():
    global gameFieldSize, snakeSpeed, gameField, snakeColor
    try:
        prew_cols, prew_rows = 0, 0
        selectedSetting = 0
        
        with open(userStatsPath, 'r', encoding='utf-8') as f:
            userStats = json.load(f)

        class settingsObject():
            def __init__(self, name, content, selectedItem):
                self.isSelected = False
                self.content = content
                self.selectedItem = selectedItem % len(self.content)
                self.name = name

            def getStr(self):
                strSize = 30
                if self.isSelected:
                    name =  "*" + self.name 
                    settings = (" < " if self.selectedItem != 0 else "   ") + self.content[self.selectedItem] + (" >" if self.selectedItem < len(self.content) - 1 else "  ") 
                    str =  name + ' ' * ((strSize - len(name) - len(settings))//2) + settings
                    return str
                else:
                    name =  " " + self.name 
                    settings = (" < " if self.selectedItem != 0 else "   ") + self.content[self.selectedItem] + (" >" if self.selectedItem < len(self.content) - 1 else "  ")
                    str =  name + ' ' * ((strSize - len(name) - len(settings))//2) + settings
                    return str
                
            def changeSelectedItem(self, direction):
                if direction == 'next':
                    self.selectedItem = (self.selectedItem + 1) % len(self.content)
                elif direction == 'prev': 
                    self.selectedItem = (self.selectedItem - 1) % len(self.content)

        settingsMenu = [settingsObject('Field size', ["small", 'medium', 'big', 'large'], 1), settingsObject('Snake speed', ['slow', 'moderate', 'fast'], 1), settingsObject('Snake color', list(snakeColorsDict.keys()), 0),"Start"]

        def printMenu(changeSelection=None):
            menuToPrint = ''
            termCols, termRows = os.get_terminal_size()
            centeredStr = ''
            nonlocal prew_cols, prew_rows, settingsMenu, selectedSetting
            
            if termCols != prew_cols or termRows != prew_rows:
                prew_cols, prew_rows = termCols, termRows
                sys.stdout.write('\033[2J\033[H')
                sys.stdout.flush()

            if isinstance(settingsMenu[selectedSetting], settingsObject):
                settingsMenu[selectedSetting].isSelected = False

            if changeSelection:
                if changeSelection == 'up':
                    selectedSetting = (selectedSetting - 1) % len(settingsMenu)
                elif changeSelection == 'down':
                    selectedSetting = (selectedSetting + 1) % len(settingsMenu)

            if isinstance(settingsMenu[selectedSetting], settingsObject):
                settingsMenu[selectedSetting].isSelected = True 

            for i in logo.splitlines():
                centeredStr += ' ' * int((termCols - len(i))//2) + i + '\n'

            menuToPrint += centeredStr
            
            record = "Your record: " + str( userStats[settingsMenu[0].content[settingsMenu[0].selectedItem]][settingsMenu[1].content[settingsMenu[1].selectedItem]])

            menuToPrint += ' ' * int((termCols - len(record))//2) + record + '\n' + '\n'

            for i in settingsMenu:
                if isinstance(i, settingsObject):
                    if i.name != "Snake color":
                        menuToPrint += ' ' * int((termCols - len(i.getStr()))//2) + i.getStr() + '\n'
                    else:
                        menuToPrint += ' ' * int((termCols - len(i.getStr()))//2) + i.getStr() + " " +  (f"\033[38;5;{snakeColorsDict[i.content[i.selectedItem]][2]}m\033[48;5;{snakeColorsDict[i.content[i.selectedItem]][1]}m" + "##" + "\033[0m" + f"\033[38;5;0m\033[48;5;{snakeColorsDict[i.content[i.selectedItem]][0]}m" + " :" + "\033[0m") + '\n'
                else:
                    if selectedSetting != len(settingsMenu) - 1:
                        menuToPrint += '\n' + ' ' * int((termCols - len(i))//2) + ' ' + i + '\n'
                    else:
                        menuToPrint += '\n' + ' ' * int((termCols - len(i))//2) + '*' + i + '\n'

            upperSpace = (termRows - len(menuToPrint.splitlines()))//2

            menuToPrint = '\n' * upperSpace + menuToPrint + '\n' * (termRows - upperSpace - len(menuToPrint.splitlines()) - 1) + '↑/↓/←/→ – move, Enter – select, Ctrl + C – quit'

            sys.stdout.write('\033[2J\033[H')
            sys.stdout.flush()
            sys.stdout.write(Cursor.POS(1, 1))    
            sys.stdout.write(menuToPrint)
            sys.stdout.flush()         

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
                        printMenu(changeSelection='up')
                    
                    elif key == keyboard.Key.down:
                        printMenu(changeSelection='down')  
                    
                    elif key == keyboard.Key.right:
                        if isinstance(settingsMenu[selectedSetting], settingsObject):
                            settingsMenu[selectedSetting].changeSelectedItem('next')
                        printMenu()
                    
                    elif key == keyboard.Key.left:
                        if isinstance(settingsMenu[selectedSetting], settingsObject):
                            settingsMenu[selectedSetting].changeSelectedItem('prev')
                        printMenu()
                    
                    elif key == keyboard.Key.enter:
                        if not isinstance(settingsMenu[selectedSetting], settingsObject):
                            match settingsMenu[0].content[settingsMenu[0].selectedItem]:
                                case 'small':
                                    gameFieldSize = SMALL_FIELD_SIZE
                                case 'medium':
                                    gameFieldSize = MEDIUM_FIELD_SIZE
                                case 'big':
                                    gameFieldSize = BIG_FIELD_SIZE
                                case 'large':
                                    gameFieldSize = LARGE_FIELD_SIZE
                                
                            match settingsMenu[1].content[settingsMenu[1].selectedItem]:
                                case 'slow':
                                    snakeSpeed = SLOW_SNAKE_SPEED
                                case 'moderate':
                                    snakeSpeed = MODERATE_SNAKE_SPEED
                                case 'fast':
                                    snakeSpeed = FAST_SNAKE_SPEED

                            snakeColor = settingsMenu[2].content[settingsMenu[2].selectedItem]

                            gameField = np.array([[ cellObject() for _ in range(gameFieldSize)]for _ in range(gameFieldSize)])
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

def checkDefeat(gameFieldSize, snake):
    y, x = snake.coordinateHistory[0]
    
    if y >= gameFieldSize or y < 0:
        return True
    elif x >= gameFieldSize or x < 0:
        return True
    
    for i in list(snake.coordinateHistory)[1:]:
        if (y,x) == i:
            return True

    return False
    
def checkWin(gameFieldSize, snake):
    if snake.bodyCount + 1 == gameFieldSize ** 2 :
        return True
    return False

def restartProcess(snake):
    global gameField, moveDirection
    for j in gameField:
        for i in j:
            i.isApple = False
            i.isSnakeHead = False
            i.isSnakeBody = False
            i.isStone = False
    moveDirection = 'right'
    sys.stdout.write('\033[2J\033[H')
    sys.stdout.flush()
    printField(snake)

def writeNewRecord(record):
    with open(userStatsPath, 'r') as f:
        recordJson = json.load(f)
    
    snakeSpeedStr = ''
    fieldSizeStr = ''

    if snakeSpeed == SLOW_SNAKE_SPEED:
        snakeSpeedStr = 'slow'
    elif snakeSpeed == MODERATE_SNAKE_SPEED:
        snakeSpeedStr = 'moderate'
    elif snakeSpeed == FAST_SNAKE_SPEED:
        snakeSpeedStr = 'fast'

    if gameFieldSize == SMALL_FIELD_SIZE:
        fieldSizeStr = 'small'
    elif gameFieldSize == MEDIUM_FIELD_SIZE:
        fieldSizeStr = 'medium'
    elif gameFieldSize == BIG_FIELD_SIZE:
        fieldSizeStr = 'big'
    elif gameFieldSize == LARGE_FIELD_SIZE:
        fieldSizeStr = 'large'

    if recordJson[fieldSizeStr][snakeSpeedStr] < record:
        recordJson[fieldSizeStr][snakeSpeedStr] = record
        with open(userStatsPath, 'w', encoding='utf-8') as f:
            json.dump(recordJson, f, ensure_ascii=False)

def updateScene(snake):
    global gameFieldSize, moveDirection, gameField
    
    while not stop_event.is_set():
        while pause_event.is_set():
            time.sleep(0.1)

        if snake.direction != moveDirection:
            snake.changeDirection(moveDirection)
        
        if checkDefeat(gameFieldSize, snake):
            stop_event.set()
            printField(snake, isDefeat=True)
            writeNewRecord(snake.bodyCount - 2)
        
        elif checkWin(gameFieldSize, snake):
            stop_event.set()
            printField(snake, isWin=True)
            writeNewRecord(snake.bodyCount - 2)
        else:
            snake.move(gameField)
            printField(snake)

        time.sleep(snakeSpeed)

def flush_input():
    try:
        import termios
        termios.tcflush(sys.stdin, termios.TCIFLUSH)
    except ImportError:
        while msvcrt.kbhit():
            msvcrt.getch()

def startGame(gameField, snake):
    gameField[gameFieldSize//2, gameFieldSize//2 + 2].isApple = True
    gameField[gameFieldSize//2, gameFieldSize//2 - 2].isSnakeHead = True
    gameField[gameFieldSize//2, gameFieldSize//2 - 3].isSnakeBody = True
    gameField[gameFieldSize//2, gameFieldSize//2 - 4].isSnakeBody = True
    snake.coordinateHistory.clear()
    snake.bodyCount = 2
    snake.coordinateHistory.append((gameFieldSize//2, gameFieldSize//2 - 2))
    snake.coordinateHistory.append((gameFieldSize//2, gameFieldSize//2 - 3))
    snake.coordinateHistory.append((gameFieldSize//2, gameFieldSize//2 - 4))
    freePlaces = []
    for i in range(gameFieldSize):
        for j in range(gameFieldSize):
            gameField[i, j].changeGrassPattern()
            if not gameField[i, j].isSnakeHead and not gameField[i, j].isSnakeBody and not gameField[i, j].isApple:
                freePlaces.append((i, j))

    random.shuffle(freePlaces)

    for i in range(random.randint(gameFieldSize//4, gameFieldSize//2)):
        gameField[freePlaces[i]].isStone = True    

def placeApple(gameField):
    global gameFieldSize
    freePlaces = []
    for i in range(gameFieldSize):
        for j in range(gameFieldSize):
            if not gameField[i, j].isSnakeHead and not gameField[i, j].isSnakeBody:
                freePlaces.append((i, j))
    
    random.shuffle(freePlaces)

    if freePlaces:
        gameField[freePlaces[0]].isApple = True

def main():
    global gameField, moveDirection
    moves = 0
    snake = snakeObject()

    try:
        while True:
            updateSceneThread = threading.Thread(target=updateScene, args=(snake,), daemon=True)
            try:
                sys.stdout.write('\033[2J\033[H')
                sys.stdout.flush()
                checkTerminalSize()
                settingsMenu()
                startGame(gameField, snake)
                stop_event.clear()
                sys.stdout.write('\033[2J\033[H')
                sys.stdout.flush()
                printField(snake)
                with keyboard.Events() as events:
                    while True:
                        event = events.get(1.0)
                        if event is None:
                            continue

                        if isinstance(event, keyboard.Events.Press):
                            key = event.key

                            if not stop_event.is_set():
                                if key == keyboard.Key.up:
                                    if moves == 0:                                       
                                        updateSceneThread.start()
                                    moves += 1
                                    moveDirection = 'up'

                                elif key == keyboard.Key.down:
                                    if moves == 0:
                                        updateSceneThread.start()
                                    moves += 1
                                    moveDirection = 'down'

                                elif key == keyboard.Key.right:
                                    if moves == 0:
                                        updateSceneThread.start()
                                    moves += 1
                                    moveDirection = 'right'

                                elif key == keyboard.Key.left:
                                    if moves == 0:
                                        updateSceneThread.start()
                                    moves += 1
                                    moveDirection = 'left'
                                
                                elif key == keyboard.Key.esc :
                                    if not pause_event.is_set():
                                        pause_event.set()
                                        printField(snake, isPaused=True)
                                    else:
                                        pause_event.clear()
                                        sys.stdout.write('\033[2J\033[H')
                                        sys.stdout.flush()

                            if ((hasattr(key, 'vk') and key.vk == 82) or key == keyboard.KeyCode.from_char('r') or (hasattr(key, 'char') and key.char == 'r')) and stop_event.is_set():
                                moves = 0
                                updateSceneThread.join()
                                updateSceneThread = threading.Thread(target=updateScene, args=(snake,), daemon=True)
                                stop_event.clear()
                                pause_event.clear()
                                snake.direction = 'right'
                                restartProcess(snake)
                                startGame(gameField, snake)
                                printField(snake)

            except KeyboardInterrupt:
                stop_event.set()
                if moves != 0:
                    updateSceneThread.join()
                moves = 0
                pause_event.clear()
                snake.direction = 'right'
                moveDirection = 'right'
                sys.stdout.write('\033[2J\033[H')
                sys.stdout.flush()
                continue

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