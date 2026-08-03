#pragma once

#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

//LJMU Framework Includes
#include "LJMUTextOverlay.h"
#include "LJMUSpriteOverlay.h"
#include "LJMUGeometryActor.h"

namespace Glyph3
{
	class TextActor;
}

using namespace Glyph3;

namespace LJMUDX
{
	//////////////////////////////////////
	//LJMULevelDemo.H
	//Class Application for a DirectX 11
	//Driven Application using the DirectX Toolkit
	//Hieroglyph 3 Rendering Engine and LUA.
	//
	//
	//AUTHORS:  DR PO YANG
	//			DR CHRIS CARTER
	//////////////////////////////////////

	class LJMULevelDemo : public Application //Inherit from the Hieroglyph Base Class
	{

	public:
		//------------CONSTRUCTORS------------------------------------------------
		LJMULevelDemo();	//Standard Empty Constructor which builds the object

	public:
		//------------INHERITED METHODS-------------------------------------------
		virtual void Initialize();					//Initialise the DirectX11 Scene
		virtual void Update();						//Update the DirectX Scene
		virtual void Shutdown();					//Shutdown the DirectX11 Scene

		virtual bool ConfigureEngineComponents();	//Initialise Hieroglyph and DirectX TK Modules
		virtual void ShutdownEngineComponents();	//Destroy Hieroglyph and DirectX TK Modules

		virtual void TakeScreenShot();				//Allow a screenshot to be generated

		virtual bool HandleEvent(EventPtr pEvent);	//Handle an I/O Event
		virtual std::wstring GetName();				//Get the Name of this App Instance

		//------------CUSTOM METHODS-----------------------------------------------
		void inputAssemblyStage();					//Stage to setup our VB and IB Info
		
		std::wstring outputFPSInfo();				//Convert the timer's Frames Per Second to a formatted string
		std::wstring outputCarPosition(Actor* pactor); //Outputs the car's position
		std::wstring outputCarOrientation(Actor* pactor); //Outputs the car's orientation
		std::wstring outputCarSpeed(Actor* pactor); //Outputs the car's speed
		std::wstring outputCarDebugMatrix(Actor* pactor); //Outputs car's debug matrix

		void setupLightSources();
		void setLightsToMaterial(MaterialPtr material);
		MaterialPtr setupMaterialProperties(MaterialPtr material);

		void updateLightSources();
		void applyLightsToAllMaterials();

		void changeTime(MaterialPtr material, float totalPlayTime);

	protected:
		//-------------CLASS MEMBERS-----------------------------------------------
		RendererDX11*			m_pRenderer11;		//Pointer to our DirectX 11 Device
		Win32RenderWindow*		m_pWindow;			//Pointer to our Windows-Based Window

		int						m_iSwapChain;		//Index of our Swap Chain 
		ResourcePtr				m_RenderTarget;		//Pointer to the GPU Render Target for Colour
		ResourcePtr				m_DepthTarget;		//Pointer to the GPU Render Target for Depth

		LJMUGeometryActor*      m_platformActor;
		float                   m_platformWidth = 256.0f;
		float                   m_platformLength = 256.0f;

		LJMUGeometryActor*      m_skyboxActor;
		float                   m_skyboxTimeValue;

		const float             DEG_TO_RAD = GLYPH_PI / 180.0f;

		LJMUGeometryActor*      m_carActor;
		ResourcePtr             m_carTexture;
		TextActor*              m_carLabelText;

		LJMUGeometryActor*      m_bull1Actor;
		ResourcePtr             m_bull1Texture;

		LJMUGeometryActor*      m_bull2Actor;
		ResourcePtr             m_bull2Texture;

		std::vector<Vector3f>   m_bull1Checkpoints;
		int                     m_currentBull1CheckpointIndex;
		int                     m_nextBull1CheckpointIndex;

		std::vector<Vector3f>   m_bull2Checkpoints;
		int                     m_currentBull2CheckpointIndex;
		int                     m_nextBull2CheckpointIndex;

		Vector3f                m_referenceBull1Direction;
		Vector3f                m_currentBull1Direction;
		std::vector<Vector3f>::value_type m_targetBull1Direction;
		float                   m_bull1DistanceToNextCheckpoint;

		Vector3f                m_referenceBull2Direction;
		Vector3f                m_currentBull2Direction;
		std::vector<Vector3f>::value_type m_targetBull2Direction;
		float                   m_bull2DistanceToNextCheckpoint;

		float                   m_bull1LinearSpeed;
		float                   m_bull1AngularSpeed;
		int                     m_bull1State;

		float                   m_bull2LinearSpeed;
		float                   m_bull2AngularSpeed;
		int                     m_bull2State;

		std::vector<Vector3f>   m_checkpoints;
		int                     m_currentCheckpointIndex;
		int                     m_nextCheckpointIndex;

		Vector3f                m_referenceCarDirection;
		Vector3f                m_currentCarDirection;
		std::vector<Vector3f>::value_type m_targetCarDirection;
		float                   m_carDistanceToNextCheckpoint;

		float                   m_carLinearSpeed;
		float                   m_carAngularSpeed;

		LJMUGeometryActor*      m_treeTrunkActor;
		LJMUGeometryActor*      m_treeTopActor;
		Actor*                  m_treeTrunkPivot;
		std::vector<LJMUGeometryActor*> m_treeTrunks;
		std::vector<LJMUGeometryActor*> m_treeTops;
		std::vector<Actor*>     m_treeTrunkPivots;

		LJMUGeometryActor*      m_flagpoleActor;
		LJMUGeometryActor*      m_flagActor;

		float                   m_totalPlayTime = 0;

		ResourcePtr             m_racetrackTexture;
		ResourcePtr             m_leavesTexture;
		ResourcePtr             m_treetrunkTexture;
		ResourcePtr             m_flagpoleTexture;
		ResourcePtr             m_flagTexture;
		ResourcePtr             m_skyboxTexture;
		ResourcePtr             m_skyboxTexture1;

		Vector4f               m_DirectionalLightDirection;
		Vector4f               m_DirectionalLightColour;

		Vector4f               m_PointLightPosition;
		Vector4f               m_PointLightColour;
		Vector4f               m_PointLightRange;

		Vector4f               m_SpotLightDirection;
		Vector4f               m_SpotLightColour;
		Vector4f               m_SpotLightPosition;
		Vector4f               m_SpotLightRange;
		Vector4f               m_SpotLightFocus;

		//--------------HIEROGLYPH OBJECTS-----------------------------------------
		ViewPerspective*		m_pRenderView;		//3D Output View - DirectX 11 Accelerated
		LJMUTextOverlay*        m_pRender_text;		//2D Output View - DirectX 11 Accelerated
		Camera*					m_pCamera;			//Camera Object

		float					m_iscreenWidth = 1280.0f;
		float					m_iscreenHeight = 720.0f;

		ResourcePtr             m_spriteTex;
		LJMUSpriteOverlay*      m_renderSprite;
		Camera*                 m_objSpriteCam;
	};
}
