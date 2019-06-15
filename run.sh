#!/bin/bash

################################################################################
#################### For MacOS #################################################
################################################################################
if [ "$(uname)" == "Darwin" ]; then
  DEV_OPTS="-v ${HOME}:/mnt"

  # allow access from localhost, this will also start Xterm to export X11
  echo "Install XQuartz and allow connections from network clients to run UI `
       `apps inside docker"
  xhost + 127.0.0.1

  if [ -f /Users/${USER}/.bash_profile ]; then
    BASHRC="-v /Users/${USER}/.bash_profile:/home/dev/.bashrc:rw "
  fi
 
  eval "sudo docker pull ragumanjegowda/docker:latest"
  eval "sudo docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined `
          `-e DISPLAY=host.docker.internal:0 ` 
          `$DEV_OPTS $BASHRC -it ragumanjegowda/docker:latest /bin/bash"

################################################################################
#################### For Linux #################################################
################################################################################
elif [ "$(uname -s)" == "Linux" ]; then
  HOME_DIR="$(mktemp -d)"

  X_OPTS="-e SSH_CLIENT=${SSH_CLIENT} -e DISPLAY `
         `-e QT_X11_NO_MITSHM=1 --privileged `
         `-v ${HOME}/.Xauthority:/home/${USER}/.Xauthority:rw `
         `-v /tmp/.X11-unix:/tmp/.X11-unix:rw"

  DEV_OPTS="-v ${HOME}:/mnt"

  NET_OPTS='--net=host'

  AUTH_OPTS="-h $(hostname -f) `
            `-v ${HOME_DIR}:/home/${USER}:rw `
            `-e HOME -u $(id -u) `
            `-v /etc/passwd:/etc/passwd:ro `
            `-v /etc/shadow:/etc/shadow:ro `
            `-v /etc/pam.d/common-session:/etc/pam.d/common-session:ro `
            `-v /var/lib/sss/pipes:/var/lib/sss/pipes:rw"

  if [ -f /home/${USER}/.bashrc ]; then
    BASHRC="-v /home/${USER}/.bashrc:/home/${USER}/.bashrc:rw "
  fi

  if [ -d /data ]; then
    DATA_OPTS='-v /data:/data'
  fi
  
  eval "sudo docker pull ragumanjegowda/docker:latest"
  eval "sudo docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined `
          `$X_OPTS $DEV_OPTS $NET_OPTS $AUTH_OPTS $BASHRC $DATA_OPTS -w ${HOME} `
          `-it ragumanjegowda/docker:latest /bin/bash; rm -rf $HOME_DIR"

################################################################################
# Any other OS
################################################################################
else
  echo "$(uname) is not supported" >&2
  exit 1
fi
