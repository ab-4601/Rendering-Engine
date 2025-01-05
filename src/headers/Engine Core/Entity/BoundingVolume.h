#ifndef _BOUNDING_VOLUME_H_
#define _BOUNDING_VOLUME_H_

#include "../Mesh/Model.h"
#include "Transform.h"
#include <glm/gtx/norm.hpp>
#include <Dense>

struct BoundingVolume {
	virtual bool isWithinFrustum(const Frustum& frustum, const Transform& transform) const = 0;
	virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

	bool isWithinFrustum(const Frustum& frustum) const {
		return (
			isOnOrForwardPlane(frustum.leftPlane) && isOnOrForwardPlane(frustum.rightPlane) &&
			isOnOrForwardPlane(frustum.topPlane) && isOnOrForwardPlane(frustum.bottomPlane) &&
			isOnOrForwardPlane(frustum.nearPlane) && isOnOrForwardPlane(frustum.farPlane)
		);
	}
};

struct SphereBV : public BoundingVolume {
	glm::vec3 center;
	float radius;

	SphereBV(glm::vec3 center = glm::vec3{ 0.f }, float radius = 0.f)
		: BoundingVolume(), center{ center }, radius{ radius } {
	}

	virtual bool isOnOrForwardPlane(const Plane& plane) const override final {
		return plane.getSignedDistanceToPlane(center) > -radius;
	}

	virtual bool isWithinFrustum(const Frustum& frustum, const Transform& transform) const override final {
		const glm::vec3 globalScale = transform.getGlobalScale();
		const glm::vec3 globalCenter = transform.getModelMatrix() * glm::vec4(center, 1.f);
		const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

		SphereBV globalSphere(globalCenter, radius * (maxScale * 0.5f));

		return (
			globalSphere.isOnOrForwardPlane(frustum.nearPlane) && globalSphere.isOnOrForwardPlane(frustum.farPlane) &&
			globalSphere.isOnOrForwardPlane(frustum.leftPlane) && globalSphere.isOnOrForwardPlane(frustum.rightPlane) &&
			globalSphere.isOnOrForwardPlane(frustum.topPlane) && globalSphere.isOnOrForwardPlane(frustum.bottomPlane)
		);
	}
};

struct SquareAABB : public BoundingVolume {
	glm::vec3 center{ 0.f };
	float extent{ 0.f };

	SquareAABB(glm::vec3 center = glm::vec3{ 0.f }, float extent = 0.f)
		: BoundingVolume(), center{ center }, extent{ extent } {
	}

	virtual bool isOnOrForwardPlane(const Plane& plane) const override final {
		const float r = extent * (std::abs(plane.normal.x) + std::abs(plane.normal.y) + std::abs(plane.normal.z));
		return -r <= plane.getSignedDistanceToPlane(center);
	}

	virtual bool isWithinFrustum(const Frustum& frustum, const Transform& transform) const override final {
		const glm::vec3 globalCenter = transform.getModelMatrix() * glm::vec4(center, 1.f);
		const glm::vec3 right = transform.getRightVector() * extent;
		const glm::vec3 up = transform.getUpVector() * extent;
		const glm::vec3 forward = transform.getForwardVector() * extent;

		glm::vec3 i{ 1.f, 0.f, 0.f }, j{ 0.f, 1.f, 1.f }, k{ 0.f, 0.f, 1.f };

		const float iCap = std::abs(glm::dot(i, right)) + std::abs(glm::dot(i, up)) + std::abs(glm::dot(i, forward));
		const float jCap = std::abs(glm::dot(j, right)) + std::abs(glm::dot(j, up)) + std::abs(glm::dot(j, forward));
		const float kCap = std::abs(glm::dot(k, right)) + std::abs(glm::dot(k, up)) + std::abs(glm::dot(k, forward));

		const SquareAABB globalAABB(globalCenter, std::max(std::max(iCap, jCap), kCap));

		return (
			globalAABB.isOnOrForwardPlane(frustum.leftPlane) && globalAABB.isOnOrForwardPlane(frustum.rightPlane) &&
			globalAABB.isOnOrForwardPlane(frustum.nearPlane) && globalAABB.isOnOrForwardPlane(frustum.farPlane) &&
			globalAABB.isOnOrForwardPlane(frustum.topPlane) && globalAABB.isOnOrForwardPlane(frustum.bottomPlane)
		);
	}
};

struct AABB : public BoundingVolume {
	glm::vec3 center;
	glm::vec3 extents;
	glm::vec3 vertices[8];

	AABB(glm::vec3 min, glm::vec3 max)
		: BoundingVolume(), center{ (max + min) * 0.5f }, extents{ max.x - center.x, max.y - center.y, max.z - center.z } {
		for (int i = 0; i < 8; i++)
			vertices[i] = glm::vec3{ 0.f };
	}

	AABB(glm::vec3 center, float iCap, float jCap, float kCap)
		: BoundingVolume(), center{ center }, extents{ iCap, jCap, kCap } {
		for (int i = 0; i < 8; i++)
			vertices[i] = glm::vec3{ 0.f };
	}

	const glm::vec3* getVertices() {
		vertices[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
		vertices[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
		vertices[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
		vertices[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
		vertices[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
		vertices[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
		vertices[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
		vertices[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };

		return vertices;
	}

	virtual bool isOnOrForwardPlane(const Plane& plane) const override final {
		const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
			extents.z * std::abs(plane.normal.z);

		return -r <= plane.getSignedDistanceToPlane(center);
	}

	virtual bool isWithinFrustum(const Frustum& frustum, const Transform& transform) const override final {
		const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };
		const glm::vec3 right = transform.getRightVector() * extents.x;
		const glm::vec3 up = transform.getUpVector() * extents.y;
		const glm::vec3 forward = transform.getForwardVector() * extents.z;

		glm::vec3 i{ 1.f, 0.f, 0.f }, j{ 0.f, 1.f, 1.f }, k{ 0.f, 0.f, 1.f };

		const float iCap = std::abs(glm::dot(i, right)) + std::abs(glm::dot(i, up)) + std::abs(glm::dot(i, forward));
		const float jCap = std::abs(glm::dot(j, right)) + std::abs(glm::dot(j, up)) + std::abs(glm::dot(j, forward));
		const float kCap = std::abs(glm::dot(k, right)) + std::abs(glm::dot(k, up)) + std::abs(glm::dot(k, forward));

		const AABB globalAABB(globalCenter, iCap, jCap, kCap);

		return (
			globalAABB.isOnOrForwardPlane(frustum.leftPlane) && globalAABB.isOnOrForwardPlane(frustum.rightPlane) &&
			globalAABB.isOnOrForwardPlane(frustum.nearPlane) && globalAABB.isOnOrForwardPlane(frustum.farPlane) &&
			globalAABB.isOnOrForwardPlane(frustum.topPlane) && globalAABB.isOnOrForwardPlane(frustum.bottomPlane)
		);
	}
};

struct OBB : public BoundingVolume {
	glm::vec3 center;
	glm::vec3 extents;
	glm::vec3 axes[3];

	OBB(glm::vec3 center = glm::vec3(0.f), glm::vec3 extents = glm::vec3(0.f))
		: center{ center }, extents{ extents }
	{
		for (int i = 0; i < 3; i++)
			this->axes[i] = glm::vec3(0.f);
	}

	virtual bool isOnOrForwardPlane(const Plane& plane) const override final {
		return false;
	}

	virtual bool isWithinFrustum(const Frustum& frustum, const Transform& transform) const override final {
		return false;
	}

	~OBB() = default;
};

AABB generateAABB(const Model& model) {
	glm::vec3 minAABB{ std::numeric_limits<float>::max() };
	glm::vec3 maxAABB{ std::numeric_limits<float>::min() };

	for (const Vertex& vertex : model.getVertices()) {
		minAABB = glm::min(minAABB, vertex.position);
		maxAABB = glm::max(maxAABB, vertex.position);
	}

	return AABB(minAABB, maxAABB);
}

SphereBV generateSphereBV(const Model& model) {
	glm::vec3 minAABB{ std::numeric_limits<float>::max() };
	glm::vec3 maxAABB{ std::numeric_limits<float>::min() };

	for (const Vertex& vertex : model.getVertices()) {
		minAABB = glm::min(minAABB, vertex.position);
		maxAABB = glm::max(maxAABB, vertex.position);
	}

	return SphereBV((minAABB + maxAABB) * 0.5f, glm::length(minAABB - maxAABB));
}

OBB generateOBB(const Model& model, glm::vec3 axes) {
	OBB globalOBB;

	glm::vec3 mean{ 0.f };

	const std::vector<Vertex>& vertices = model.getVertices();

	for (const Vertex& vertex : vertices)
		mean += vertex.position;

	mean /= (float)vertices.size();

	Eigen::Matrix3f covariance = Eigen::Matrix3f::Zero();

	glm::vec3 centered{ 0.f };
	for (const Vertex& vertex : vertices) {
		centered = vertex.position - mean;
		covariance(0, 0) += centered.x * centered.x;
		covariance(0, 1) += centered.x * centered.y;
		covariance(0, 2) += centered.x * centered.z;

		covariance(1, 0) += centered.y * centered.x;
		covariance(1, 1) += centered.y * centered.y;
		covariance(1, 2) += centered.y * centered.z;

		covariance(2, 0) += centered.z * centered.x;
		covariance(2, 1) += centered.z * centered.y;
		covariance(2, 2) += centered.z * centered.z;
	}

	covariance /= (float)vertices.size();

	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covariance);
	if (solver.info() != Eigen::Success) {
		std::cerr << "Failed to compute eigen vectors" << std::endl;
		return globalOBB;
	}

	Eigen::Matrix3f eigenVectors = solver.eigenvectors();

	globalOBB.axes[0] = glm::vec3(eigenVectors(0, 0), eigenVectors(1, 0), eigenVectors(2, 0));
	globalOBB.axes[1] = glm::vec3(eigenVectors(0, 1), eigenVectors(1, 1), eigenVectors(2, 1));
	globalOBB.axes[2] = glm::vec3(eigenVectors(0, 2), eigenVectors(1, 2), eigenVectors(2, 2));

	glm::vec3 minOBB{ std::numeric_limits<float>::max() };
	glm::vec3 maxOBB{ std::numeric_limits<float>::min() };

	for (const Vertex& vertex : vertices) {
		for (int i = 0; i < 3; i++) {
			float projection = glm::dot(vertex.position - mean, globalOBB.axes[i]);
			minOBB[i] = std::min(minOBB[i], projection);
			maxOBB[i] = std::max(maxOBB[i], projection);
		}
	}

	globalOBB.center = mean;
	globalOBB.extents = (maxOBB - minOBB) * 0.5f;

	return globalOBB;
}

#endif