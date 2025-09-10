#!/usr/bin/bash
#
# Runs all build scripts in tmux panes
#

# This is used inside the release docker container
if [ "$1" = "--docker" ]; then
  echo "Importing PGP key..."
  gpg --import ~/private.pgp
  echo "Adding AUR ssh key..."
  eval $(ssh-agent -s)
  ssh-add ~/.ssh/aur_rsa
fi

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Upload source archive to GitHub release..."

# Upload source archive to GitHub release
"$DIR/github/build-github-src.sh"

# check if exit code was 0
rc=$?
if [[ $rc != 0 ]]; then exit $rc; fi

# Upload source archive to SourceForge
#$DIR/sourceforge/build-sourceforge-src.sh

echo "Starting the deployment process for all platforms..."

# lets wait a bit to make sure SourceForce has their files in place
#sleep 3

cd "$DIR" || exit 1

# kill the previous session
zellij delete-session deploy-qownnotes

# start a new deployment session with all deployment scripts from releaser.kdl
zellij --layout releaser.kdl attach deploy-qownnotes -c

exit

# fallback if zellij doesn't work
../ubuntu-launchpad/build-for-launchpad.sh
../ubuntu-launchpad/build-for-launchpad-qt6.sh
../obs/build-for-obs.sh
aur/build-for-aur.sh
gentoo/build-for-gentoo.sh
slackware/build-for-slackware.sh
snap/build-for-launchpad-snap.sh
obs/cmake-qt6/build-for-obs.sh

exit

#
# this was the previous konsole tabs implementation
#

# List of commands to run, with parameters, in quotes, space-separated; do not use quotes inside (see bash arrays)
COMMANDS=("$DIR/../ubuntu-launchpad/build-for-launchpad.sh" "$DIR/../obs/build-for-obs.sh" "$DIR/aur/build-for-aur.sh" "$DIR/gentoo/build-for-gentoo.sh" "$DIR/slackware/build-for-slackware.sh" "$DIR/snap/build-for-launchpad-snap.sh" "$DIR/obs/cmake-qt6/build-for-obs.sh")
# "$DIR/snap/build-for-local-snap.sh"

# KDS=$KONSOLE_DBUS_SERVICE # This is the ref of the current konsole and only works in a konsole
# KDS=$(org.kde.konsole)    # This is found in some examples but is incomplete

qdbus >/tmp/q0     # Get the current list of konsoles
/usr/bin/konsole & # Launch a new konsole
# PID=$!                    # And get its PID - But for some reason this is off by a few
sleep 1
qdbus >/tmp/q1 # Get the new list of konsoles
# KDS=org.kde.konsole-$PID
# KDS=org.kde.konsole       # Sometimes
KDS=$(diff /tmp/q{0,1} | grep konsole) # Let's hope there's only one
#echo $KDS
KDS=${KDS:3}
echo "$KDS"

echo $KDS >/tmp/KDS
echo >>/tmp/KDS

qdbus "$KDS" >>/tmp/KDS || exit
echo >>/tmp/KDS

# See note https://docs.kde.org/trunk5/en/applications/konsole/scripting.html about using /Konsole
qdbus $KDS /Windows/1 >>/tmp/KDS
echo >>/tmp/KDS

FirstTime=1

for i in "${COMMANDS[@]}"; do
  echo "Starting: $i"
  echo >>/tmp/KDS
  if [ $FirstTime -eq 1 ]; then
    session=$(qdbus $KDS /Windows/1 currentSession)
    FirstTime=0
  else
    session=$(qdbus $KDS /Windows/1 newSession)
  fi
  echo $session >>/tmp/KDS

  # Test: Display possible actions
  qdbus $KDS /Sessions/${session} >>/tmp/KDS

  # Doesn't work well, maybe use setTabTitleFormat 0/1 instead
  # Title "0" appears to be the initial title, title "1" is the title used after commands are executed.
  #qdbus $KDS /Sessions/${session} setTitle 0 $i
  #qdbus $KDS /Sessions/${session} setTitle 1 $i

  # The line break is necessary to commit the command. \n doesn't work
  qdbus $KDS /Sessions/${session} sendText "${i}
"

  # Optional: will ping when there's no more output in the window
  qdbus $KDS /Sessions/${session} setMonitorSilence true
done
