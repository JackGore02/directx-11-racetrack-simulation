//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

//------------DX TK AND STD/STL Includes-------------------------------------
#include <sstream>

//------------Include Hieroglyph Engine Files--------------------------------

//Include the Logging System
#include "Log.h"

//Include the Event System
#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"
#include "ScriptManager.h"

//Include the DirectX Rendering Components
#include <iomanip>

#include "PipelineManagerDX11.h"
#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "MaterialGeneratorDX11.h"
#include <SamplerStateConfigDX11.h>

#include "FirstPersonCamera.h"
#include "../../Source/Objects/TextActor.h"

#include "LJMUMeshOBJ.h"
#include <FileSystem.h>

#include "../../Source/Mathematics/PerlinNoise.h"

//Add a Using Directive to avoid typing Glyph3 for basic constructs
using namespace Glyph3;
//Include our own application Namespace
using namespace LJMUDX;

LJMULevelDemo AppInstance; 

//---------CONSTRUCTORS-------------------------------------------------------

///////////////////////////////////////
//
///////////////////////////////////////
LJMULevelDemo::LJMULevelDemo():
m_pRender_text(nullptr),
m_pRenderView(nullptr),
m_pCamera(nullptr),
m_pRenderer11(nullptr),
m_pWindow(nullptr),
m_iSwapChain(0),
m_DepthTarget(nullptr),
m_RenderTarget(nullptr)
{
	
}

//---------METHODS------------------------------------------------------------

//////////////////////////////////////
// Get the Window Name of this Application
//////////////////////////////////////
std::wstring LJMULevelDemo::GetName()
{
	return(std::wstring(L"DirectX11 Racetrack Simulation"));
}

/////////////////////////////////////
// Assemble our Input Layouts for this
// Stage of the Pipeline.
/////////////////////////////////////
void LJMULevelDemo::inputAssemblyStage()
{
	//-----LOAD SPRITES FOR THIS SCENE-----------------------------------------
	this->m_spriteTex = RendererDX11::Get()->LoadTexture(L"CarSprite.png");

	//-----LOAD TEXTURES FOR THIS SCENE-----------------------------------------
	m_racetrackTexture = RendererDX11::Get()->LoadTexture(L"racetrack.png");
	m_leavesTexture = RendererDX11::Get()->LoadTexture(L"leaves.png");
	m_treetrunkTexture = RendererDX11::Get()->LoadTexture(L"treetrunk.png");
	m_flagpoleTexture = RendererDX11::Get()->LoadTexture(L"pole.png");
	m_flagTexture = RendererDX11::Get()->LoadTexture(L"flag.png");
	m_skyboxTexture = RendererDX11::Get()->LoadTexture(L"sky1.png");
	m_skyboxTexture1 = RendererDX11::Get()->LoadTexture(L"sky0.png");

	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------
	setupLightSources();

	m_platformActor = new LJMUGeometryActor();
	m_platformActor->DrawRect(Vector3f(0.0f, 0.0f, 0.0f),
		  Vector3f(1.0f, 0.0f, 0.0f),
		  Vector3f(0.0f, 0.0f, -1.0f),
		Vector2f(m_platformWidth, m_platformLength));
	setupMaterialProperties(m_platformActor->m_pLitTexturedMaterial);
	setLightsToMaterial(m_platformActor->m_pLitTexturedMaterial);
	m_platformActor->UseLitTexturedMaterial(m_racetrackTexture);
	m_platformActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	this->m_pScene->AddActor(m_platformActor);

	m_skyboxActor = new LJMUGeometryActor();
	m_skyboxActor->DrawSphere(Vector3f(0.0f, 0.0f, 0.0f),
		500.0f, 20, 40);
	m_skyboxActor->UseSkyboxMaterial(m_skyboxTexture, m_skyboxTexture1);
	m_skyboxActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	this->m_pScene->AddActor(m_skyboxActor);

	Vector4f base_colour = Vector4f(1, 1, 1, 1);

	m_carTexture = RendererDX11::Get()->LoadTexture(L"Lamborginhi_Aventador_diffuse.png");
	BasicMeshPtr car_geometry = m_carActor->generateObjMesh(L"Lamborghini_Aventador_triangles.obj", base_colour);
	MaterialPtr car_material = m_carActor->createLitTexturedMaterial();

	setupMaterialProperties(car_material);
	setLightsToMaterial(car_material);
	
	m_carActor = new LJMUGeometryActor();
	m_carActor->applyTextureToMaterial(car_material, m_carTexture);
	m_carActor->GetBody()->SetGeometry(car_geometry);
	m_carActor->GetBody()->SetMaterial(car_material);
	m_carActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	m_carActor->GetNode()->Scale() = Vector3f(0.1f, 0.1f, 0.1f);
	this->m_pScene->AddActor(m_carActor);

	m_carLinearSpeed = 40.0f;
	m_carAngularSpeed = 1.5f;

	const int NumberOfCheckpoints = 31;
	Vector3f checkpointCoords[NumberOfCheckpoints] = {
		
		{ 10.24, 0, 128.64 },//1 
		{-185.6, 0, -1.28 },//2
		{ -213.76, 0, -50.56 },//3
		{-210.56, 0, -108.8 },//4
		{-180.48, 0, -152.32 },//5
		{-142.08, 0, -175.36 },//6
		{ -80, 0, -176 },//7
		{ 129.92, 0, -38.4 },//8
		{ 161.92, 0, 3.2 },//9
		{ 176.64, 0, 52.8 },//10
		{ 188.8, 0, 131.84 },//11
		{ 170.88, 0, 179.2 },//12
		{ 140.16, 0, 198.4},//13
		{ 112, 0, 182.4 },//14
		{ 110.72, 0, 145.92 },//15
		{ 140.16, 0, 86.4 },//16
		{135.04, 0, 28.8 },//17
		{ 112.64, 0, -7.04 },//18
		{ -92.16, 0, -141.44 },//19
		{ -143.36, 0, -131.2 },//20
		{ -168.96, 0, -108.16 },//21
		{ -177.92, 0, -74.24 },//22
		{ -165.12, 0, -31.36 },//23
		{ -119.68, 0, -3.2 },//24
		{ -60.8, 0, -3.2 },//25
		{ 0, 0, -14.72 },//26
		{ 64, 0, 2.56 },//27
		{ 96, 0, 33.92 },//28
		{ 101.76, 0, 81.92 },//29
		{ 74.88, 0, 122.88 },//30
		{ 42.24, 0, 131.84 }//31
	};

	for (int i = 0; i < NumberOfCheckpoints; i++)
	{
		m_checkpoints.push_back(checkpointCoords[i]);
	}

	m_currentCheckpointIndex = 0;
	m_nextCheckpointIndex = m_currentCheckpointIndex + 1;

	Matrix3f tStartRotation;

	m_referenceCarDirection = Vector3f(0, 0, 1);
	m_currentCarDirection = m_referenceCarDirection;
	m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_checkpoints[m_currentCheckpointIndex];
	m_carDistanceToNextCheckpoint = m_targetCarDirection.Magnitude();
	m_targetCarDirection.Normalize();

	float angle = acos(m_referenceCarDirection.Dot(m_targetCarDirection));

	Vector3f axis = m_referenceCarDirection.Cross(m_targetCarDirection);
	axis.Normalize();

	tStartRotation.RotationEuler(axis, angle);

	m_carActor->GetNode()->Rotation() = tStartRotation;
	m_carActor->GetNode()->Position() = m_checkpoints[m_currentCheckpointIndex];

	float carTextSize = 10.0f;
	m_carLabelText = new TextActor();
	m_carLabelText->SetColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	m_carLabelText->SetText(L"P1");
	m_carLabelText->SetLineJustification(LineJustification::RIGHT);
	m_carLabelText->GetNode()->Scale() = Vector3f(1.0f, 1.0f, 1.0f) * carTextSize;
	this->m_pScene->AddActor(m_carLabelText);

	const int numberOfTrees = 16;
	Vector3f treePositions[numberOfTrees] = {
		{ 207.36, 0, -128.64 },//1
		{ 197.76, 0, -213.76 },//2
		{ 131.84, 0, -174.08 },//3
		{ 78.08, 0, -208 },//4
		{ -40.96, 0, -230.4 },//5
		{ -145.28, 0, -236.16 },//6
		{ -225.92, 0, -205.44 },//7
		{ -231.68, 0, 46.72 }, //8
		{ -214.4, 0, 124.8 },//9
		{ -157.44, 0, 151.68 },//10
		{ -111.36, 0, 200.32 },//11
		{ 17.92, 0, 227.2 },//12
		{ 225.28, 0, 229.76 },//13
		{ 230.4, 0, 8.32 },//14
		{ -103.68, 0, -69.12 },//15
		{ -202.88, 0, 211.2 }//16
	};

	float treeTrunkLength = 8.0f;
	float treeTrunkSize = 1.5f;

	for (int i = 0; i < numberOfTrees; i++)
	{
		m_treeTrunkActor = new LJMUGeometryActor();
		setupMaterialProperties(m_treeTrunkActor->m_pLitTexturedMaterial);
		setLightsToMaterial(m_treeTrunkActor->m_pLitTexturedMaterial);
		m_treeTrunkActor->UseLitTexturedMaterial(m_treetrunkTexture);
		m_treeTrunkActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, treeTrunkLength, 1.0f));
		m_treeTrunkActor->GetNode()->Position() = Vector3f(0.0f, treeTrunkLength * treeTrunkSize, 0.0f);
		m_treeTrunkActor->GetNode()->Scale() = Vector3f(1, 1, 1) * treeTrunkSize;
		this->m_pScene->AddActor(m_treeTrunkActor);
		m_treeTrunks.push_back(m_treeTrunkActor);

		m_treeTopActor = new LJMUGeometryActor();
		setupMaterialProperties(m_treeTopActor->m_pLitTexturedMaterial);
		setLightsToMaterial(m_treeTopActor->m_pLitTexturedMaterial);
		m_treeTopActor->UseLitTexturedMaterial(m_leavesTexture);
		m_treeTopActor->DrawCylinder(Vector3f(0, 0, 0), Vector3f(0, 12, 0), 6, 0, 5, 20);
		m_treeTopActor->DrawDisc(Vector3f(0, 0, 0), Vector3f(0, -1, 0), 6);
		m_treeTopActor->GetNode()->Position() = Vector3f(0.0f, treeTrunkLength, 0.0f);
		this->m_pScene->AddActor(m_treeTopActor);
		m_treeTops.push_back(m_treeTopActor);

		Glyph3::Node3D* treeTopNode = m_treeTopActor->GetNode();
		m_treeTrunkActor->GetNode()->AttachChild(treeTopNode);

		m_treeTrunkPivot = new Actor();
		Glyph3::Node3D* treeTrunkNode = m_treeTrunkActor->GetNode();
		m_treeTrunkPivot->GetNode()->AttachChild(treeTrunkNode);
		m_treeTrunkPivot->GetNode()->Position() = treePositions[i];
		this->m_pScene->AddActor(m_treeTrunkPivot);
		m_treeTrunkPivots.push_back(m_treeTrunkPivot);
	}

	m_bull1Texture = RendererDX11::Get()->LoadTexture(L"bull_diff.png");
	BasicMeshPtr bull_geometry = m_bull1Actor->generateObjMesh(L"bull.obj", base_colour);
	MaterialPtr bull_material = m_bull1Actor->createLitTexturedMaterial();

	setupMaterialProperties(bull_material);
	setLightsToMaterial(bull_material);
	
	m_bull1Actor = new LJMUGeometryActor();
	m_bull1Actor->applyTextureToMaterial(bull_material, m_bull1Texture);
	m_bull1Actor->GetBody()->SetGeometry(bull_geometry);
	m_bull1Actor->GetBody()->SetMaterial(bull_material);
	m_bull1Actor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	m_bull1Actor->GetNode()->Scale() = Vector3f(0.3f, 0.3f, 0.3f);
	this->m_pScene->AddActor(m_bull1Actor);

	m_bull1LinearSpeed = 20.0f;
	m_bull1AngularSpeed = 1.0f;
	m_bull1State = 0;

	const int NumberOfBull1Checkpoints = 3;
	Vector3f bull1CheckpointCoords[NumberOfBull1Checkpoints] = {
		{ -76.8f, 0, 147.84f},
		{ -144.64, 0, 88.96 },
		{ -118.4, 0, 174.72 }
	};

	for (int i = 0; i < NumberOfBull1Checkpoints; i++)
	{
		m_bull1Checkpoints.push_back(bull1CheckpointCoords[i]);
	}

	m_currentBull1CheckpointIndex = 0;
	m_nextBull1CheckpointIndex = m_currentBull1CheckpointIndex + 1;

	Matrix3f bull1StartRotation;

	m_referenceBull1Direction = Vector3f(0, 0, 1);
	m_currentBull1Direction = m_referenceBull1Direction;
	m_targetBull1Direction = m_bull1Checkpoints[m_nextBull1CheckpointIndex] - m_bull1Checkpoints[m_currentBull1CheckpointIndex];
	m_bull1DistanceToNextCheckpoint = m_targetBull1Direction.Magnitude();
	m_targetBull1Direction.Normalize();

	float bull1Angle = acos(m_referenceBull1Direction.Dot(m_targetBull1Direction));

	Vector3f bull1Axis = m_referenceBull1Direction.Cross(m_targetBull1Direction);
	bull1Axis.Normalize();

	bull1StartRotation.RotationEuler(bull1Axis, bull1Angle);

	m_bull1Actor->GetNode()->Rotation() = bull1StartRotation;
	m_bull1Actor->GetNode()->Position() = m_bull1Checkpoints[m_currentBull1CheckpointIndex];

	m_bull2Texture = RendererDX11::Get()->LoadTexture(L"bull_diff.png");
	BasicMeshPtr bull2_geometry = m_bull2Actor->generateObjMesh(L"bull.obj", base_colour);
	MaterialPtr bull2_material = m_bull2Actor->createLitTexturedMaterial();

	setupMaterialProperties(bull2_material);
	setLightsToMaterial(bull2_material);

	m_bull2Actor = new LJMUGeometryActor();
	m_bull2Actor->applyTextureToMaterial(bull2_material, m_bull2Texture);
	m_bull2Actor->GetBody()->SetGeometry(bull2_geometry);
	m_bull2Actor->GetBody()->SetMaterial(bull2_material);
	m_bull2Actor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
	m_bull2Actor->GetNode()->Scale() = Vector3f(0.3f, 0.3f, 0.3f);
	this->m_pScene->AddActor(m_bull2Actor);

	m_bull2LinearSpeed = 15.0f;
	m_bull2AngularSpeed = 1.0f;
	m_bull2State = 0;

	const int NumberOfBull2Checkpoints = 4;
	Vector3f bull2CheckpointCoords[NumberOfBull2Checkpoints] = {
		{ 140.16, 0, -112.64 },
		{ 130.56, 0, -161.92 },
		{ 69.76, 0, -137.6 },
		{ 81.28, 0, -174.72 }
	};

	for (int i = 0; i < NumberOfBull2Checkpoints; i++)
	{
		m_bull2Checkpoints.push_back(bull2CheckpointCoords[i]);
	}

	m_currentBull2CheckpointIndex = 0;
	m_nextBull2CheckpointIndex = m_currentBull2CheckpointIndex + 1;

	Matrix3f bull2StartRotation;

	m_referenceBull2Direction = Vector3f(0, 0, 1);
	m_currentBull2Direction = m_referenceBull2Direction;
	m_targetBull2Direction = m_bull2Checkpoints[m_nextBull2CheckpointIndex] - m_bull2Checkpoints[m_currentBull2CheckpointIndex];
	m_bull2DistanceToNextCheckpoint = m_targetBull2Direction.Magnitude();
	m_targetBull2Direction.Normalize();

	float bull2Angle = acos(m_referenceBull2Direction.Dot(m_targetBull2Direction));

	Vector3f bull2Axis = m_referenceBull2Direction.Cross(m_targetBull2Direction);
	bull2Axis.Normalize();

	bull2StartRotation.RotationEuler(bull2Axis, bull2Angle);

	m_bull2Actor->GetNode()->Rotation() = bull2StartRotation;
	m_bull2Actor->GetNode()->Position() = m_bull2Checkpoints[m_currentBull2CheckpointIndex];

	float flagpolelength = 12.0f;
	float flagpolesize = 1.5f;

	m_flagpoleActor = new LJMUGeometryActor();
	setupMaterialProperties(m_flagpoleActor->m_pLitTexturedMaterial);
	setLightsToMaterial(m_flagpoleActor->m_pLitTexturedMaterial);
	m_flagpoleActor->UseLitTexturedMaterial(m_flagpoleTexture);
	m_flagpoleActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, flagpolelength, 1.0f));
	m_flagpoleActor->GetNode()->Position() = Vector3f(185.0f, flagpolelength * flagpolesize, 0.0f);
	m_flagpoleActor->GetNode()->Scale() = Vector3f(1, 1, 1) * flagpolesize;
	this->m_pScene->AddActor(m_flagpoleActor);

	float flagwidth = 3.0f;
	float flagheight = 2.0f;
	float flagdepth = 0.3f;

	m_flagActor = new LJMUGeometryActor();
	setupMaterialProperties(m_flagActor->m_pLitTexturedMaterial);
	setLightsToMaterial(m_flagActor->m_pLitTexturedMaterial);
	m_flagActor->UseLitTexturedMaterial(m_flagTexture);
	m_flagActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(flagwidth, flagheight, flagdepth));
	m_flagActor->GetNode()->Position() = Vector3f(-7.0f, 8.0f, 0.0f);
	m_flagActor->GetNode()->Scale() = Vector3f(2, 2, 2);
	this->m_pScene->AddActor(m_flagActor);

	Glyph3::Node3D* flagNode = m_flagActor->GetNode();
	m_flagpoleActor->GetNode()->AttachChild(flagNode);
}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	this->inputAssemblyStage();

	this->m_pCamera = new FirstPersonCamera();
	this->m_pCamera->SetEventManager(&this->EvtManager);

	Vector3f tcamerapos(0, 150, -350);
	this->m_pCamera->Spatial().SetTranslation(tcamerapos);
	this->m_pCamera->Spatial().RotateXBy(20 * DEG_TO_RAD);

	this->m_pRenderView = new ViewPerspective(*this->m_pRenderer11,
		                                     this->m_RenderTarget, this->m_DepthTarget);
	this->m_pRenderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	this->m_pCamera->SetCameraView(this->m_pRenderView);

	this->m_pRender_text = new LJMUTextOverlay(*this->m_pRenderer11, 
		                                      this->m_RenderTarget, 
											  std::wstring(L"Cambria"), 
											  25);	

	this->m_pCamera->SetOverlayView(this->m_pRender_text);

	this->m_objSpriteCam = new Camera();

	this->m_renderSprite = new LJMUSpriteOverlay(*this->m_pRenderer11,
		                                             this->m_RenderTarget,
		                                   std::wstring(L"CarSprite.png"));

	this->m_objSpriteCam->SetOverlayView(this->m_renderSprite);
	

	this->m_pCamera->SetProjectionParams(0.1f, 1000.0f, m_iscreenWidth / m_iscreenHeight,
		                                   static_cast<float>(GLYPH_PI) / 2.0f);
	
	this->m_pScene->AddCamera(this->m_pCamera);

	this->m_pScene->AddCamera(this->m_objSpriteCam);
}

///////////////////////////////////
// Update the State of our Game and 
// Output the Results to Screen (Render)
/////////////////////////////////// 
void LJMULevelDemo::Update()
{
	this->m_pTimer->Update();
	EvtManager.ProcessEvent(EvtFrameStartPtr( new EvtFrameStart(this->m_pTimer->Elapsed())));

	//----------START RENDERING--------------------------------------------------------------
	this->m_pScene->Update(m_pTimer->Elapsed());
	this->m_pScene->Render(this->m_pRenderer11);

	float tpf = m_pTimer->Elapsed();

	if (tpf > 10.0f / 60.0f)
	{
		//If this is triggered, most likely the user is debugging the code,
		//hence time per frame is much larger than 10x 60fps then
		tpf = 1 / 60.0f;
	}

	m_totalPlayTime += tpf;

	m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_carActor->GetNode()->Position();
	float distanceToNextCheckpoint = m_targetCarDirection.Magnitude();
	m_targetCarDirection.Normalize();

	float angle = acos(m_currentCarDirection.Dot(m_targetCarDirection));
	if (angle > m_carAngularSpeed * tpf)
	{
		Vector3f axis = m_currentCarDirection.Cross(m_targetCarDirection);
		axis.Normalize();

		Matrix3f tRotation;
		tRotation.RotationEuler(axis, m_carAngularSpeed * tpf);
		m_carActor->GetNode()->Rotation() *= tRotation;

		m_currentCarDirection = m_carActor->GetNode()->Rotation() * m_referenceCarDirection;
		m_currentCarDirection.Normalize();
	}

	m_carActor->GetNode()->Position() = m_carActor->GetNode()->Position() + m_currentCarDirection * m_carLinearSpeed * tpf;

	if ((m_checkpoints[m_nextCheckpointIndex] - m_carActor->GetNode()->Position()).Magnitude() > distanceToNextCheckpoint)
	{
		m_currentCheckpointIndex++;
		m_nextCheckpointIndex++;
		m_currentCheckpointIndex = m_currentCheckpointIndex % m_checkpoints.size();
		m_nextCheckpointIndex = m_nextCheckpointIndex % m_checkpoints.size();
	}

	m_carLabelText->GetNode()->Position() = m_carActor->GetNode()->Position() + Vector3f(-5, 20, 0);

	Matrix3f tRotation;
	float rotationSpeed = 2;
	float maxSwayAngle = 10.0f / 360.0f * GLYPH_PI;

	for (int i = 0; i < m_treeTrunkPivots.size(); i++)
	{
		float phaseOffset = (i * GLYPH_PI / m_treeTrunkPivots.size());
		float amplitudeVariation = maxSwayAngle * (0.5f + (static_cast<float>(i) / m_treeTrunkPivots.size()) / 2.0f);

		tRotation.RotationZ(cos(m_totalPlayTime * rotationSpeed + phaseOffset) * amplitudeVariation);
		m_treeTrunkPivots[i]->GetNode()->Rotation() = tRotation;
	}

	if (m_bull1State == 0)
	{
		m_targetBull1Direction = m_bull1Checkpoints[m_nextBull1CheckpointIndex] - m_bull1Actor->GetNode()->Position();
		float bull1DistanceToNextCheckpoint = m_targetBull1Direction.Magnitude();
		m_targetBull1Direction.Normalize();
		m_currentBull1Direction = m_targetBull1Direction;

		if (bull1DistanceToNextCheckpoint > m_bull1DistanceToNextCheckpoint)
		{
			m_currentBull1CheckpointIndex++;
			m_nextBull1CheckpointIndex++;
			m_currentBull1CheckpointIndex = m_currentBull1CheckpointIndex % m_bull1Checkpoints.size();
			m_nextBull1CheckpointIndex = m_nextBull1CheckpointIndex % m_bull1Checkpoints.size();
			m_bull1Actor->GetNode()->Position() = m_bull1Checkpoints[m_currentBull1CheckpointIndex];

			m_targetBull1Direction = m_bull1Checkpoints[m_nextBull1CheckpointIndex] - m_bull1Actor->GetNode()->Position();
			bull1DistanceToNextCheckpoint = m_targetBull1Direction.Magnitude();
			m_targetBull1Direction.Normalize();

			float angle = acos(m_currentBull1Direction.Dot(m_targetBull1Direction));
			if (angle > m_bull1AngularSpeed * tpf)
			{
				m_bull1State = 1;
			}
		}
		else
		{
			m_bull1Actor->GetNode()->Position() = m_bull1Actor->GetNode()->Position() + m_currentBull1Direction * m_bull1LinearSpeed * tpf;
			m_bull1DistanceToNextCheckpoint = bull1DistanceToNextCheckpoint;
		}
	}
	else
	{
		float angle = acos(m_currentBull1Direction.Dot(m_targetBull1Direction));
		if (angle < m_bull1AngularSpeed * tpf)
		{
			m_currentBull1Direction = m_targetBull1Direction;
			float angle = acos(m_referenceBull1Direction.Dot(m_targetBull1Direction));

			Vector3f axis = m_referenceBull1Direction.Cross(m_currentBull1Direction);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, angle);
			m_bull1Actor->GetNode()->Rotation() = tstartRotation;

			m_targetBull1Direction = m_bull1Checkpoints[m_nextBull1CheckpointIndex] - m_bull1Actor->GetNode()->Position();
			m_bull1DistanceToNextCheckpoint = m_targetBull1Direction.Magnitude();

			m_bull1State = 0;
		}
		else
		{
			angle = angle + m_bull1AngularSpeed * tpf;
			Vector3f axis = m_currentBull1Direction.Cross(m_targetBull1Direction);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, m_bull1AngularSpeed * tpf);
			m_bull1Actor->GetNode()->Rotation() *= tstartRotation;
			m_currentBull1Direction = m_bull1Actor->GetNode()->Rotation() * m_referenceBull1Direction;
			m_currentBull1Direction.Normalize();
		}
	}

	if (m_bull2State == 0)
	{
		m_targetBull2Direction = m_bull2Checkpoints[m_nextBull2CheckpointIndex] - m_bull2Actor->GetNode()->Position();
		float bull2DistanceToNextCheckpoint = m_targetBull2Direction.Magnitude();
		m_targetBull2Direction.Normalize();
		m_currentBull2Direction = m_targetBull2Direction;

		if (bull2DistanceToNextCheckpoint > m_bull2DistanceToNextCheckpoint)
		{
			m_currentBull2CheckpointIndex++;
			m_nextBull2CheckpointIndex++;
			m_currentBull2CheckpointIndex = m_currentBull2CheckpointIndex % m_bull2Checkpoints.size();
			m_nextBull2CheckpointIndex = m_nextBull2CheckpointIndex % m_bull2Checkpoints.size();
			m_bull2Actor->GetNode()->Position() = m_bull2Checkpoints[m_currentBull2CheckpointIndex];

			m_targetBull2Direction = m_bull2Checkpoints[m_nextBull2CheckpointIndex] - m_bull2Actor->GetNode()->Position();
			bull2DistanceToNextCheckpoint = m_targetBull2Direction.Magnitude();
			m_targetBull2Direction.Normalize();

			float angle = acos(m_currentBull2Direction.Dot(m_targetBull2Direction));
			if (angle > m_bull2AngularSpeed * tpf)
			{
				m_bull2State = 1;
			}
		}
		else
		{
			m_bull2Actor->GetNode()->Position() = m_bull2Actor->GetNode()->Position() + m_currentBull2Direction * m_bull2LinearSpeed * tpf;
			m_bull2DistanceToNextCheckpoint = bull2DistanceToNextCheckpoint;
		}
	}
	else
	{
		float angle = acos(m_currentBull2Direction.Dot(m_targetBull2Direction));
		if (angle < m_bull2AngularSpeed * tpf)
		{
			m_currentBull2Direction = m_targetBull2Direction;
			float angle = acos(m_referenceBull2Direction.Dot(m_targetBull2Direction));

			Vector3f axis = m_referenceBull2Direction.Cross(m_currentBull2Direction);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, angle);
			m_bull2Actor->GetNode()->Rotation() = tstartRotation;

			m_targetBull2Direction = m_bull2Checkpoints[m_nextBull2CheckpointIndex] - m_bull2Actor->GetNode()->Position();
			m_bull2DistanceToNextCheckpoint = m_targetBull2Direction.Magnitude();

			m_bull2State = 0;
		}
		else
		{
			angle = angle + m_bull2AngularSpeed * tpf;
			Vector3f axis = m_currentBull2Direction.Cross(m_targetBull2Direction);
			axis.Normalize();

			Matrix3f tstartRotation;
			tstartRotation.RotationEuler(axis, m_bull2AngularSpeed * tpf);
			m_bull2Actor->GetNode()->Rotation() *= tstartRotation;
			m_currentBull2Direction = m_bull2Actor->GetNode()->Rotation() * m_referenceBull2Direction;
			m_currentBull2Direction.Normalize();
		}
	}

	updateLightSources();
	applyLightsToAllMaterials();

	changeTime(m_skyboxActor->m_pSkyboxMaterial, m_totalPlayTime);

	//--------2D SPRITE RENDERING-------------------------------------------------------------
	Matrix4f tspritexform = Matrix4f::Identity();
	tspritexform.SetTranslation(Vector3f(10, 20, 0));
	this->m_renderSprite->drawSprite(tspritexform, Vector4f(1, 1, 1, 1));

	//--------HUD TEXT RENDERING-------------------------------------------------------------
	Matrix4f ttextpos = Matrix4f::Identity();
	float tx = 10.0f;
	float ty = 125.0f;
	Vector4f tredclr(1.0f, 0.0f, 0.0f, 1.0f);

	ttextpos.SetTranslation(Vector3f(tx, ty, 0.0f));
	this->m_pRender_text->writeText(this->outputCarPosition(m_carActor), ttextpos, tredclr);

	ty += 30;

	ttextpos.SetTranslation(Vector3f(tx, ty, 0.0f));
	this->m_pRender_text->writeText(this->outputCarOrientation(m_carActor), ttextpos, tredclr);

	ty += 30;

	ttextpos.SetTranslation(Vector3f(tx, ty, 0.0f));
	this->m_pRender_text->writeText(this->outputCarSpeed(m_carActor), ttextpos, tredclr);

	ty += 30;

	ttextpos.SetTranslation(Vector3f(tx, ty, 0.0f));
	this->m_pRender_text->writeText(this->outputFPSInfo(), ttextpos, tredclr);

	tx = this->m_pWindow->GetWidth() - 400.0f;
	ty = 30;
	ttextpos.SetTranslation(Vector3f(tx, ty, 0.0f));

	std::wstring tmatrixtext = L"Car Matrix\n";
	tmatrixtext.append(this->outputCarDebugMatrix(this->m_carActor));
	this->m_pRender_text->writeText(tmatrixtext, ttextpos, tredclr);

	//--------END RENDERING-------------------------------------------------------------
	this->m_pRenderer11->Present(this->m_pWindow->GetHandle(), this->m_pWindow->GetSwapChain());
}

///////////////////////////////////
// Configure the DirectX 11 Programmable
// Pipeline Stages and Create the Window
// Calls 
///////////////////////////////////
bool LJMULevelDemo::ConfigureEngineComponents()
{

	// Set the render window parameters and initialize the window
	this->m_pWindow = new Win32RenderWindow();
	this->m_pWindow->SetPosition(25, 25);
	this->m_pWindow->SetSize(m_iscreenWidth, m_iscreenHeight);
	this->m_pWindow->SetCaption(this->GetName());
	this->m_pWindow->Initialize(this);


	// Create the renderer and initialize it for the desired device
	// type and feature level.
	this->m_pRenderer11 = new RendererDX11();

	if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0))
	{
		Log::Get().Write(L"Could not create hardware device, trying to create the reference device...");

		if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0))
		{
			ShowWindow(this->m_pWindow->GetHandle(), SW_HIDE);
			MessageBox(this->m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device!", 
						L"DirectX11 Racetrack Simulation", MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
			this->RequestTermination();
			return(false);
		}
		// If using the reference device, utilize a fixed time step for any animations.
		this->m_pTimer->SetFixedTimeStep(1.0f / 10.0f);
	}

	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.
	SwapChainConfigDX11 tconfig;
	tconfig.SetWidth(this->m_pWindow->GetWidth());
	tconfig.SetHeight(this->m_pWindow->GetHeight());
	tconfig.SetOutputWindow(this->m_pWindow->GetHandle());
	this->m_iSwapChain = this->m_pRenderer11->CreateSwapChain(&tconfig);
	this->m_pWindow->SetSwapChain(this->m_iSwapChain);
	
	//Create Colour and Depth Buffers
	this->m_RenderTarget = this->m_pRenderer11->GetSwapChainResource(this->m_iSwapChain);

	Texture2dConfigDX11 tdepthconfig;
	tdepthconfig.SetDepthBuffer(m_iscreenWidth, m_iscreenHeight);
	this->m_DepthTarget = this->m_pRenderer11->CreateTexture2D(&tdepthconfig, 0);

	// Bind the swap chain render target and the depth buffer for use in rendering.  
	this->m_pRenderer11->pImmPipeline->ClearRenderTargets();
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, this->m_RenderTarget->m_iResourceRTV);
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState(this->m_DepthTarget->m_iResourceDSV);
	this->m_pRenderer11->pImmPipeline->ApplyRenderTargets();

	D3D11_VIEWPORT tviewport;
	tviewport.Width = static_cast< float >(m_iscreenWidth);
	tviewport.Height = static_cast< float >(m_iscreenHeight);
	tviewport.MinDepth = 0.0f;
	tviewport.MaxDepth = 1.0f;
	tviewport.TopLeftX = 0;
	tviewport.TopLeftY = 0;

	int tvpindex = this->m_pRenderer11->CreateViewPort(tviewport);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState(1);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState(0, tvpindex);
	return(true);
}

//////////////////////////////////
//Handle Input Events in the Application
//////////////////////////////////
bool LJMULevelDemo::HandleEvent(EventPtr pevent)
{
	eEVENT e = pevent->GetEventType();

	if (e == SYSTEM_KEYBOARD_KEYDOWN)
	{
		EvtKeyDownPtr tkey_down = std::static_pointer_cast<EvtKeyDown>(pevent);
		unsigned int  tkeycode = tkey_down->GetCharacterCode();
	}
	else if (e == SYSTEM_KEYBOARD_KEYUP)
	{
		EvtKeyUpPtr tkey_up = std::static_pointer_cast<EvtKeyUp>(pevent);
		unsigned int tkeycode = tkey_up->GetCharacterCode();
	}

	return(Application::HandleEvent(pevent));
}

//////////////////////////////////
// Destroy Resources created by the engine
//////////////////////////////////
void LJMULevelDemo::ShutdownEngineComponents()
{
	if (this->m_pRenderer11)
	{
		this->m_pRenderer11->Shutdown();
		delete this->m_pRenderer11;
	}

	if (this->m_pWindow)
	{
		this->m_pWindow->Shutdown();
		delete this->m_pWindow;
	}
}

//////////////////////////////////
// Shutdown the Application
//////////////////////////////////
void LJMULevelDemo::Shutdown()
{
	//NOTHING TO DO HERE
}

//////////////////////////////////
// Take a Screenshot of the Application
//////////////////////////////////
void LJMULevelDemo::TakeScreenShot()
{
	if (this->m_bSaveScreenshot)
	{
		this->m_bSaveScreenshot = false;
		this->m_pRenderer11->pImmPipeline->SaveTextureScreenShot(0, this->GetName());
	}
}

//////////////////////////////////////
// Output our Frame Rate
//////////////////////////////////////
std::wstring LJMULevelDemo::outputFPSInfo()
{
	std::wstringstream out;
	out << L"FPS: " << m_pTimer->Framerate();
	return out.str();
}

//////////////////////////////////////
// Output our Car Information
//////////////////////////////////////
std::wstring LJMULevelDemo::outputCarPosition(Actor* pactor)
{
	std::wstringstream out;
	Matrix4f twm = pactor->GetNode()->WorldMatrix();

		out << std::fixed << std::setprecision(0);
	    out << L"Car's Position: (" << twm[Matrix4f::m41] << L", "
		<< twm[Matrix4f::m42] << L", "
		<< twm[Matrix4f::m43] << L")" << std::endl;

	return out.str();
}

std::wstring LJMULevelDemo::outputCarOrientation(Actor* pactor)
{
	std::wstringstream out;
	Matrix4f twm = pactor->GetNode()->WorldMatrix();

	float forwardX = twm[Matrix4f::m31]; 
	float forwardZ = twm[Matrix4f::m33];

	out << std::fixed << std::setprecision(2);
	out << L"Car's Orientation: " << forwardX << " " << forwardZ << "" << std::endl;

	return out.str();
}

std::wstring LJMULevelDemo::outputCarSpeed(Actor* pactor)
{
	std::wstringstream out;

	out << std::fixed << std::setprecision(0);
	out << L"Car's Speed: " << m_carLinearSpeed << std::endl;

	return out.str();
}

std::wstring LJMULevelDemo::outputCarDebugMatrix(Actor* pactor)
{
	std::wstringstream out;
	Matrix4f twm = pactor->GetNode()->WorldMatrix();

	out << std::fixed << std::setprecision(2);
	out << std::setw(10) << twm[Matrix4f::m11] << std::setw(10) << twm[Matrix4f::m12]
		<< std::setw(10) << twm[Matrix4f::m13] << std::setw(10) << twm[Matrix4f::m14] << std::endl;
	out << std::setw(10) << twm[Matrix4f::m21] << std::setw(10) << twm[Matrix4f::m22]
		<< std::setw(10) << twm[Matrix4f::m23] << std::setw(10) << twm[Matrix4f::m24] << std::endl;
	out << std::setw(10) << twm[Matrix4f::m31] << std::setw(10) << twm[Matrix4f::m32]
		<< std::setw(10) << twm[Matrix4f::m33] << std::setw(10) << twm[Matrix4f::m34] << std::endl;
	out << std::setw(10) << twm[Matrix4f::m41] << std::setw(10) << twm[Matrix4f::m42]
		<< std::setw(10) << twm[Matrix4f::m43] << std::setw(10) << twm[Matrix4f::m44] << std::endl;
	return out.str();
}

void LJMULevelDemo::setupLightSources()
{
	//Directional Light Properties
	Vector3f directionalLightDir = Vector3f(0.0f, -0.5f, 0.5f);
	directionalLightDir.Normalize();
	m_DirectionalLightDirection = Vector4f(directionalLightDir, 1.0f);

	//Point Light Properties
	m_PointLightPosition = Vector4f(170.0f, 30.0f, 0.0f, 0.0f);
	m_PointLightColour = Vector4f(1.0f, 0.1f, 0.1f, 1.0f);
	m_PointLightRange = Vector4f(40.0f, 0.0f, 100.0f, 0.0f);
	//The range of the point light source is set to 60.0
	//We only use the first component of the 4D vector because
	//we cannot just send a scalar number to the GPU

	//Spot Light Properties
	m_SpotLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLightRange = Vector4f(150.0f, 0.0f, 0.0f, 0.0f);
	m_SpotLightFocus = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);
	//The focus of the spot light source is set to 20.0
	//We can only use the first component of the 4D vector because
	//we cannot just send a scalar number to the GPU
}

void LJMULevelDemo::changeTime(MaterialPtr material, float totalPlayTime)
{
	m_skyboxTimeValue = sin(totalPlayTime * 0.3f) + 1.0f;
	m_skyboxTimeValue = m_skyboxTimeValue / 2.0f;

	Vector4f time = Vector4f(m_skyboxTimeValue, 0.0f, 0.0f, 0.0f);
	material->Parameters.SetVectorParameter(L"timex", time);
}

void LJMULevelDemo::setLightsToMaterial(MaterialPtr material)
{
	material->Parameters.SetVectorParameter(L"DirectionalLightColour", m_DirectionalLightColour);
	material->Parameters.SetVectorParameter(L"DirectionalLightDirection", m_DirectionalLightDirection);

	material->Parameters.SetVectorParameter(L"SpotLightColour", m_SpotLightColour);
	material->Parameters.SetVectorParameter(L"SpotLightDirection", m_SpotLightDirection);
	material->Parameters.SetVectorParameter(L"SpotLightPosition", m_SpotLightPosition);
	material->Parameters.SetVectorParameter(L"SpotLightRange", m_SpotLightRange);
	material->Parameters.SetVectorParameter(L"SpotLightFocus", m_SpotLightFocus);

	material->Parameters.SetVectorParameter(L"PointLightColour", m_PointLightColour);
	material->Parameters.SetVectorParameter(L"PointLightPosition", m_PointLightPosition);
	material->Parameters.SetVectorParameter(L"PointLightRange", m_PointLightRange);
}

MaterialPtr LJMULevelDemo::setupMaterialProperties(MaterialPtr material)
{
	float ambient_constant = 0.005f;
	float diffuse_constant = 4.0f;
	float specular_constant = 0.1f;
	float shininess_constant = 200.0f;
	Vector4f emmisivity = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", Vector4f(
		ambient_constant,
		diffuse_constant,
		specular_constant,
		shininess_constant));

	material->Parameters.SetVectorParameter(L"SurfaceEmmisiveColour", emmisivity);
	setLightsToMaterial(material);
	return material;
}

void LJMULevelDemo::updateLightSources()
{
	Vector3f CarForwardDirection = m_currentCarDirection;
	CarForwardDirection.y = -0.1f;
	CarForwardDirection.Normalize();
	m_SpotLightDirection = Vector4f(CarForwardDirection, 1.0f);

	Vector3f CarPosition = m_carActor->GetNode()->Position();
	m_SpotLightPosition = Vector4f(CarPosition, 1.0f);

	float intensityFactor = 1.0f - m_skyboxTimeValue;
	float scalingFactor = 0.5f;
	intensityFactor *= scalingFactor;
	m_DirectionalLightColour = Vector4f(intensityFactor, intensityFactor, intensityFactor, 1.0f);
}

void LJMULevelDemo::applyLightsToAllMaterials()
{
	MaterialPtr material = m_platformActor->GetBody()->GetMaterial();
	setLightsToMaterial(material);
	m_platformActor->GetBody()->SetMaterial(material);

	for (int i = 0; i < m_treeTrunks.size(); i++)
	{
		material = m_treeTrunks[i]->GetBody()->GetMaterial();
		setLightsToMaterial(material);
		m_treeTrunks[i]->GetBody()->SetMaterial(material);
	}

	for (int i = 0; i < m_treeTops.size(); i++)
	{
		material = m_treeTops[i]->GetBody()->GetMaterial();
		setLightsToMaterial(material);
		m_treeTops[i]->GetBody()->SetMaterial(material);
	}

	material = m_carActor->GetBody()->GetMaterial();
	setLightsToMaterial(material);
	m_carActor->GetBody()->SetMaterial(material);

	material = m_bull1Actor->GetBody()->GetMaterial();
	setLightsToMaterial(material);
	m_bull1Actor->GetBody()->SetMaterial(material);

	material = m_bull2Actor->GetBody()->GetMaterial();
	setLightsToMaterial(material);
	m_bull2Actor->GetBody()->SetMaterial(material);

	material = m_flagpoleActor->GetBody()->GetMaterial();
	setLightsToMaterial(material);
	m_flagpoleActor->GetBody()->SetMaterial(material);

	material = m_flagActor->GetBody()->GetMaterial();
	setLightsToMaterial(material);
	m_flagActor->GetBody()->SetMaterial(material);
}
