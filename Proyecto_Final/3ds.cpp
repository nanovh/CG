#include "main.h"
#include "3ds.h"
#include <assert.h>

int gBuffer[50000] = {0};					// This is used to read past unwanted data

// This file handles all of the code needed to load a .3DS file.
// Basically, how it works is, you load a chunk, then you check
// the chunk ID.  Depending on the chunk ID, you load the information
// that is stored in that chunk.  If you do not want to read that information,
// you read past it.  You know how many bytes to read past the chunk because
// every chunk stores the length in bytes of that chunk.

///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This constructor initializes the tChunk data
/////
///////////////////////////////// CLOAD3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CLoad3DS::CLoad3DS()
{
	m_FilePointer = NULL;
}


///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is called by the client to open the .3ds file, read it, then clean up
/////
///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CLoad3DS::Import3DS(t3DModel *pModel, char *strFileName)
{
	char strMessage[255] = {0};
	tChunk currentChunk = {0};

	// Open the 3DS file
	m_FilePointer = fopen(strFileName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!m_FilePointer) 
	{
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Once we have the file open, we need to read the very first data chunk
	// to see if it's a 3DS file.  That way we don't read an invalid file.
	// If it is a 3DS file, then the first chunk ID will be equal to PRIMARY (some hex num)

	// Read the first chuck of the file to see if it's a 3DS file
	ReadChunk(&currentChunk);

	// Make sure this is a 3DS file
	if (currentChunk.ID != PRIMARY)
	{
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Now we actually start reading in the data.  ProcessNextChunk() is recursive

	// Begin loading objects, by calling this recursive function
	ProcessNextChunk(pModel, &currentChunk);

	// After we have read the whole 3DS file, we want to calculate our own vertex normals.
	ComputeNormals(pModel);

	// Clean up after everything
	CleanUp();

	return true;
}

///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::CleanUp()
{
	if (m_FilePointer) {
		fclose(m_FilePointer);					// Close the current file pointer
		m_FilePointer = NULL;
	}
}


///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads the main sections of the .3DS file, then dives deeper with recursion
/////
///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	t3DObject newObject = {0};					// This is used to add to our object list
	tMaterialInfo newTexture = {0};				// This is used to add to our material list

	tChunk currentChunk = {0};					// The current chunk to load
	tChunk tempChunk = {0};						// A temp chunk for holding data		

	// Below we check our chunk ID each time we read a new chunk.  Then, if
	// we want to extract the information from that chunk, we do so.
	// If we don't want a chunk, we just read past it.  

	// Continue to read the sub chunks until we have reached the length.
	// After we read ANYTHING we add the bytes read to the chunk and then check
	// check against the length.
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read next Chunk
		ReadChunk(&currentChunk);

		// Check the chunk ID
		switch (currentChunk.ID)
		{
		case VERSION:							// This holds the version of the file
			
			// If the file was made in 3D Studio Max, this chunk has an int that 
			// holds the file version.  Since there might be new additions to the 3DS file
			// format in 4.0, we give a warning to that problem.
			// However, if the file wasn't made by 3D Studio Max, we don't 100% what the
			// version length will be so we'll simply ignore the value

			// Read the file version and add the bytes read to our bytesRead variable
			currentChunk.bytesRead += fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);

			// If the file version is over 3, give a warning that there could be a problem
			if ((currentChunk.length - currentChunk.bytesRead == 4) && (gBuffer[0] > 0x03)) {
				MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly", "Warning", MB_OK);
			}
			break;

		case OBJECTINFO:						// This holds the version of the mesh
			{	
			// This chunk holds the version of the mesh.  It is also the head of the MATERIAL
			// and OBJECT chunks.  From here on we start reading in the material and object info.

			// Read the next chunk
			ReadChunk(&tempChunk);

			// Get the version of the mesh
			tempChunk.bytesRead += fread(gBuffer, 1, tempChunk.length - tempChunk.bytesRead, m_FilePointer);

			// Increase the bytesRead by the bytes read from the last chunk
			currentChunk.bytesRead += tempChunk.bytesRead;

			// Go to the next chunk, which is the object has a texture, it should be MATERIAL, then OBJECT.
			ProcessNextChunk(pModel, &currentChunk);
			break;
		}
		case MATERIAL:							// This holds the material information

			// This chunk is the header for the material info chunks

			// Increase the number of materials
			pModel->numOfMaterials++;

			// Add a empty texture structure to our texture list.
			// If you are unfamiliar with STL's "vector" class, all push_back()
			// does is add a new node onto the list.  I used the vector class
			// so I didn't need to write my own link list functions.  
			pModel->pMaterials.push_back(newTexture);

			// Proceed to the material loading function
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case OBJECT:							// This holds the name of the object being read
				
			// This chunk is the header for the object info chunks.  It also
			// holds the name of the object.

			// Increase the object count
			pModel->numOfObjects++;
		
			// Add a new tObject node to our list of objects (like a link list)
			pModel->pObject.push_back(newObject);
			
			// Initialize the object and all it's data members
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(t3DObject));

			// Get the name of the object and store it, then add the read bytes to our byte counter.
			currentChunk.bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);
			
			// Now proceed to read in the rest of the object information
			ProcessNextObjectChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), &currentChunk);
			break;

		case EDITKEYFRAME:

			// Because I wanted to make this a SIMPLE tutorial as possible, I did not include
			// the key frame information.  This chunk is the header for all the animation info.
			// In a later tutorial this will be the subject and explained thoroughly.

			//ProcessNextKeyFrameChunk(pModel, currentChunk);

			// Read past this chunk and add the bytes read to the byte counter
			currentChunk.bytesRead += fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			break;

		default: 
			
			// If we didn't care about a chunk, then we get here.  We still need
			// to read past the unknown or ignored chunk and add the bytes read to the byte counter.
			currentChunk.bytesRead += fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}


///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles all the information about the objects in the file
/////
///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	// The current chunk to work with
	tChunk currentChunk = {0};

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(&currentChunk);

		// Check which chunk we just read
		switch (currentChunk.ID)
		{
		case OBJECT_MESH:					// This lets us know that we are reading a new object
		
			// We found a new object, so let's read in it's info using recursion
			ProcessNextObjectChunk(pModel, pObject, &currentChunk);
			break;

		case OBJECT_VERTICES:				// This is the objects vertices
			ReadVertices(pObject, &currentChunk);
			break;

		case OBJECT_FACES:					// This is the objects face information
			ReadVertexIndices(pObject, &currentChunk);
			break;

		case OBJECT_MATERIAL:				// This holds the material name that the object has
			
			// This chunk holds the name of the material that the object has assigned to it.
			// This could either be just a color or a texture map.  This chunk also holds
			// the faces that the texture is assigned to (In the case that there is multiple
			// textures assigned to one object, or it just has a texture on a part of the object.
			// Since most of my game objects just have the texture around the whole object, and 
			// they aren't multitextured, I just want the material name.

			// We now will read the name of the material assigned to this object
			ReadObjectMaterial(pModel, pObject, &currentChunk);			
			break;

		case OBJECT_UV:						// This holds the UV texture coordinates for the object

			// This chunk holds all of the UV coordinates for our object.  Let's read them in.
			ReadUVCoordinates(pObject, &currentChunk);
			break;

		default:  

			// Read past the ignored or unknown chunks
			currentChunk.bytesRead += fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}


///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles all the information about the material (Texture)
/////
///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ProcessNextMaterialChunk(t3DModel *pModel, tChunk *pPreviousChunk)
{
	// The current chunk to work with
	tChunk currentChunk = {0};

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(&currentChunk);

		// Check which chunk we just read in
		switch (currentChunk.ID)
		{
		case MATNAME:							// This chunk holds the name of the material
			
			// Here we read in the material name
			currentChunk.bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			break;

		case MATDIFFUSE:						// This holds the R G B color of our object
			ReadColorChunk(&(pModel->pMaterials[pModel->numOfMaterials - 1]), &currentChunk);
			break;
		
		case MATMAP:							// This is the header for the texture info
			
			// Proceed to read in the material information
			ProcessNextMaterialChunk(pModel, &currentChunk);
			break;

		case MATMAPFILE:						// This stores the file name of the material

			// Here we read in the material's file name
			currentChunk.bytesRead += fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			break;
		
		default:  

			// Read past the ignored or unknown chunks
			currentChunk.bytesRead += fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += currentChunk.bytesRead;
	}
}

///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in a chunk ID and it's length in bytes
/////
///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadChunk(tChunk *pChunk)
{
	// This reads the chunk ID which is 2 bytes.
	// The chunk ID is like OBJECT or MATERIAL.  It tells what data is
	// able to be read in within the chunks section.  
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, m_FilePointer);

	// Then, we read the length of the chunk which is 4 bytes.
	// This is how we know how much to read in, or read past.
	pChunk->bytesRead += fread(&pChunk->length, 1, 4, m_FilePointer);
}

///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in a string of characters
/////
///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CLoad3DS::GetString(char *pBuffer)
{
	int index = 0;

	// Read 1 byte of data which is the first letter of the string
	fread(pBuffer, 1, 1, m_FilePointer);

	// Loop until we get NULL
	while (*(pBuffer + index++) != 0) {

		// Read in a character at a time until we hit NULL.
		fread(pBuffer + index, 1, 1, m_FilePointer);
	}

	// Return the string length, which is how many bytes we read in (including the NULL)
	return strlen(pBuffer) + 1;
}


///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the RGB color data
/////
///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk)
{
	tChunk tempChunk = {0};

	// Read the color chunk info
	ReadChunk(&tempChunk);

	// Read in the R G B color (3 bytes - 0 through 255)
	tempChunk.bytesRead += fread(pMaterial->color, 1, tempChunk.length - tempChunk.bytesRead, m_FilePointer);

	// Add the bytes read to our chunk
	pChunk->bytesRead += tempChunk.bytesRead;
}


///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the indices for the vertex array
/////
///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertexIndices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;					// This is used to read in the current face index

	// In order to read in the vertex indices for the object, we need to first
	// read in the number of them, then read them in.  Remember,
	// we only want 3 of the 4 values read in for each face.  The fourth is
	// a visibility flag for 3D Studio Max that doesn't mean anything to us.

	// Read in the number of faces that are in this object (int)
	pPreviousChunk->bytesRead += fread(&pObject->numOfFaces, 1, 2, m_FilePointer);

	// Alloc enough memory for the faces and initialize the structure
	pObject->pFaces = new tFace [pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(tFace) * pObject->numOfFaces);

	// Go through all of the faces in this object
	for(int i = 0; i < pObject->numOfFaces; i++)
	{
		// Next, we read in the A then B then C index for the face, but ignore the 4th value.
		// The fourth value is a visibility flag for 3D Studio Max, we don't care about this.
		for(int j = 0; j < 4; j++)
		{
			// Read the first vertice index for the current face 
			pPreviousChunk->bytesRead += fread(&index, 1, sizeof(index), m_FilePointer);

			if(j < 3)
			{
				// Store the index in our face structure.
				pObject->pFaces[i].vertIndex[j] = index;
			}
		}
	}
}


///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the UV coordinates for the object
/////
///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadUVCoordinates(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// In order to read in the UV indices for the object, we need to first
	// read in the amount there are, then read them in.

	// Read in the number of UV coordinates there are (int)
	pPreviousChunk->bytesRead += fread(&pObject->numTexVertex, 1, 2, m_FilePointer);

	// Allocate memory to hold the UV coordinates
	pObject->pTexVerts = new CVector2 [pObject->numTexVertex];

	// Read in the texture coodinates (an array 2 float)
	pPreviousChunk->bytesRead += fread(pObject->pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}


///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the vertices for the object
/////
///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadVertices(t3DObject *pObject, tChunk *pPreviousChunk)
{
	// Like most chunks, before we read in the actual vertices, we need
	// to find out how many there are to read in.  Once we have that number
	// we then fread() them into our vertice array.

	// Read in the number of vertices (int)
	pPreviousChunk->bytesRead += fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);

	// Allocate the memory for the verts and initialize the structure
	pObject->pVerts = new CVector3 [pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(CVector3) * pObject->numOfVerts);

	// Read in the array of vertices (an array of 3 floats)
	pPreviousChunk->bytesRead += fread(pObject->pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);

	// Now we should have all of the vertices read in.  Because 3D Studio Max
	// Models with the Z-Axis pointing up (strange and ugly I know!), we need
	// to flip the y values with the z values in our vertices.  That way it
	// will be normal, with Y pointing up.  If you prefer to work with Z pointing
	// up, then just delete this next loop.  Also, because we swap the Y and Z
	// we need to negate the Z to make it come out correctly.

	// Go through all of the vertices that we just read and swap the Y and Z values
	for(int i = 0; i < pObject->numOfVerts; i++)
	{
		// Store off the Y value
		float fTempY = pObject->pVerts[i].y;

		// Set the Y value to the Z value
		pObject->pVerts[i].y = pObject->pVerts[i].z;

		// Set the Z value to the Y value, 
		// but negative Z because 3D Studio max does the opposite.
		pObject->pVerts[i].z = -fTempY;
	}
}


///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the material name assigned to the object and sets the materialID
/////
///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk)
{
	char strMaterial[255] = {0};			// This is used to hold the objects material name

	// *What is a material?*  - A material is either the color or the texture map of the object.
	// It can also hold other information like the brightness, shine, etc... Stuff we don't
	// really care about.  We just want the color, or the texture map file name really.

	// Here we read the material name that is assigned to the current object.
	// strMaterial should now have a string of the material name, like "Material #2" etc..
	pPreviousChunk->bytesRead += GetString(strMaterial);

	// Now that we have a material name, we need to go through all of the materials
	// and check the name against each material.  When we find a material in our material
	// list that matches this name we just read in, then we assign the materialID
	// of the object to that material index.  You will notice that we passed in the
	// model to this function.  This is because we need the number of textures.
	// Yes though, we could have just passed in the model and not the object too.

	// Go through all of the textures
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		// If the material we just read in matches the current texture name
		if(strcmp(strMaterial, pModel->pMaterials[i].strName) == 0)
		{
			// Now that we found the material, check if it's a texture map.
			// If the strFile has a string length of 1 and over it's a texture
			if(strlen(pModel->pMaterials[i].strFile) > 0) 
			{				
				// Set the material ID to the current index 'i' and stop checking
				pObject->materialID = i;

				// Set the object's flag to say it has a texture map to bind.
				pObject->bHasTexture = true;
			}	
			break;
		}
		else
		{
			// Here we check first to see if there is a texture already assigned to this object
			if(pObject->bHasTexture != true)
			{
				// Set the ID to -1 to show there is no material for this object
				pObject->materialID = -1;
			}
		}
	}

	// Read past the rest of the chunk since we don't care about shared vertices
	// You will notice we subtract the bytes already read in this chunk from the total length.
	pPreviousChunk->bytesRead += fread(gBuffer, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}			

bool CLoad3DS::Load3DSFile(char *FileName, t3DModel *pModel, CTga *Textura)
{
	char sZTexturePath[256];
	char sZTextureName[30];

	strcpy(sZTexturePath,"Texturas/");

	if(!Import3DS(pModel, FileName))			// Load our .3DS file into our model structure
		return 0;

	// Depending on how many textures we found, load each one (Assuming .BMP)
	// If you want to load other files than bitmaps, you will need to adjust CreateTexture().
	// Below, we go through all of the materials and check if they have a texture map to load.
	// Otherwise, the material just holds the color information and we don't need to load a texture.

	// Go through all the materials
	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(pModel->pMaterials[i].strFile) > 0)
		{
			// Use the name of the texture file to load the bitmap, with a texture ID (i).
			// We pass in our global texture array, the name of the texture, and an ID to reference it.	
			strcpy(sZTextureName,pModel->pMaterials[i].strFile);
			strcat(sZTexturePath,sZTextureName);

			Textura[i].LoadTGA(sZTexturePath);

			strcpy(sZTexturePath,"");
			strcpy(sZTextureName,"");
			strcpy(sZTexturePath,"Texturas/");
		}

		// Set the texture ID for this material
		pModel->pMaterials[i].texureId = i;
	}
	
	return TRUE;
}

void CLoad3DS::Render3DSFile(t3DModel *pModel, CTga *Textura, int tipo)
{
	// Since we know how many objects our model has, go through each of them.
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if(pModel->pObject.size() <= 0) break;

		// Get the current object that we are displaying
		t3DObject *pObject = &pModel->pObject[i];
			
		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture) {

			// Turn on texture mapping and turn off color
			glEnable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);

			// Bind the texture map to the object by it's materialID
			glBindTexture(GL_TEXTURE_2D, Textura[pObject->materialID].texID);
		} else {

			// Turn off texture mapping and turn on color
			glDisable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);
		}

		// This determines if we are in wireframe or normal mode
		if(tipo == 1)
			glBegin(GL_TRIANGLES);					// Begin drawing with our selected mode (triangles or lines)
		else if(tipo == 2)
			glBegin(GL_LINE_STRIP);

			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Go through each corner of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the index for each point of the face
					int index = pObject->pFaces[j].vertIndex[whichVertex];
			
					// Give OpenGL the normal for this vertex.
					glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
				
					// If the object has a texture associated with it, give it a texture coordinate.
					if(pObject->bHasTexture) {

						// Make sure there was a UVW map applied to the object or else it won't have tex coords.
						if(pObject->pTexVerts) {
							glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
						}
					} else {

						// Make sure there is a valid material/color assigned to this object.
						// You should always at least assign a material color to an object, 
						// but just in case we want to check the size of the material list.
						// if the size is at least one, and the material ID != -1,
						// then we have a valid material.
						if(pModel->pMaterials.size() && pObject->materialID >= 0) 
						{
							// Get and set the color that the object is, since it must not have a texture
							BYTE *pColor = pModel->pMaterials[pObject->materialID].color;

							// Assign the current color to this model
							glColor3ub(pColor[0], pColor[1], pColor[2]);
						}
					}

					// Pass in the current vertex of the object (Corner of current face)
					glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
				}
			}

		glEnd();								// End the drawing

		glDisable(GL_TEXTURE_2D);
	}

}

void CLoad3DS::UnLoad3DSFile(t3DModel *pModel, CTga *Textura)
{
	// Go through all the objects in the scene
	for(int i = 0; i < pModel->numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		delete [] pModel->pObject[i].pFaces;
		delete [] pModel->pObject[i].pNormals;
		delete [] pModel->pObject[i].pVerts;
		delete [] pModel->pObject[i].pTexVerts;
	}

	for(int i = 0; i < pModel->numOfMaterials; i++)
	{
		glDeleteTextures(1, &Textura[i].texID);
	}
}

// *Note* 
//
// Below are some math functions for calculating vertex normals.  We want vertex normals
// because it makes the lighting look really smooth and life like.  You probably already
// have these functions in the rest of your engine, so you can delete these and call
// your own.  I wanted to add them so I could show how to calculate vertex normals.

//////////////////////////////	Math Functions  ////////////////////////////////*

// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// This calculates a vector between 2 points and returns the result
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;							// The variable to hold the resultant vector

	vVector.x = vPoint1.x - vPoint2.x;			// Subtract point1 and point2 x's
	vVector.y = vPoint1.y - vPoint2.y;			// Subtract point1 and point2 y's
	vVector.z = vPoint1.z - vPoint2.z;			// Subtract point1 and point2 z's

	return vVector;								// Return the resultant vector
}

// This adds 2 vectors together and returns the result
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector2.x + vVector1.x;		// Add Vector1 and Vector2 x's
	vResult.y = vVector2.y + vVector1.y;		// Add Vector1 and Vector2 y's
	vResult.z = vVector2.z + vVector1.z;		// Add Vector1 and Vector2 z's

	return vResult;								// Return the resultant vector
}

// This divides a vector by a single number (scalar) and returns the result
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector1.x / Scaler;			// Divide Vector1's x value by the scaler
	vResult.y = vVector1.y / Scaler;			// Divide Vector1's y value by the scaler
	vResult.z = vVector1.z / Scaler;			// Divide Vector1's z value by the scaler

	return vResult;								// Return the resultant vector
}

// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;								// The vector to hold the cross product
												// Get the X value
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
												// Get the Y value
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
												// Get the Z value
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;								// Return the cross product
}

// This returns the normal of a vector
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;							// This holds the magitude			

	Magnitude = Mag(vNormal);					// Get the magnitude

	vNormal.x /= (float)Magnitude;				// Divide the vector's X by the magnitude
	vNormal.y /= (float)Magnitude;				// Divide the vector's Y by the magnitude
	vNormal.z /= (float)Magnitude;				// Divide the vector's Z by the magnitude

	return vNormal;								// Return the normal
}

///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function computes the normals and vertex normals of the objects
/////
///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CLoad3DS::ComputeNormals(t3DModel *pModel)
{
	CVector3 vVector1, vVector2, vNormal, vPoly[3];

	// If there are no objects, we can skip this part
	if(pModel->numOfObjects <= 0)
		return;

	// What are vertex normals?  And how are they different from other normals?
	// Well, if you find the normal to a triangle, you are finding a "Face Normal".
	// If you give OpenGL a face normal for lighting, it will make your object look
	// really flat and not very round.  If we find the normal for each vertex, it makes
	// the smooth lighting look.  This also covers up blocky looking objects and they appear
	// to have more polygons than they do.    Basically, what you do is first
	// calculate the face normals, then you take the average of all the normals around each
	// vertex.  It's just averaging.  That way you get a better approximation for that vertex.

	// Go through each of the objects to calculate their normals
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// Get the current object
		t3DObject *pObject = &(pModel->pObject[index]);

		// Here we allocate all the memory we need to calculate the normals
		CVector3 *pNormals		= new CVector3 [pObject->numOfFaces];
		CVector3 *pTempNormals	= new CVector3 [pObject->numOfFaces];
		pObject->pNormals		= new CVector3 [pObject->numOfVerts];

		// Go though all of the faces of this object
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			// To cut down LARGE code, we extract the 3 points of this face
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)

			vVector1 = Vector(vPoly[0], vPoly[2]);		// Get the vector of the polygon (we just need 2 sides for the normal)
			vVector2 = Vector(vPoly[2], vPoly[1]);		// Get a second vector of the polygon

			vNormal  = Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
			pTempNormals[i] = vNormal;					// Save the un-normalized normal for the vertex normals
			vNormal  = Normalize(vNormal);				// Normalize the cross product to give us the polygons normal

			pNormals[i] = vNormal;						// Assign the normal to the list of normals
		}

		//////////////// Now Get The Vertex Normals /////////////////

		CVector3 vSum = {0.0, 0.0, 0.0};
		CVector3 vZero = vSum;
		int shared=0;

		for (int i = 0; i < pObject->numOfVerts; i++)			// Go through all of the vertices
		{
			for (int j = 0; j < pObject->numOfFaces; j++)	// Go through all of the triangles
			{												// Check if the vertex is shared by another face
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);// Add the un-normalized normal of the shared face
					shared++;								// Increase the number of shared triangles
				}
			}      
			
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// Normalize the normal for the final vertex normal
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);	

			vSum = vZero;									// Reset the sum
			shared = 0;										// Reset the shared
		}
	
		// Free our memory and start over on the next object
		delete [] pTempNormals;
		delete [] pNormals;
	}
}
