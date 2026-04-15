from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="FunHub-minesweeper",
    version="1.0.3",
    py_modules=["minesweeper"],
    description="CLI minesweeper game for FunHud app-launcher",
    long_description=open("README.md", encoding="utf-8").read(),
    long_description_content_type="text/markdown",
    packages=find_packages(),
    install_requires=[
        "pynput>=1.7.6",
        "colorama>=0.4.6",
        "numpy>=1.24.0",
    ],
    entry_points={
        'console_scripts': [
            'funhub-minesweeper=minesweeper:main',
        ],
    },
)