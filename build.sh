#!/bin/sh

if [ ! -d "bin" ]; then 
  echo "Create bin folder"
  mkdir bin 
fi

echo "Change to bin folder"
cd bin

echo "Make clean"
make clean

if [ -d "./DependencyChaos.app" ]; then 
  echo "Delete existing app"
  rm -R DependencyChaos.app
fi

echo "Qmake..."
qmake ../DependencyChaos.pro -spec macx-clang CONFIG+=debug CONFIG+=x86_64 && /usr/bin/make qmake_all

echo "Make..."
make

echo "Copy dependencies"
cp -R ../deploy/Frameworks DependencyChaos.app/Contents
cp -R ../deploy/Plugins DependencyChaos.app/Contents


echo "Done"
