# Dockerfile, Image, Container
# base image 
FROM python:3.11

# create another directory in the container base directory
WORKDIR /validator

# copy requirements file to the container
COPY ./validator/Requirements.txt /validator/Requirements.txt

# install dependencies
RUN pip install --no-cache-dir -r Requirements.txt

# copy the source code placed in the src folder
COPY ./validator/src /validator/src

# install additional dependency
RUN pip install posix-ipc

# specify the executable to run this container
# using Runner as your executable file
# linux terminal equivalence: python3 Runner.py
CMD ["python3", "src/Runner.py"]
