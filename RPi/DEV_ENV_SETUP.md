# Setting up python development environment


### Version Information of development environment

Python

	Python 2.7.13


### Set up guide for windows

#### 1. Ensure workstation has python 2.7.13

Ensure that your workstation has python version 2.7.13. If not, please install it [here](https://www.python.org/downloads/release/python-2713/)


#### 2. Install virtualenv using `pip` command

Installation of Python comes together with `pip`. `Pip` is a dependency package manager in which you can install 3rd party packages.

Execute the following command in your command line to install `virtualenv` for python.
```
pip install virtualenv
```

#### 3. Creating virtual environment

Create a virtual environment having the python 2.7.13 inside it. It is important to use a virtual environment since a development workstation may have both python3 and python2.7. Creating a virtual environment will sandbox your development environment with 1 python version.

Execute in command line the following step
```
cd CE3004---MDP   # change directory to mdp project folder
virtualenv --python=C:\Python27\python.exe venv    # create the virtualenv folder
venv\scripts\activate     # activate the virtual environment
```

#### 4. Install required python packages

Some 3rd party python packages are necessary for our MDP purposes. For example, `PyBluez` is a python package for `bluetooth` communication. Over time, the number of python packages we use will be expected to increase and these packages are listed in the [requirements.txt](../requirements.txt) file.

Install all the listed packages using the following command
```
pip install -r requirements.txt
```

To update the requirements.txt file with new packages that you have already installed and are necessary, execute the command
```
pip freeze > requirements.txt
```
