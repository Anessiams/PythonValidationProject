# Dockerfile, Image, Container
#base image 
FROM python:3.11

#create another director in the container base directory
WORKDIR /validator

COPY ./Requirements.txt /validator/Requirements.txt
# no cache to utilize docker cache to optimize rebuilding images
RUN pip install --no-cache-dir -r /validator/Requirements.txt

#copying the src code placed in src folder
COPY ./src /validator/src

RUN pip install posix-ipc 
# RUN pip install python-signal need to confirm if included or installed.

#this is the executable to run this container
# im using Runner as our executable file
# linux terminal equivalence: python3 Runner.py
CMD["python3", './Runner.py']
