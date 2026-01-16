#pragma once
#include <vector>
#include <string>
#include <map>


namespace FlawedEngine {
	struct MeshCPUData;
	struct sBoneInfo;

    bool SaveMeshesBinary(const std::string& path, const std::vector<MeshCPUData>& meshes, const std::map<std::string, sBoneInfo>& boneInfoMap, int boneCounter);
    bool LoadMeshesBinary(const std::string& path, std::vector<MeshCPUData>& outMeshes, std::map<std::string, sBoneInfo>& outBoneInfoMap, int& outBoneCounter);
}