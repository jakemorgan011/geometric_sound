echo "building with cmake..."
dir="wav"
if [ ! -d "$dir" ]; then 
  echo "no wav folder found... creating wav folder."
  mkdir "$dir"
else
  echo "wav folder found"
fi
cmake --build build -j
