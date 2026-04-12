from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="FunHub-tic-tac-toe",
    version="1.0.2",
    py_modules=["tic_tac_toe"],
    description="CLI tic-tac-toe game for FunHud app-launcher",
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
            'funhub-tic-tac-toe=tic_tac_toe:main',
        ],
    },
)