#ifndef __ResourceManager_H__
#define __ResourceManager_H__

class ResourceManager;

#include "Mesh/Material.h"
#include "Mesh/Mesh.h"
#include "Mesh/Texture.h"
#include "Mesh/SpriteSheet.h"

class Game;
class EventManager;
struct WaveData;
class ShaderProgram;
class SpriteSheet;
class Audio;
class AudioList;

class ResourceManager
{
protected:
	std::map<std::string, Mesh*> m_pMeshes;
	std::map<std::string, ShaderProgram*> m_pShaders;
	std::map<std::string, Texture*> m_pTextures;
	std::map<std::string, SpriteSheet*> m_pSheets;
	std::map<std::string, Material*> m_pMaterials;
	std::map<std::string, AudioList*> m_AudioLists;

	EventManager* m_EventManager;

public:
	ResourceManager(Game* game);
	virtual ~ResourceManager();

	Mesh* AddMesh(std::string name, Mesh* pMesh);
	void AddShader(std::string name, ShaderProgram* pShader);
	void AddTexture(std::string name, Texture* pTexture);
	void AddMaterial(std::string name, Material* pMaterial);
	void AddSpriteSheet(std::string name, SpriteSheet* pSheet);
	AudioList* AddAudioList(const char* ListName, AudioList* list);

	//Creates an Audio voice.
	// Parameter 1 : Name the audio voice
	// Parameter 2 : Name of the WaveData to use.
	Audio* CreateAudio(const char* AudioName, const char* WaveDataName);

	//Load raw wav file
	// Parameter 1 : Name the Wav file.
	// Parameter 2 : The filepath to locate. (The set path is "Data/Audio/'FilePath'.wav". do not include .wav yourself)
	void LoadWaveData(const char* WaveMane, const char* FilePath);

	Mesh*          GetMesh(const std::string name);
	ShaderProgram* GetShader(const std::string name);
	Texture*       GetTexture(const std::string name);
	Material*      GetMaterial(const std::string name);
	SpriteSheet*   GetSpriteSheet(const std::string name);
	Audio*		   GetAudio(const std::string name);
	AudioList*	   GetAudioList(const std::string name);

	void HandleEvent(Event* pEvent);

	void ImGuiDisplayAudioLists();

};

#endif //__ResourceManager_H__
