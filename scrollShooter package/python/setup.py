from setuptools import setup, find_packages
import sys
from pathlib import Path

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="funhub-scroll-shooter",
    version="0.1.5",
    py_modules=["scroll-shooter"],
    description="CLI scroll shooter game for FunHud app-launcher",
    long_description=open("README.md", encoding="utf-8").read(),
    long_description_content_type="text/markdown",
    packages=find_packages(),
    include_package_data=True,
    package_data={
        "runner":  ["binaries/**/*"],
    },
    entry_points={
        "console_scripts": [
            "funhub-scroll-shooter = runner:run_binary",
        ],
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: OS Independent",
        "License :: OSI Approved :: MIT License",
    ],
    python_requires=">=3.6",
)