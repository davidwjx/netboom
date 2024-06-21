#!/bin/bash -x
if [ "$#" -lt "1" ]; then
    echo ""
    echo "Syntax:  deploy.sh <PLATFORM>"
    echo "Example: deploy.sh x64"
    echo "Exit..."
    exit 1
fi

PLATFORM=$1

# export CONAN_LOGIN_USERNAME_ARTIFACT_REMOTE=${CONAN_CRED_USR}
# export CONAN_PASSWORD_ARTIFACT_REMOTE=${CONAN_CRED_PSW}
"${CONAN_PATH}" remove "inucommon_${PLATFORM}/${PKG_VER}" -f

cp "conan/${PLATFORM}/conanfile.py" .
"${CONAN_PATH}" create . inuitive/prod
PKG_VER=$(echo $(cat include/Version.h | grep "define INUCOMMON_VERSION_STR" | awk -F '\"' '{print $2}') | sed -e 's/[[:space:]]*$//')
"${CONAN_PATH}" upload "inucommon_${PLATFORM}/${PKG_VER}@inuitive/prod" -r artifact-remote --all

exit 0 