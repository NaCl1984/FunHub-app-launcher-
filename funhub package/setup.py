from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="FunHub-app-launcher",
    version="1.0.4",
    py_modules=["menu"],
    description="CLI launcher for CLI apps",
    long_description=open("README.md", encoding="utf-8").read(),
    long_description_content_type="text/markdown",
    packages=find_packages(),
    install_requires=[
        "pynput>=1.7.6",
        "colorama>=0.4.6",
        "requests>=2.31.0",
        "numpy>=1.24.0",
        "packaging",
    ],
    entry_points={
        'console_scripts': [
            'funhub=menu:main',
        ],
    },
)