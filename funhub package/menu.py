import os
import shutil
import time
from pynput import keyboard
import sys
from colorama import init, Cursor
import ctypes
import subprocess
import json
import warnings
import requests
import importlib.util
import threading
from pathlib import Path
import numpy as np
from importlib.metadata import version
from packaging.version import parse

warnings.filterwarnings('ignore')

init(autoreset=True)
oldSettings  = 0

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

MIN_TERMINAL_COLS, MIN_TERMINAL_ROWS = 150, 50

prew_term_cols = 0
prew_term_rows = 0

GITHUB_JSON_URL = f"https://raw.githubusercontent.com/NaCl1984/FunHub-tech-page/refs/heads/main/plugins.json?t={int(time.time())}"

LAUNCHER_PACKAGER_NAME = 'funhub-app-launcher'

app_dir = Path.home() / ".funhub"
app_dir.mkdir(exist_ok=True)

LOCAL_SYSTEM_JSON_PATH = app_dir / "plugins.json"

USER_JSON_PATH = app_dir / "usersPlugins.json"

ROWS_PER_COLUMN = 10
COLUMNS_PER_PAGE = 3

isloadingEvent =  threading.Event()

class menuObject():
    def __init__ (self, isSelected, data):
        self.isSelected = isSelected
        self.type = data['type']
        self.name = data['name']
        
        if self.type =='local':
            self.pluginPath = data['pluginPath']
        elif self.type == 'remote':
            self.packageName = data['packageName']
            self.commandName = data['commandName']
        

    def getName(self):
        if self.type != 'empty':
            return f'*{self.name}' if self.isSelected else f' {self.name}'
        return " "
    
    def subrocessRun(self, executable):
        try:
            subprocess.run([executable])
        except KeyboardInterrupt:
            pass

    def run(self):
        if self.type == 'local':
            if os.path.exists(self.pluginPath):
                subprocess.run([sys.executable, self.pluginPath])
            else:
                systemMessage(f'Plugin "{self.name}" not found', [buttonObject('Ok', lambda: None)])
        else:
            try:
                installedVersion = version(self.packageName)
            except:
                installedVersion = None
            url = f"https://pypi.org/pypi/{self.packageName}/json"
            response = requests.get(url, timeout=5)
            response.raise_for_status()
            latestVersion = response.json()["info"]["version"]
            executable = shutil.which(self.commandName)
    
            if executable and not parse(latestVersion) > parse(installedVersion):
                self.subrocessRun(executable)
            elif executable and parse(latestVersion) > parse(installedVersion):
                systemMessage(f'Plugin "{self.name}" not updated to latest vesrion. Update to latest version?',[buttonObject('Cancel', lambda: None), buttonObject('Update', lambda: updatePackage(self.packageName)), buttonObject('Launch with out update', lambda: self.subrocessRun(executable))])
            else:
                systemMessage(f'Plugin "{self.name}" not installed yet. Install it now?',[buttonObject('Cancel', lambda: None), buttonObject('Install', lambda: installPackage(self.packageName))])

class buttonObject():
    name = ''
    action = None

    def __init__ (self, name, action):
        self.name = name
        self.action = action

    def proceedAction(self):
        self.action()

def systemMessage(message, buttons):
    sys.stdout.write('\033[2J\033[H')
    sys.stdout.flush()
    selectedButton = 0

    cols, rows = MIN_TERMINAL_COLS, MIN_TERMINAL_ROWS

    centeredMessageText = " " * ((cols - len(message))//2) + message
    
    try:
        with keyboard.Events() as events:
            while True:
                buttonsStr = ''
                for i in buttons:
                    if i == buttons[selectedButton]:
                        buttonsStr += " " * 5 + f'*{i.name}'
                    else:
                        buttonsStr += " " * 5 + f' {i.name}'
                
                centeredButtons = " " * ((cols - len(buttonsStr))//2) + buttonsStr
                systemMessage = centeredMessageText + '\n\n' + centeredButtons
                upperSpace = (rows - len(systemMessage.splitlines()))//2
                systemMessage = '\n' * upperSpace + systemMessage + '\n' * (rows - upperSpace - len(systemMessage.splitlines()) + 1 ) + '←/→ – move, Enter – select, Ctrl + C – quit'
                
                sys.stdout.write(systemMessage)
                sys.stdout.flush()

                event = events.get(1.0)
                if event is None:
                    continue

                if isinstance(event, keyboard.Events.Press):
                    key = event.key
                    if key == keyboard.Key.right:
                        selectedButton = (selectedButton + 1) % len(buttons)
                        continue
                    elif key == keyboard.Key.left:
                        selectedButton = (selectedButton - 1) % len(buttons)
                        continue
                    elif key == keyboard.Key.enter:
                        buttons[selectedButton].proceedAction()
                        return
    except KeyboardInterrupt:
        pass
    finally:
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()

def getSystemPluginsJson():
    try:
        headers = {
        'User-Agent': 'Mozilla/5.0',
        'Cache-Control': 'no-cache'
        }
        response = requests.get(GITHUB_JSON_URL, headers=headers, timeout=5, verify=False)
        response.raise_for_status() 
        
        remote_data = response.json()
        
        with open(LOCAL_SYSTEM_JSON_PATH, 'w', encoding='utf-8') as f:
            json.dump(remote_data, f, ensure_ascii=False, indent=4)
            
        return remote_data
    except Exception as e:
        if os.path.exists(LOCAL_SYSTEM_JSON_PATH):
            with open(LOCAL_SYSTEM_JSON_PATH, 'r', encoding='utf-8') as f:
                return json.load(f)
        systemMessage(f"Nothings was loaded: \n {e}. \n\n Check your internet connection and then reload. \n If its doesnt help try download plugins.json file from GitHub repository.", [buttonObject('Ok', lambda: None)])
        return {"plugins": []}

def getUsersPluginsJson():
    if os.path.exists(USER_JSON_PATH):
        with open(USER_JSON_PATH, 'r', encoding='utf-8') as f:
            return json.load(f)
    else:
        with open(USER_JSON_PATH, 'w', encoding='utf-8') as f:
            json.dump({"plugins": []}, f)
            return {"plugins": []}

plugins = getSystemPluginsJson()['plugins'] + getUsersPluginsJson()['plugins']

menuList = [menuObject(True if i == 0 else False, p) for i, p in enumerate(plugins)]

def createMatrix(menuList, rowsPerCols=10, colsPerPage=3):
    itemsPerPage = rowsPerCols * colsPerPage
    pages = []
    page = []

    for p in range(0, len(menuList), itemsPerPage):
        pageItems = menuList[p : p + itemsPerPage]
        pageStructure = []

        for i in range(0, len(pageItems), rowsPerCols):
            column = pageItems[i : i + rowsPerCols]
            
            if len(column) < rowsPerCols:
                column += [menuObject(False, {"type":"empty", "name":""})] * (rowsPerCols - len(column))
            pageStructure.append(column)

        if len(pageStructure) < colsPerPage:
            for _ in range(colsPerPage - len(pageStructure)):
                pageStructure.append([menuObject(False, {"type":"empty", "name":""})] * rowsPerCols)

        npPage = np.array(pageStructure, dtype=object)
        pages.append(npPage.T.tolist())
        
    return np.array(pages) 

selectedItem = 0, 0, 0

logo = r'''
     _,---.               .-._        ,--.-,,-,--,                         
  .-`.' ,  \ .--.-. .-.-./==/ \  .-._/==/  /|=|  |.--.-. .-.-.   _..---.   
 /==/_  _.-'/==/ -|/=/  ||==|, \/ /, /==|_ ||=|, /==/ -|/=/  | .' .'.-. \  
/==/-  '..-.|==| ,||=| -||==|-  \|  ||==| ,|/=| _|==| ,||=| -|/==/- '=' /  
|==|_ ,    /|==|- | =/  ||==| ,  | -||==|- `-' _ |==|- | =/  ||==|-,   '   
|==|   .--' |==|,  \/ - ||==| -   _ ||==|  _     |==|,  \/ - ||==|  .=. \  
|==|-  |    |==|-   ,   /|==|  /\ , ||==|   .-. ,\==|-   ,   //==/- '=' ,| 
/==/   \    /==/ , _  .' /==/, | |- |/==/, //=/  /==/ , _  .'|==|   -   /  
`--`---'    `--`..---'   `--`./  `--``--`-' `-`--`--`..---'  `-._`.___,'   
'''

def printMenu(menuMatrix):
    global prew_term_cols, prew_term_rows
    listToString = ''
    cols, rows = shutil.get_terminal_size()
    centeredLogo = ''
    menuPages, menuRows, menuCols = menuMatrix.shape

    if prew_term_cols != cols or prew_term_rows != rows:
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()
        prew_term_cols, prew_term_rows = cols, rows

    for i in logo.splitlines():
        centeredLogo += ' ' * int((cols - len(i))//2) + i + '\n'

    listToString += centeredLogo + '\n'

    page = ''
    maxNameWidthInColumns = [0, 0, 0]

    for p, i in enumerate( menuMatrix[selectedItem[0]].T.tolist()):
        for j in i:
            maxNameWidthInColumns[p] = max(maxNameWidthInColumns[p], len(j.getName()))

    for i in menuMatrix[selectedItem[0]]:
        row = ''

        for p, j in enumerate(i):
            row += j.getName().ljust(maxNameWidthInColumns[p]) + ' ' * 3 if p < 2 else j.getName().ljust(maxNameWidthInColumns[p])
        
        page += ' ' * ((cols - len(row))//2) + row + '\n'

    pageIndex = ('< ' if selectedItem[0] > 0 else '  ') + f'{selectedItem[0] + 1}/{menuPages}' + (' >' if selectedItem[0] < menuPages - 1 else '  ' )

    listToString += page + '\n' + ' ' * ((cols - len(pageIndex))//2) + (pageIndex if menuPages > 1 else '')

    upperSpace = (rows - len(listToString.splitlines()))//2

    listToString = '\n' * upperSpace + listToString + '\n' * (rows - upperSpace - len(listToString.splitlines()) - 1) + '↑/↓/←/→ – move, Enter – select, Ctrl + C – quit'

    sys.stdout.write(Cursor.POS(1, 1))
    sys.stdout.write(listToString)
    sys.stdout.flush()

def changeSelection(menu, direction):
    global selectedItem
    old_page = selectedItem[0]
    menu[selectedItem].isSelected = False
    pages, _, _ = menu.shape
    cols, rows = getPageDimentions(menu[selectedItem[0]])

    p, r, c = selectedItem # p - страница, c - колонка, r - строка

    # Вспомогательная функция для получения кол-ва реальных строк в колонке
    def get_limit(p_idx, c_idx):
        _, rows_list = getPageDimentions(menu[p_idx])
        return rows_list[c_idx]

    if direction == 'down':
        limit = get_limit(p, c)
        r += 1
        if r >= limit :
            r = 0
            c += 1
            if c > cols - 1:
                c = 0
                p = (p + 1) % pages
            
    elif direction == 'up':
        limit = get_limit(p, c)
        r -= 1
        if r < 0:
            r = limit - 1
            c -= 1
            if c < 0:
                p -= 1
                cur_cols, _ =  getPageDimentions(menu[p])
                c = cur_cols - 1
                limit = get_limit(p, c)
                r = limit - 1
                if p < 0:
                    p = pages - 1


    elif direction == 'right':
        c += 1
        if c >= cols: # Переход на следующую страницу
            c = 0
            p = (p + 1) % pages
        
        # Проверка: если в новой колонке меньше строк, чем текущий индекс r
        new_limit = get_limit(p, c)
        if new_limit == 0: # Если колонка совсем пустая
            r = 0
        elif r >= new_limit:
            r = new_limit - 1

    elif direction == 'left':
        c -= 1
        if c < 0: # Переход на предыдущую страницу
            p = (p - 1) % pages
            cur_cols, _ =  getPageDimentions(menu[p])
            c = cur_cols - 1
            
        new_limit = get_limit(p, c)
        # Если попали на пустую колонку в конце списка
        while new_limit == 0 and c > 0:
            c -= 1
            new_limit = get_limit(p, c)
        
        if r >= new_limit and new_limit > 0:
            r = new_limit - 1
        elif new_limit == 0:
            r = 0
    
    selectedItem = p,r,c    

    if selectedItem[0] != old_page:
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()

    menu[selectedItem].isSelected = True

def app_exit(exception):
    if exception:
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()
        if sys.platform != 'win32':
            termios.tcsetattr(fd,termios.TCSADRAIN, oldSettings)
        print(f'Caused exception: {exception}')
        flush_input()
        sys.exit(1)
    else:
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()
        if sys.platform != 'win32':
            termios.tcsetattr(fd,termios.TCSADRAIN, oldSettings)
            print("\033[?25h")
        flush_input()
        sys.exit(0)

def loadingAnimation(repeat):
    animationState = 0
    frame = ''
    cols, rows = shutil.get_terminal_size()


    sys.stdout.write('\033[2J\033[H')
    sys.stdout.flush()

    if repeat >= 0:
        for _ in range(repeat):
            if animationState == 0:
                frame = '\\'
                animationState += 1
            elif animationState == 1:
                frame = '|'
                animationState += 1
            elif animationState == 2:
                frame = '/'
                animationState += 1
            elif animationState == 3:
                frame = '-'
                animationState = 0

            sys.stdout.write(Cursor.POS(1, 1))    
            sys.stdout.write('\n' * ((rows - 1)//2)  + ' ' * ((cols - 1)//2) + frame)
            sys.stdout.flush()
            time.sleep(0.1)
    else:
        while isloadingEvent.is_set():
            if animationState == 0:
                frame = '\\'
                animationState += 1
            elif animationState == 1:
                frame = '|'
                animationState += 1
            elif animationState == 2:
                frame = '/'
                animationState += 1
            elif animationState == 3:
                frame = '-'
                animationState = 0

            sys.stdout.write(Cursor.POS(1, 1))    
            sys.stdout.write('\n' * ((rows - 1)//2)  + ' ' * ((cols - 1)//2) + frame)
            sys.stdout.flush()
            time.sleep(0.1)
    
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

def installPackage(packageName):
    try:
        isloadingEvent.set()
        loading = threading.Thread(target=loadingAnimation, args=(-1,), daemon=True)
        loading.start()
        subprocess.run([sys.executable, '-m', 'pip', 'install', packageName, '--quiet'], check=True, capture_output=True, text=True)
        isloadingEvent.clear()
        systemMessage(f'Plugin "{packageName}" was successfully installed.',[buttonObject('Ok', lambda: None)])
    
    except subprocess.CalledProcessError as e:
        systemMessage(f'Failed to install plugin "{packageName}". Error: {e.stderr}', [buttonObject('Ok', lambda: None)])
    
    except Exception as e:
        systemMessage(f'Failed to install plugin "{packageName}". Error: {e}', [buttonObject('Ok', lambda: None)])

def updatePackage(packageName):
    try:
        isloadingEvent.set()
        loading = threading.Thread(target=loadingAnimation, args=(-1,), daemon=True)
        loading.start()
        subprocess.run([sys.executable, '-m', 'pip', 'install', packageName, '--quiet', '--upgrade'], check=True, capture_output=True, text=True)
        isloadingEvent.clear()
        systemMessage(f'Plugin "{packageName}" was successfully updated.',[buttonObject('Ok', lambda: None)])
    
    except subprocess.CalledProcessError as e:
        systemMessage(f'Failed to install plugin "{packageName}". Error: {e.stderr}', [buttonObject('Ok', lambda: None)])
    
    except Exception as e:
        systemMessage(f'Failed to install plugin "{packageName}". Error: {e}', [buttonObject('Ok', lambda: None)])
    
def getPageDimentions(page):
    cols = 0
    for column in page.T:
        for p in column:
            if p.type != 'empty':
                cols += 1
                break
    rows = [0 for _ in range(cols)]
    
    for i in page:
        for p, j in enumerate(i):
            if j.type != 'empty':
                rows[p] += 1
            
    return cols, rows

def checkTerminalSize():
    cols, rows = shutil.get_terminal_size()
    if cols < MIN_TERMINAL_COLS or rows < MIN_TERMINAL_ROWS:
        forceTerminalSize(MIN_TERMINAL_COLS, MIN_TERMINAL_ROWS)
        cols, rows = shutil.get_terminal_size()
        if cols < MIN_TERMINAL_COLS or rows < MIN_TERMINAL_ROWS:
            systemMessage(f"Please resize your terminal to at least {MIN_TERMINAL_COLS} columns and {MIN_TERMINAL_ROWS} rows for the best experience.", [buttonObject('Ok', lambda: None)])
            return False
    return True

def flush_input():
    try:
        import termios
        termios.tcflush(sys.stdin, termios.TCIFLUSH)
    except:
        while msvcrt.kbhit():
            msvcrt.getch()

def checkUpdates():
    try:
        installedVersion = version(LAUNCHER_PACKAGER_NAME)
    except:
        installedVersion = None
    url = f"https://pypi.org/pypi/{LAUNCHER_PACKAGER_NAME}/json"
    response = requests.get(url, timeout=5)
    response.raise_for_status()
    latestVersion = response.json()["info"]["version"]

    if parse(latestVersion) > parse(installedVersion):
        systemMessage(f'Funhub not updated to latest vesrion. Update to latest version?',[buttonObject('Exit', lambda: app_exit(None)), buttonObject('Update', lambda: updatePackage(LAUNCHER_PACKAGER_NAME)), buttonObject('Launch with out update', lambda: None)])

def main():
    global selectedItem

    menu = createMatrix(menuList, ROWS_PER_COLUMN, COLUMNS_PER_PAGE)

    try:
        forceTerminalSize(150, 50)
        checkTerminalSize()
        sys.stdout.write('\033[2J\033[H')
        sys.stdout.flush()
        checkUpdates()
        printMenu(menu)
        with keyboard.Events() as events:
            while True:
                if checkTerminalSize():
                    event = events.get(1.0)
                    if event is None:
                        continue

                    if isinstance(event, keyboard.Events.Press):
                        key = event.key
                        if key == keyboard.Key.up:
                            changeSelection(menu, 'up')
                            printMenu(menu)
                        elif key == keyboard.Key.down:
                            changeSelection(menu,'down')
                            printMenu(menu)
                        elif key == keyboard.Key.right:
                            changeSelection(menu, 'right')
                            printMenu(menu)
                        elif key == keyboard.Key.left:
                            changeSelection(menu, 'left')
                            printMenu(menu)
                        elif key == keyboard.Key.enter:

                            sys.stdout.write('\033[2J\033[H')
                            sys.stdout.flush()  

                            try:
                                loadingAnimation(5)
                                menu[selectedItem].run()
                            except KeyboardInterrupt:
                                pass
                            while events.get(0.1) is not None:
                                pass
                            checkTerminalSize()
                            sys.stdout.write('\033[2J\033[H')
                            sys.stdout.flush()
                            printMenu(menu)
    
    except KeyboardInterrupt:
        app_exit(None)

    except Exception as exception:
        isloadingEvent.clear()
        systemMessage(f'An unexpected error occurred: {exception}', [buttonObject('Ok (exit)', lambda: app_exit(exception))])
    finally:
        if sys.platform != 'win32':
            termios.tcsetattr(fd, termios.TCSADRAIN, oldSettings)
        flush_input()

if __name__ == "__main__":
    main()
