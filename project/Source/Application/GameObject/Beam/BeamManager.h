
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
	void Draw();
	bool ShotBeam(const Vector3& target, Matrix4x4* parent, const Beam::BeamType  beamType);
	const std::vector<std::unique_ptr<Beam>>& GetBeams() const {
		return beams_;
	};
private:
	std::vector<std::unique_ptr<Beam>> beams_;
};
