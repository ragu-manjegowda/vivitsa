################################################################################
#################### For MacOS #################################################
################################################################################
if [ "$(uname)" == "Darwin" ]; then
  DEV_OPTS="-v ~/:/mnt"

  if [ -f /Users/${USER}/.bashrc ]; then
    BASHRC="-v /Users/${USER}/.bashrc:/home/dev/.bashrc:rw "
  fi
 
  eval "sudo docker pull ragumanjegowda/docker:latest"
  eval "sudo docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined `
		` $DEV_OPTS $BASHRC -it ragumanjegowda/docker:latest /bin/bash"

################################################################################
#################### For Linux #################################################
################################################################################
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  HOME_DIR="$(mktemp -d)"

  X_OPTS="'--env=SSH_CLIENT=${SSH_CLIENT}' --env=DISPLAY `
          ` --env=QT_X11_NO_MITSHM=1 --privileged `
          `--volume=${HOME}/.Xauthority:/home/${USER}/.Xauthority:rw `
          `--volume=/tmp/.X11-unix:/tmp/.X11-unix:rw"

  DEV_OPTS="-v ${HOME}:/mnt"

  NET_OPTS='--net=host'

  AUTH_OPTS="-h $(hostname -f) `
            `-v ${HOME_DIR}:/home/${USER}:rw `
            `-e HOME '--user=$(id -u)' `
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
          `$X_OPTS $DEV_OPTS $NET_OPTS $AUTH_OPTS $BASHRC $DATA_OPTS`
          `-it ragumanjegowda/docker:latest /bin/bash ; rm -rf $HOME_DIR"

################################################################################
# Any other OS
################################################################################
else
  echo "$(uname) is not supported" >&2
  exit 1
fi
