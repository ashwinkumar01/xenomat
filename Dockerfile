FROM lpenz/debian-squeeze-i386:latest

MAINTAINER Peter Hübel "https://github.com/huebel"

ENV DEBIAN_FRONTEND noninteractive

ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8
ENV LC_ALL en_US.UTF-8
ENV HOME /root
ENV LD_LIBRARY_PATH /opt/xeno/lib

# Prepare
RUN \
  mkdir -p /opt/xeno/bin && \
  mkdir -p /opt/xeno/js && \
  mkdir -p /opt/xeno/lib && \
  cd /opt/xeno/lib && \
  wget http://netropy.huebel.freeshells.org:8901/download/lib/libboost_chrono.so.1.48.0 && \
  wget http://netropy.huebel.freeshells.org:8901/download/lib/libboost_system.so.1.48.0 && \
  wget http://netropy.huebel.freeshells.org:8901/download/lib/libboost_thread.so.1.48.0 && \
  cd /opt/xeno/js && \
  wget http://netropy.huebel.freeshells.org:8901/download/js/d3.min.js && \
  wget http://netropy.huebel.freeshells.org:8901/download/js/interact.js && \
  echo "Done!"
    
# Copy xenomat
ADD ./bin/debian/netropy /opt/xeno/bin/xenomat
ADD ./examples/blueprint.xml /opt/xeno/blueprint.xml
ADD ./examples/blueprint.html /opt/xeno/blueprint.html
ADD ./examples/blueprint.css /opt/xeno/blueprint.css
ADD ./examples/blueprint.js /opt/xeno/blueprint.js

# Fix permissions
RUN chmod 755 /opt/xeno/bin/xenomat

# Export XENO port
EXPOSE 8901

# default command
CMD ["/opt/xeno/bin/xenomat", "-d", "/opt/xeno/", "/opt/xeno/blueprint.xml"]
