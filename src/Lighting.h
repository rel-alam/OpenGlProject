#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include "Application.h"
#include "Camera.h"

class FBXFile;

class Lighting : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void createOpenGLBuffers(FBXFile* file);
	void cleanUpOpenGLBuffers(FBXFile* file);

	void reloadShader();

	FlyCamera* camera;
	FBXFile* m_fbx_file;

	unsigned int m_program_id;

};

#endif