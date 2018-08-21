FROM circleci/node:10-browsers

RUN sudo apt-get install xvfb libgtk2.0-0 libnotify-dev libgconf-2-4 libnss3 libxss1 libasound2
# RUN cat /etc/passwd
WORKDIR /home/circleci/temp
RUN sudo chown -R circleci:circleci /home/circleci/temp
USER circleci
RUN echo "{}" > package.json && npm install cypress && \
  cd .. && rm -rf ~/temp/
