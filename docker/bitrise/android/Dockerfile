FROM bitriseio/android-ndk:latest

# Install Google Cloud SDK for Firebase
RUN (echo "deb http://packages.cloud.google.com/apt cloud-sdk-$(lsb_release -c -s) main" | sudo tee /etc/apt/sources.list.d/google-cloud-sdk.list) && \
    (curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -) && \
    sudo apt-get update && \
    sudo apt-get install -y google-cloud-sdk python-dev python-setuptools && \
    sudo apt-get clean && \
    sudo easy_install -U pip

RUN git clone --depth 1 --branch bitrise-docker https://github.com/mapbox/mapbox-gl-native.git . && \
    make android-cmake-arm-v7 && \
    cd .. && \
    rm -rf src && \
    mkdir src
