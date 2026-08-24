///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

/***********************************************************
 *  LoadSceneTextures()
 *
 *  This function loads all textures for the scene.
 ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;

	// Load the table texture
	bReturn = CreateGLTexture(
		"wood.jpg",
		"wood");

	// Load the cake texture
	bReturn = CreateGLTexture(
		"cake.jpg",
		"cake");

	// Load the cream texture
	bReturn = CreateGLTexture(
		"cream.jpg",
		"cream");

	// Load the almond texture
	bReturn = CreateGLTexture(
		"almond.jpg",
		"almond");

	// Load the vase texture
	bReturn = CreateGLTexture(
		"vase.jpeg",
		"vase");

	// Bind all textures
	BindGLTextures();
}


void SceneManager::DefineObjectMaterials()
{
	// Wood material for the table
	OBJECT_MATERIAL woodMaterial;

	woodMaterial.ambientColor = glm::vec3(0.25f, 0.12f, 0.05f);
	woodMaterial.ambientStrength = 0.25f;
	woodMaterial.diffuseColor = glm::vec3(0.55f, 0.30f, 0.12f);
	woodMaterial.specularColor = glm::vec3(0.20f, 0.12f, 0.08f);
	woodMaterial.shininess = 10.0f;
	woodMaterial.tag = "wood";

	// Add wood material
	m_objectMaterials.push_back(woodMaterial);


	// Light rose material for the cake
	OBJECT_MATERIAL cakeMaterial;

	cakeMaterial.ambientColor = glm::vec3(0.30f, 0.08f, 0.12f);
	cakeMaterial.ambientStrength = 0.25f;
	cakeMaterial.diffuseColor = glm::vec3(0.90f, 0.35f, 0.50f);
	cakeMaterial.specularColor = glm::vec3(0.50f, 0.25f, 0.30f);
	cakeMaterial.shininess = 20.0f;
	cakeMaterial.tag = "cake";

	// Add cake material
	m_objectMaterials.push_back(cakeMaterial);


	// Cream material for the frosting
	OBJECT_MATERIAL creamMaterial;

	creamMaterial.ambientColor = glm::vec3(0.30f, 0.27f, 0.22f);
	creamMaterial.ambientStrength = 0.30f;
	creamMaterial.diffuseColor = glm::vec3(0.90f, 0.82f, 0.70f);
	creamMaterial.specularColor = glm::vec3(0.60f, 0.55f, 0.50f);
	creamMaterial.shininess = 25.0f;
	creamMaterial.tag = "cream";

	// Add cream material
	m_objectMaterials.push_back(creamMaterial);


	// Almond material
	OBJECT_MATERIAL almondMaterial;

	almondMaterial.ambientColor = glm::vec3(0.25f, 0.15f, 0.06f);
	almondMaterial.ambientStrength = 0.25f;
	almondMaterial.diffuseColor = glm::vec3(0.65f, 0.42f, 0.18f);
	almondMaterial.specularColor = glm::vec3(0.30f, 0.20f, 0.10f);
	almondMaterial.shininess = 15.0f;
	almondMaterial.tag = "almond";

	// Add almond material
	m_objectMaterials.push_back(almondMaterial);

	// Light elephant-bone ceramic material
	OBJECT_MATERIAL plateMaterial;

	plateMaterial.ambientColor = glm::vec3(0.55f, 0.52f, 0.45f);
	plateMaterial.ambientStrength = 0.30f;

	plateMaterial.diffuseColor = glm::vec3(0.82f, 0.80f, 0.70f);

	plateMaterial.specularColor = glm::vec3(0.45f, 0.43f, 0.37f);
	plateMaterial.shininess = 25.0f;

	plateMaterial.tag = "plate";

	m_objectMaterials.push_back(plateMaterial);
}


void SceneManager::SetupSceneLights()
{
	// White light above
	m_pShaderManager->setVec3Value("lightSources[0].position", -3.0f, 10.0f, 3.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.01f, 0.01f, 0.01f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 0.45f, 0.45f, 0.45f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.05f);

	// Blue light on the left
	m_pShaderManager->setVec3Value("lightSources[1].position", -3.0f, 6.0f, 3.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.01f, 0.01f, 0.01f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.0f, 0.08f, 0.20f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 20.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.3f);

	// Green light on the right
	m_pShaderManager->setVec3Value("lightSources[2].position", 6.0f, 6.0f, 3.0f);
	m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.01f, 0.01f, 0.01f);
	m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.05f, 0.20f, 0.06f);
	m_pShaderManager->setVec3Value("lightSources[2].specularColor", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 20.0f);
	m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.3f);

	// Rose light in front
	m_pShaderManager->setVec3Value("lightSources[3].position", 2.0f, 5.0f, 7.0f);
	m_pShaderManager->setVec3Value("lightSources[3].ambientColor", 0.01f, 0.01f, 0.01f);
	m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", 0.20f, 0.04f, 0.08f);
	m_pShaderManager->setVec3Value("lightSources[3].specularColor", 0.3f, 0.2f, 0.2f);
	m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 16.0f);
	m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.10f);

	// Turn lighting on
	m_pShaderManager->setBoolValue("bUseLighting", true);
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// Load all textures
	LoadSceneTextures();
	// Load materials
	DefineObjectMaterials();
	// Load lights
	SetupSceneLights();


	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTaperedCylinderMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadTorusMesh(0.12f);
	m_basicMeshes->LoadConeMesh();

}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 0.50f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// use the wood texture
	SetShaderTexture("wood");

	// use the wood material
	SetShaderMaterial("wood");

	// repeat the texture
	SetTextureUVScale(6.0f, 6.0f);

	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();

	// ----------------------
	// Cake Body
	// ----------------------

	// set the scale
	scaleXYZ = glm::vec3(2.5f, 1.5f, 2.5f);

	// set the rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the position
	positionXYZ = glm::vec3(3.5f, 0.65f, 3.0f);

	// apply the transformations
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// use the cake texture
	SetShaderTexture("cake");

	// set the material
	SetShaderMaterial("cake");

	// keep the normal texture size
	SetTextureUVScale(1.0f, 1.0f);

	// draw the cake body
	m_basicMeshes->DrawTaperedCylinderMesh();

	// ----------------------
	// Cake Frosting
	// ----------------------

	// set the scale
	scaleXYZ = glm::vec3(2.0f, 1.0f, 2.80f);

	// set the rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the position
	positionXYZ = glm::vec3(3.5f, 1.50f, 3.0f);

	// apply the transformations
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// use the cream texture
	SetShaderTexture("cream");

	// set the material
	SetShaderMaterial("cream");

	// keep the normal texture size
	SetTextureUVScale(1.0f, 1.0f);


	// draw the cake frosting
	m_basicMeshes->DrawTaperedCylinderMesh();

	// ----------------------
	// Almond Slice 1
	// ----------------------

	// set the scale
	scaleXYZ = glm::vec3(0.22f, 0.10f, 0.30f);

	// apply the transformations
	SetTransformations(
		scaleXYZ,
		0.0f,
		35.0f,
		0.0f,
		glm::vec3(4.0f, 2.5f, 4.0f));

	// use the almond texture
	SetShaderTexture("almond");

	// set the material
	SetShaderMaterial("almond");

	// keep the normal texture size
	SetTextureUVScale(1.0f, 1.0f);

	// draw the almond slice
	m_basicMeshes->DrawSphereMesh();

	// ----------------------
	// Almond Slice 2
	// ----------------------

	// set the scale
	scaleXYZ = glm::vec3(0.22f, 0.12f, 0.3f);

	// apply the transformations
	SetTransformations(
		scaleXYZ,
		0.0f,
		-25.0f,
		10.0f,
		glm::vec3(3.10f, 2.55f, 4.10f));

	// use the almond texture
	SetShaderTexture("almond");

	// set the material
	SetShaderMaterial("almond");

	// keep the normal texture size
	SetTextureUVScale(1.0f, 1.0f);

	// draw the almond slice
	m_basicMeshes->DrawSphereMesh();

	// ----------------------
	// Almond Slice 3
	// ----------------------

	// set the scale
	scaleXYZ = glm::vec3(0.22f, 0.08f, 0.25f);

	// apply the transformations
	SetTransformations(
		scaleXYZ,
		0.0f,
		75.0f,
		-5.0f,
		glm::vec3(2.80f, 2.6f, 3.20f));

	// use the almond texture
	SetShaderTexture("almond");

	// set the material
	SetShaderMaterial("almond");

	// keep the normal texture size
	SetTextureUVScale(1.0f, 1.0f);

	// draw the almond slice
	m_basicMeshes->DrawSphereMesh();

	// ----------------------
	// Almond Slice 4
	// ----------------------

	// set the scale
	scaleXYZ = glm::vec3(0.22f, 0.10f, 0.30f);

	// apply the transformations
	SetTransformations(
		scaleXYZ,
		0.0f,
		-15.0f,
		5.0f,
		glm::vec3(3.80f, 2.65f, 3.25f));

	// use the almond texture
	SetShaderTexture("almond");

	// set the material
	SetShaderMaterial("almond");

	// keep the normal texture size
	SetTextureUVScale(1.0f, 1.0f);

	// draw the almond slice
	m_basicMeshes->DrawSphereMesh();

	// ----------------------
	// Oval Plate
	// ----------------------

	// set the size of the plate
	scaleXYZ = glm::vec3(2.3f, 0.35f, 1.6f);

	// set the position and rotation of the plate
	SetTransformations(
		scaleXYZ,
		180.0f,
		0.0f,
		0.0f,
		glm::vec3(-3.0f, 1.05f, 4.0f)
	);

	// set the plate color
	SetShaderColor(
		0.88f,
		0.84f,
		0.74f,
		1.0f
	);

	// draw the plate using a tapered cylinder
	m_basicMeshes->DrawTaperedCylinderMesh();

	// ----------------------
	// Cup Body
	// ----------------------

	// set the size of the cup
	scaleXYZ = glm::vec3(0.75f, 0.9f, 0.75f);
	
	// set the position and rotation of the cup
	SetTransformations(
		scaleXYZ,
		0.0f,
		0.0f,
		0.0f,
		glm::vec3(-2.95f, 1.10f, 4.0f)
	);

	// set the cup color
	SetShaderColor(
		0.88f,
		0.84f,
		0.74f,
		1.0f
	);

	// draw cup
	m_basicMeshes->DrawCylinderMesh();

	// ----------------------
	// Cup Handle
	// ----------------------

	// set the size
	scaleXYZ = glm::vec3(0.25f, 0.25f, 0.05f);

	// set the position of the cup handle
	SetTransformations(
		scaleXYZ,
		0.0f,
		0.0f,
		-90.0f,
		glm::vec3(-2.15f, 1.6f, 4.0f)
	);

	// set the cup handle color
	SetShaderColor(0.85f, 0.80f, 0.70f, 1.0f);

	// draw the cup handle
	m_basicMeshes->DrawHalfTorusMesh();

	// ----------------------
	// Vase
	// ----------------------

	// set the size of the vase
	scaleXYZ = glm::vec3(2.0f, 2.5f, 2.0f);

	// set the vase position and rotation
	SetTransformations(
		scaleXYZ,
		0.0f,
		0.0f,
		0.0f,
		glm::vec3(-1.0f, 0.6f, 0.0f)
	);

	// use the vase texture
	SetShaderTexture("vase");

	// keep the texture at its original scale
	SetTextureUVScale(1.0f, 1.0f);

	// set the vase color
	m_basicMeshes->DrawConeMesh(true);

	// ----------------------
	// Vase Neck - Cylinder
	// ----------------------

	// make the neck smaller than the vase
	scaleXYZ = glm::vec3(0.55f, 0.8f, 0.55f);

	// place the neck above the vase
	SetTransformations(
		scaleXYZ,
		0.0f,
		0.0f,
		0.0f,
		glm::vec3(-1.0f, 2.3f, 0.0f)
	);
	
	// use the same vase texture on the neck
	SetShaderTexture("vase");

	// keep the texture at normal size
	SetTextureUVScale(1.0f, 1.0f);

	// draw the vase neck
	m_basicMeshes->DrawCylinderMesh();

	// ----------------------
	// Tulip Flower
	// ----------------------

	// make the flower round and slightly tall
	scaleXYZ = glm::vec3(0.55f, 0.65f, 0.55f);

	// place the flower at the top of the stem
	SetTransformations(
		scaleXYZ,
		0.0f,
		0.0f,
		0.0f,
		glm::vec3(-1.0f, 4.95f, 0.0f)
	);

	// set the flower color
	SetShaderColor(
		0.85f,
		0.12f,
		0.20f,
		1.0f
	);

	// draw the flower using a sphere
	m_basicMeshes->DrawSphereMesh();

	// ----------------------
	// Tulip Stem
	// ----------------------

	// make the stem thin and tall
	scaleXYZ = glm::vec3(0.06f, 1.4f, 0.06f);

	// place the stem inside the vase
	SetTransformations(
		scaleXYZ,
		0.0f,
		0.0f,
		0.0f,
		glm::vec3(-1.0f, 3.0f, 0.0f)
	);

	// set the stem color
	SetShaderColor(
		0.15f,
		0.45f,
		0.12f,
		1.0f
	);

	// draw the stem using a cylinder
	m_basicMeshes->DrawCylinderMesh();

	// ----------------------
	// Tulip Leaf 1
	// ----------------------

	// make the leaf thin and long
	scaleXYZ = glm::vec3(0.12f, 0.70f, 0.30f);

	// rotate the leaf so the bottom is to the right
	// and the top points to the left
	SetTransformations(
		scaleXYZ,
		0.0f,      // X rotation
		0.0f,      // Y rotation
		-35.0f,    // Z rotation: bottom right, top left
		glm::vec3(-0.6f, 3.8f, 0.0f)
	);

	// set the leaf color
	SetShaderColor(
		0.15f,
		0.55f,
		0.18f,
		1.0f
	);

	// draw the leaf using a sphere
	m_basicMeshes->DrawSphereMesh();
}
