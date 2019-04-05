#include "GamePCH.h"
#include "Game/Game.h"

#include "Mesh/Mesh.h"
#include "Mesh/Texture.h"
#include "Mesh/SpriteSheet.h"

#include "GameObjects/PlayerController.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "ImGuiManager.h"
#include "AudioManager.h"

#include "AudioEngine.h"
#include "Scenes/TitleScreen.h"

Game::Game(Framework* pFramework)
: GameCore( pFramework, new EventManager() )
{

    for( int i=0; i<4; i++ )
    {
        m_pControllers[i] = nullptr;
    }

	m_pResourceManager = nullptr;
	m_pSceneManager = nullptr;
	m_pImGuiManager = nullptr;
}

Game::~Game()
{
    for( int i=0; i<4; i++ )
    {
        delete m_pControllers[i];
    }

	delete m_pImGuiManager;

	delete m_pResourceManager;
	delete m_pSceneManager;
}

void Game::OnSurfaceChanged(unsigned int width, unsigned int height)
{
    // Set OpenGL to draw to the entire window.
    glViewport( 0, 0, width, height );
}

void Game::LoadContent()
{
#if WIN32
    // Turn on V-Sync.
    wglSwapInterval( 1 );
#endif

    // Turn on depth buffer testing.
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    // Turn on alpha blending.
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	m_pResourceManager = new ResourceManager(this);
	m_pSceneManager = new SceneManager();

	//MESHES
	{
		//Add things to resource manager.
		//m_pResourceManager->AddMesh("Cube", new Mesh());
		//m_pResourceManager->GetMesh("Cube")->CreateCube(vec3(1, 1, 1), vec3(0, 0, 0));

		m_pResourceManager->AddMesh("Box", new Mesh());
		m_pResourceManager->GetMesh("Box")->CreateBox(vec2(1.0f, 1.0f), vec2(0.0f, 0.0f));

		m_pResourceManager->AddMesh("Bullet", new Mesh());
		m_pResourceManager->GetMesh("Bullet")->CreateBox(vec2(0.25f, 0.25f), vec2(0.0f, 0.0f));

		m_pResourceManager->AddMesh("Plane", new Mesh());
		m_pResourceManager->GetMesh("Plane")->CreatePlane(vec2(100.0f, 100.0f), ivec2(600, 300));

		m_pResourceManager->AddMesh("Cube", new Mesh());
		m_pResourceManager->GetMesh("Cube")->GenerateOBJ("Data/OBJ/Cube.obj");

		m_pResourceManager->AddMesh("Sphere", new Mesh());
		m_pResourceManager->GetMesh("Sphere")->GenerateOBJ("Data/OBJ/Sphere.obj");

		m_pResourceManager->AddMesh("Table", new Mesh());
		m_pResourceManager->GetMesh("Table")->GenerateOBJ("Data/OBJ/Table.obj", 0.01f);

		m_pResourceManager->AddMesh("Teapot", new Mesh());
		m_pResourceManager->GetMesh("Teapot")->GenerateOBJ("Data/OBJ/Teapot.obj", 0.03f);

		m_pResourceManager->AddMesh("Boshy", new Mesh());
		m_pResourceManager->GetMesh("Boshy")->GenerateOBJ("Data/OBJ/Boshy.obj");
	}

    // Create our shaders.
	{
		m_pResourceManager->AddShader("TextureShader", new ShaderProgram("Data/Shaders/Texture.vert", "Data/Shaders/Texture.frag"));
		m_pResourceManager->AddShader("WaterShader", new ShaderProgram("Data/Shaders/Water.vert", "Data/Shaders/Water.frag"));
		m_pResourceManager->AddShader("DebugShader", new ShaderProgram("Data/Shaders/Debug.vert", "Data/Shaders/Debug.frag"));
		m_pResourceManager->AddShader("Debug3DShader", new ShaderProgram("Data/Shaders/Debug3D.vert", "Data/Shaders/Debug3D.frag"));
		m_pResourceManager->AddShader("NormalShader", new ShaderProgram("Data/Shaders/Normal.vert", "Data/Shaders/Normal.frag"));
		m_pResourceManager->AddShader("LightingShader", new ShaderProgram("Data/Shaders/Lighting.vert", "Data/Shaders/Lighting.frag"));
	}

    // Load some textures.
	{
		m_pResourceManager->AddTexture("Megaman", new Texture("Data/Textures/Megaman.png"));
		m_pResourceManager->AddTexture("Dice", new Texture("Data/Textures/Dice.png"));
		m_pResourceManager->AddTexture("Meteor", new Texture("Data/Textures/Meteor.png"));
		m_pResourceManager->AddTexture("Water", new Texture("Data/Textures/Water.png"));
	}

	//Spritesheets
	{
		//font
		m_pResourceManager->AddSpriteSheet("DefaultText", new SpriteSheet("Textures/DefaultFont_White", "0.png"));
	}

	{
		m_pResourceManager->AddMaterial("Megaman", new Material(m_pResourceManager->GetShader("TextureShader"), m_pResourceManager->GetTexture("Megaman")));
		m_pResourceManager->AddMaterial("Debug",   new Material(m_pResourceManager->GetShader("DebugShader"), m_pResourceManager->GetTexture("Dice")));
		m_pResourceManager->AddMaterial("Debug3D",   new Material(m_pResourceManager->GetShader("Debug3DShader"), m_pResourceManager->GetTexture("Dice")));
		m_pResourceManager->AddMaterial("Text",  new Material(m_pResourceManager->GetShader("TextureShader"), m_pResourceManager->GetSpriteSheet("DefaultText")));
	}

    // Create our controllers.
    for( int i=0; i<4; i++ )
    {
        m_pControllers[i] = new PlayerController();
    }

	//Scenes
	m_pSceneManager->AddScene("TitleScene", new TitleScreen(this, m_pResourceManager));

	m_pImGuiManager = new ImGuiManager();
	m_pImGuiManager->Init();

    CheckForGLErrors();
}

void Game::OnEvent(Event* pEvent)
{
	m_pControllers[0]->OnEvent(pEvent);

	m_pImGuiManager->OnEvent(pEvent);

	m_pSceneManager->OnEvent(pEvent);

#if WIN32
	// Enable/Disable V-Sync with F1 and F2.
	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;

		// Enable V-Sync.
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == VK_F1)
			wglSwapInterval(1);

		// Disable V-Sync.
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == VK_F2)
			wglSwapInterval(0);

		//reset everything
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 'R')
			Reset();
	}
#endif //WIN32
}

void Game::Update(float deltatime)
{

	if (deltatime > 0.1f)
	{
		deltatime = 0.1f;
	}
	m_pImGuiManager->StartFrame((float)m_pFramework->GetWindowWidth(), (float)m_pFramework->GetWindowHeight(), deltatime);

	ImGui::Begin("Sound");
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Public Channels"))
	{
		if (ImGui::Button("Stop All Public Channels"))
		{
			AudioManager::GetEngine()->StopAllPublicAudioChannels();
		}
	}
	ImGui::PopID();
	ImGui::End();

	m_pSceneManager->Update(deltatime);
}

void Game::Draw()
{
    // Setup the values we will clear to, then actually clear the color and depth buffers.
    glClearColor( 0.0f, 0.0f, 0.4f, 0.0f ); // (red, green, blue, alpha) - dark blue.
#if WIN32
    glClearDepth( 1 ); // 1 is maximum depth.
#endif
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	m_pSceneManager->Draw();

	m_pImGuiManager->EndFrame();

    CheckForGLErrors();
}

void Game::Reset()
{
	m_pSceneManager->Reset();
	m_pSceneManager->PushScene("TitleScene");
}
