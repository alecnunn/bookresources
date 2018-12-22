#ifndef _WAVEFILEFACTORY_H_
#define _WAVEFILEFACTORY_H_

#include <string>
#include <map>

class WaveFile;

// This is a singleton factory that manages .wav file memory.
// Currently it just loads a wav file when it is requested
// and keeps it in memory. Eventually you would want it to keep
// only frequently requested and short duration wav files in memory
class WaveFileFactory
{
	public:
		WaveFileFactory();
		virtual ~WaveFileFactory();

		static WaveFileFactory* Instance();

		WaveFile* Get(const char*);

    protected:
		static WaveFileFactory* s_pInstance;

		typedef std::map<std::string, WaveFile*> WavMap;
		typedef WavMap::iterator WavIterator;
		
		WavMap m_wavs;
};

inline WaveFileFactory* WaveFileFactory::Instance()
{
	if (!s_pInstance)
		s_pInstance = new WaveFileFactory();

	return s_pInstance;
}


#endif