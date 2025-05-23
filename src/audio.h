#define __MACOS_CORE__
#define GLOBAL_SAMPLERATE 48000
#define BUFFER_LEN 1024
#include <rtaudio/RtAudio.h>
#include <sndfile.h>
#include <wx/log.h>

// ongod i donot want to do this rn MAY 21
class file_reader {
public:
  file_reader(){}
  ~file_reader(){}
  // okay so this seems to be working. have to test and figure out a way to debug everything.
  // ABSOLUTE PATH NEEDED HERE.
  void setSample(const char* path){
    file = sf_open(path, SFM_READ, &info);
    if(!file){
      wxLogDebug("error reading sample... ensure sample is contains .wav extension.");
    }
    if(file){
      wxLogDebug(".wav sample has been read successfully.");
      // info should contain all encoded wav data.
      auto check = sf_format_check(&info);
      wxLogDebug("1 for successful format, 0 for unsuccessful format.");
      wxLogDebug("format result: %d", check);
    }
  }
private:
  SF_INFO info{};
  SNDFILE* file;
};

class playback{
public:
  playback(){}
  ~playback(){}

private:
};
