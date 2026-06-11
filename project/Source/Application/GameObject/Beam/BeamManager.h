
#pragma once
#include"Beam.h"
#include<vector>
#include"Vector3.h"
#include"Camera.h"

class BeamManager
{
public:
	BeamManager();
	~BeamManager();
	void Initialize();
	void Update();
	void Draw(Camera* camera);
	bool ShotBeam(const Vector3& target);
	const std::vector<std::unique_ptr<Beam>>& GetBeams() const {
		return beams_;
	};
	void SetParentMatrix(Matrix4x4* parent);
private:
	std::vector<std::unique_ptr<Beam>> beams_;
};
