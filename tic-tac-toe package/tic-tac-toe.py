#!/usr/bin/env python3
import os
import random 
import time
from pynput import keyboard
import sys
from colorama import init, Cursor
import ctypes
import numpy as np

init(autoreset=True)

if sys.platform == 'win32':
    import msvcrt
    kernel32 = ctypes.windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

class cellObject():
    value = ' '
    isSelected = False

logo = r'''
 ,--.--------.  .=-.-.  _,.----.          ,--.--------.   ,---.       _,.----.          ,--.--------.   _,.---._        ,----.  
/==/,  -   , -\/==/_ /.' .' -   \        /==/,  -   , -\.--.'  \    .' .' -   \        /==/,  -   , -\,-.' , -  `.   ,-.--` , \ 
\==\.-.  - ,-./==|, |/==/  ,  ,-'        \==\.-.  - ,-./\==\-/\ \  /==/  ,  ,-'        \==\.-.  - ,-./==/_,  ,  - \ |==|-  _.-` 
 `--`\==\- \  |==|  ||==|-   |  .         `--`\==\- \   /==/-|_\ | |==|-   |  .         `--`\==\- \ |==|   .=.     ||==|   `.-. 
      \==\_ \ |==|- ||==|_   `-' \             \==\_ \  \==\,   - \|==|_   `-' \             \==\_ \|==|_ : ;=:  - /==/_ ,    / 
      |==|- | |==| ,||==|   _  , |             |==|- |  /==/ -   ,||==|   _  , |             |==|- ||==| , '='     |==|    .-'  
      |==|, | |==|- |\==\.       /             |==|, | /==/-  /\ - \==\.       /             |==|, | \==\ -    ,_ /|==|_  ,`-._ 
      /==/ -/ /==/. / `-.`.___.-'              /==/ -/ \==\ _.\=\.-'`-.`.___.-'              /==/ -/  '.='. -   .' /==/ ,     / 
      `--`--` `--`-`                           `--`--`  `--`                                 `--`--`    `--`--''   `--`-----``  
'''

centeredLogo = ''

for i in logo.splitlines():
    centeredLogo += ' ' * int((os.get_terminal_size().columns - len(i))//2) + i + '\n'

gameField = [[cellObject() for _ in range(3)] for _ in range(3)]
npGameField = np.array(gameField)

selectedCell = 0, 0
npGameField[0][0].isSelected = True

def printField(isWin = False, isDraw = False, isLose = False):
    termCols, termRows = os.get_terminal_size()
    resulStr = ''
    for r in range(3):
        row_str = ''
        for c in range(3):
            cell = gameField[r][c]
            char = f'[{cell.value}]' if cell.isSelected else f' {cell.value} '
            row_str += char + ('|' if c < 2 else '')
        resulStr += row_str + '\n'
        if r < 2:
            resulStr += '---' * 3 + '\n'

    centeredResulStr = ''

    for i in resulStr.splitlines():
        centeredResulStr += ' ' * int((termCols - len(i))//2) + i + '\n'

    centeredResulStr = centeredLogo + '\n' + centeredResulStr
    upperSpace = (termRows - len(centeredResulStr.splitlines()))//2
    
    if isWin:
        status_text = 'You win!'
    elif isLose:
        status_text = 'You lose!'
    elif isDraw:
        status_text = "It's a draw!"
    else:
        status_text = None

    if not status_text:
        centeredResulStr = '\n' * upperSpace + centeredResulStr + '\n' * (termRows - upperSpace - len(centeredResulStr.splitlines()) - 1) + '↑/↓/←/→ – move, Enter – enter, R - restart, Ctrl + C – quit'
    else:
        centeredResulStr = '\n' * upperSpace + centeredResulStr + '\n\n' + ' ' * ((termCols - len(status_text))//2) + status_text + '\n' * (termRows - upperSpace - len(centeredResulStr.splitlines()) - 3) + 'Press Ctrl + C to exit or R to restart'
   
    sys.stdout.write(Cursor.POS(1, 1))    
    sys.stdout.write(centeredResulStr)
    sys.stdout.flush()
        
def changeSelection(direction):
    global selectedCell
    cols, rows = npGameField.shape
    
    npGameField[selectedCell[0]][selectedCell[1]].isSelected = False
    
    if direction == 'up':
        selectedCell = (selectedCell[0] - 1) % cols, selectedCell[1]
    elif direction == 'down':
        selectedCell = (selectedCell[0] + 1) % cols, selectedCell[1]
    elif direction == 'right':
        selectedCell = selectedCell[0], (selectedCell[1] + 1) % rows
    elif direction == 'left':
        selectedCell = selectedCell[0], (selectedCell[1] - 1) % rows

    npGameField[selectedCell[0]][selectedCell[1]].isSelected = True

def doTurn():
    emptyCells = [(r, c) for r in range(3) for c in range(3) if npGameField[r][c].value == ' ']

    isClose, cellToWin = checkCloseToWin('O')

    if isClose:
        npGameField[cellToWin[0]][cellToWin[1]].value = 'O'
        return

    isClose, cellToWin = checkCloseToWin('X')

    if isClose:
        npGameField[cellToWin[0]][cellToWin[1]].value = 'O'
        return

    elif npGameField[1][1].value == ' ':
        npGameField[1][1].value = 'O'
        return

    corners = [(0, 0), (0, 2), (2, 0), (2, 2)]

    if np.count_nonzero(npGameField == ' ') >= 1:
        for i in range(4):
            if npGameField[corners[i]] == ' ':
                npGameField[corners[i]].value = 'O'
                return
    else:
        npGameField[random.choice(emptyCells)].value = 'O'
     
def checkCloseToWin(sign):
    for r in range(3):
        values = [cell.value for cell in npGameField[r]]
        if values.count(sign) == 2 and values.count(' ') == 1:
            for c in range(3):
                if npGameField[r][c].value == ' ':
                    return True, (r, c)
    
    for c in range(3):
        values = [cell.value for cell in npGameField[:, c]]
        if values.count(sign) == 2 and values.count(' ') == 1:
            for r in range(3):
                if npGameField[r][c].value == ' ':
                    return True, (r, c)

    diagonal = []

    for r in range(3):
        diagonal.append(npGameField[r][r].value)

    if diagonal.count(sign) == 2 and diagonal.count(' ') == 1:
       for r in range(3):
           if npGameField[r][(r % 3) - 1].value == ' ':
               return True, (r, (r % 3) - 1)

    diagonal = []
    subDiagonalCoord = [(0,2), (1,1), (2,0)]
    for r in subDiagonalCoord:
        diagonal.append(npGameField[r].value)

    if diagonal.count(sign) == 2 and diagonal.count(' ') == 1:
       for r in range(3):
           if npGameField[r][(r % 3) - 1].value == ' ':
               return True, (r, (r % 3) - 1)
    
    return False, None

def checkWin(sign = 'X'):
    for r in range(3):
        if npGameField[r][0].value == npGameField[r][1].value == npGameField[r][2].value == sign:
            return True

    for c in range(3):
        if npGameField[0][c].value == npGameField[1][c].value == npGameField[2][c].value == sign:
            return True

    if npGameField[0][0].value == npGameField[1][1].value == npGameField[2][2].value == sign:
        return True

    if npGameField[0][2].value == npGameField[1][1].value == npGameField[2][0].value == sign:
        return True

    return False

def hasEmptyCells():
    for r in range(3):
        for c in range(3):
            if npGameField[r][c].value == ' ':
                return True
    return False

def restartProcess():
    global selectedCell
    for r in range(3):
        for c in range(3):
            npGameField[r][c].value = ' '
    npGameField[selectedCell[0]][selectedCell[1]].isSelected = False
    selectedCell = 0, 0
    npGameField[0][0].isSelected = True
    sys.stdout.write('\033[2J\033[H')
    sys.stdout.flush()
    printField()

def flush_input():
    try:
        import termios
        termios.tcflush(sys.stdin, termios.TCIFLUSH)
    except ImportError:
        while msvcrt.kbhit():
            msvcrt.getch()


def main():
    global selectedCell
    isGameEnd = False
    try:
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
                            if npGameField[selectedCell[0]][selectedCell[1]].value == ' ':
                                npGameField[selectedCell[0]][selectedCell[1]].value = 'X' 
                                printField()
                                if checkWin('X'):
                                    sys.stdout.write('\033[2J\033[H')
                                    sys.stdout.flush()
                                    printField(isWin=True)
                                    isGameEnd = True
                                
                                elif not hasEmptyCells():
                                    sys.stdout.write('\033[2J\033[H')
                                    sys.stdout.flush()
                                    printField(isDraw=True)
                                    isGameEnd = True
                                else:
                                    doTurn()
                                    printField()
                                    
                                    if checkWin('O'):
                                        sys.stdout.write('\033[2J\033[H')
                                        sys.stdout.flush()
                                        printField(isLose=True)
                                        isGameEnd = True
                                    
                                    elif not hasEmptyCells():
                                        sys.stdout.write('\033[2J\033[H')
                                        sys.stdout.flush()
                                        printField(isDraw=True)
                                        isGameEnd = True

                    if hasattr(key, 'vk') and key.vk == 82:
                        print(key)
                        restartProcess()
                        isGameEnd = False
                    elif hasattr(key, 'value') and key.value == 27:
                        print(key)
                        restartProcess()
                        isGameEnd = False
                    elif key == keyboard.KeyCode.from_char('r'):
                        restartProcess()
                        isGameEnd = False

    except KeyboardInterrupt:
        flush_input()
        os._exit(0)

    except Exception as e:
        flush_input()
        print(e)
        os._exit(1)

main()
