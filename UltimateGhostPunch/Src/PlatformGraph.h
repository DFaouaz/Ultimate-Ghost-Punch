#pragma once
#ifndef PLATFORM_GRAPH_H
#define PLATFORM_GRAPH_H

#include <UserComponent.h>
#include <vector>
#include <map>

class PhysicsSystem;
class RaycastHit;
class PlatformNode;
class NavigationLink;

class PlatformGraph : public UserComponent
{
private:
	PhysicsSystem* physicsSystem;
	Vector3 levelStart;
	Vector3 levelEnd;
	Vector3 currentPos;

	std::vector<PlatformNode> platforms;
	std::map<float, int> lastPlatforms;

	int currentPlatformIndex;
	Vector3 fallOffset;
	Vector3 playerCollisionSize;

	std::string saveFilename;
	std::string loadFilename;
	std::string fileRoute;

	void drawLinks();

	float getDistance(const Vector3& pos, const PlatformNode& node);

protected:
	virtual void start();
	virtual void update(float deltaTime);
	virtual void handleData(ComponentData* data);

public:
	PlatformGraph(GameObject* gameObject);
	virtual ~PlatformGraph();

	void createNodes();

	void saveGraph();
	bool loadGraph();

	void clearAllConnections();
	void clearConnections(int platform);

	void addLinkToPlatform(int platform, const NavigationLink& navLink);
	void removeLastLink(int platform);

	void setSaveFileName(std::string name);
	void setLoadFileName(std::string name);

	int getIndex(const Vector3& pos);

	int getFurthestIndex(const Vector3& pos);
	int getClosestIndex(const Vector3& pos);

	std::vector<PlatformNode>& getPlatforms();
};

#endif