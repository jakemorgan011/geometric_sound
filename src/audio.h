#define __MACOS_CORE__
#define GLOBAL_SAMPLERATE 48000
#define BUFFER_LEN 1024
#include <rtaudio/RtAudio.h>
// uhhh this will say it's not found but its installed globally on my machine through brew.
#include <sndfile.h>
#include <wx/log.h>

// ongod i donot want to do this rn MAY 21
class file_reader
{
public:
  file_reader() {}
  ~file_reader() {}
  // okay so this seems to be working. have to test and figure out a way to debug everything.
  // ABSOLUTE PATH NEEDED HERE.
  void setSample(const char *path)
  {
    file = sf_open(path, SFM_READ, &info);
    if (!file)
    {
      std::cout << "error reading file at:" << path << std::endl;
    }
    if (file)
    {
      std::cout << ".wav file set successfully" << std::endl;
      // info should contain all encoded wav data.
      auto check = sf_format_check(&info);
      std::cout << "1 for successful format, 0 for unsuccessful format." << std::endl;
      std::cout << "format result: " << check << std::endl;;
    }
  }

private:
  SF_INFO info{};
  SNDFILE *file;
};
// reference the rtaudio mcgill documentation to get this working MAY 23
class playback
{
public:
  playback() {}
  ~playback() {}

private:
};
