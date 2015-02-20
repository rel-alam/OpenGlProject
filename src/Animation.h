#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Application.h"
#include "Camera.h"

#include "FBXFile.h"
#include "Vertex.h"

class Animation : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void GenerateGLMeshes(FBXFile* fbx);



	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer);
	void UpdateBones(FBXSkeleton* skeleton);



	FlyCamera camera;

	float m_timer;

	unsigned int m_program_id;

	FBXFile* m_file;
	std::vector<OpenGLData> m_meshes;
};

#endif // !CAM_PROJ_H_
