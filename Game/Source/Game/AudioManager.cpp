#include "GamePCH.h"
#include <fstream>
#include "AudioManager.h"
#include "AudioEngine.h"
#include "AudioDataStructures.h"

AudioEngine* AudioManager::Engine = nullptr;
std::map<std::string, Audio*>* AudioManager::LoadedAudioMap = nullptr;
std::map<std::string, WaveData*>* AudioManager::LoadedWaveDataMap = nullptr;

void AudioManager::Initialize()
{
	if (Engine == nullptr)
		Engine = new AudioEngine();

	if (LoadedAudioMap == nullptr)
		LoadedAudioMap = new std::map<std::string, Audio *>;

	if (LoadedWaveDataMap == nullptr)
		LoadedWaveDataMap = new std::map<std::string, WaveData *>;
}


void AudioManager::Release()
{
	delete Engine;
	Engine = nullptr;

	for (auto object : *LoadedAudioMap)
		delete object.second;

	for (auto object : *LoadedWaveDataMap)
		delete object.second;

	delete LoadedAudioMap;
	delete LoadedWaveDataMap;
}

bool AudioManager::LoadFromPath(const char* name, const char* path)
{
	assert(LoadedWaveDataMap->find(name) == LoadedWaveDataMap->end());

	//filename pathing
	std::string aPath = "Data/Audio/" + (std::string)path + ".wav";

	//Open the input file
	std::ifstream inFile(aPath.c_str(), std::ios::binary | std::ios::in);
	if (inFile.bad())
	{
		return false;
	}

	unsigned long dwChunkId = 0;
	unsigned long dwFileSize = 0;
	unsigned long dwChunkSize = 0;
	unsigned long dwExtra = 0;

	//look for 'RIFF' chunk identifier
	inFile.seekg(0, std::ios::beg);
	inFile.read(reinterpret_cast<char*>(&dwChunkId), sizeof(dwChunkId));

	//if we didnt find RIFF then this isn't a wave file. Get out.
	if (dwChunkId != 'FFIR')
	{
		inFile.close();
		return false;
	}

	//get file size
	inFile.seekg(4, std::ios::beg); 
	inFile.read(reinterpret_cast<char*>(&dwFileSize), sizeof(dwFileSize));

	//if our file size is 16 bytes or less, get out.
	if (dwFileSize <= 16)
	{
		inFile.close();
		return false;
	}

	//get file format
	inFile.seekg(8, std::ios::beg); 
	inFile.read(reinterpret_cast<char*>(&dwExtra), sizeof(dwExtra));

	//if the file format isn't WAVE, get out.
	if (dwExtra != 'EVAW')
	{
		inFile.close();
		return false;
	}

	//Create the WaveData object
	WaveData* waveData = new WaveData();
	waveData->name = name;

	//look for 'fmt ' chunk id
	bool bFilledFormat = false;
	for (unsigned int i = 12; i < dwFileSize; )
	{
		//find our format ID
		inFile.seekg(i, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkId), sizeof(dwChunkId));

		//get our format size. by default this should end up being 16
		inFile.seekg(i + 4, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkSize), sizeof(dwChunkSize));

		if (dwChunkId == ' tmf')
		{
			//fill our wave format
			inFile.seekg(i + 8, std::ios::beg);
			inFile.read(reinterpret_cast<char*>(&waveData->waveFormat), sizeof(waveData->waveFormat));

			//break out now that format has been found and filled
			bFilledFormat = true;
			break;
		}

		//didn't find it. skip over a chunk.
		dwChunkSize += 8; //add offsets of the chunk id, and chunk size data entries
		dwChunkSize += 1;
		dwChunkSize &= 0xfffffffe; //guarantees WORD padding alignment
		i += dwChunkSize;
	}

	//we didnt get our format chunk. Get out.
	if (bFilledFormat == false)
	{
		delete waveData;
		inFile.close();
		return false;
	}

	//look for 'data' chunk id
	bool bFilledData = false;
	for (unsigned int i = 12; i < dwFileSize; )
	{
		//get our data ID
		inFile.seekg(i, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkId), sizeof(dwChunkId));

		//get our data chunk
		inFile.seekg(i + 4, std::ios::beg);
		inFile.read(reinterpret_cast<char*>(&dwChunkSize), sizeof(dwChunkSize));

		if (dwChunkId == 'atad')
		{
			//allocate space for the chunk
			waveData->data = new unsigned char[dwChunkSize];

			//fill the data with said chunk
			inFile.seekg(i + 8, std::ios::beg);
			inFile.read(reinterpret_cast<char*>(waveData->data), dwChunkSize);

			//set buffer values
			waveData->buffer.AudioBytes = dwChunkSize;
			waveData->buffer.pAudioData = (BYTE*)waveData->data;
			waveData->buffer.PlayBegin = 0;
			waveData->buffer.PlayLength = 0;

			//break out now that data has been found and filled
			bFilledData = true;
			break;
		}

		//didn't find it. skip over a chunk.
		dwChunkSize += 8; //add offsets of the chunk id, and chunk size data entries
		dwChunkSize += 1;
		dwChunkSize &= 0xfffffffe; //guarantees WORD padding alignment
		i += dwChunkSize;
	}

	//we didnt find our data chunk. Get out.
	if (!bFilledData)
	{
		delete waveData;
		inFile.close();
		return false;
	}

	//Close the input file
	inFile.close();

	//store the wave data in our audio manager map
	(*LoadedWaveDataMap)[name] = waveData;

	//Return true
	return true;
}

Audio* AudioManager::CreateAudio(const char* audioname, const char* filepath)
{
	assert(LoadedAudioMap->find(audioname) == LoadedAudioMap->end());

	Audio* audio = new Audio(audioname, filepath);
	(*LoadedAudioMap)[audioname] = audio;

	return audio;
}

Audio * AudioManager::GetAudio(const std::string AudioName)
{
	auto it = LoadedAudioMap->find(AudioName);
	if (it == LoadedAudioMap->end())
		return nullptr;

	return it->second;
}

WaveData* AudioManager::GetWaveData(const std::string AudioName)
{
	auto it = LoadedWaveDataMap->find(AudioName);
	if (it == LoadedWaveDataMap->end())
		return nullptr;

	return it->second;
}
