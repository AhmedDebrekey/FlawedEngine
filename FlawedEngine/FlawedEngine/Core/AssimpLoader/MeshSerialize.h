#pragma once
#include <vector>
#include <string>


namespace FlawedEngine {
	struct MeshCPUData;

    bool SaveMeshesBinary(const std::string& path, const std::vector<MeshCPUData>& meshes);
    bool LoadMeshesBinary(const std::string& path, std::vector<MeshCPUData>& outMeshes);
}