#include <Windows.h>
#include <vector>
#include "glm_header.h"

class BoundingSphere {
public:
	BoundingSphere() : centre(0), radius(1) {}
	~BoundingSphere() {}
	void fit(const BoundingSphere& sphere) {
		float d = glm::distance(centre, sphere.centre) +
			sphere.radius;
		if (d > radius)
			radius = d;
	}
	glm::vec4 centre;
	float radius;
};

class Node {
public:
	Node() : parent(nullptr), localTransform(1) {}
	virtual ~Node() { for (auto child : children) delete child; }
	void addChild(Node* child) {
		children.push_back(child);
	}
	virtual void update() {
		updateWorldTransform();
		for (auto child : children)
			child->update();
		updateWorldBounds();
	}
	void updateWorldTransform() {
		if (parent == nullptr)
			worldTransform = localTransform;
		else
			worldTransform = parent->worldTransform *
			localTransform;
	}
	void updateWorldBounds() {
		worldBounds.centre = worldTransform * localBounds.centre;
		worldBounds.radius = localBounds.radius;
		for (auto child : children)
			worldBounds.fit(child->worldBounds);
	}
protected:
	Node* parent;
	BoundingSphere localBounds;
	BoundingSphere worldBounds;
	glm::mat4 localTransform;
	glm::mat4 worldTransform;
	std::vector<Node*> children;
};

void addChildren(Node* root, int childrenPerNode, int depth) {
	if (depth > 0) {
		for (int i = 0; i < childrenPerNode; i++) {
			Node* child = new Node();
			addChildren(child, childrenPerNode, depth - 1);
			root->addChild(child);
		}
	}
}

int main()
{

	Node* root = new Node();
	addChildren(root, 6, 7);
	root->update();
	delete root;

	return 0;
}