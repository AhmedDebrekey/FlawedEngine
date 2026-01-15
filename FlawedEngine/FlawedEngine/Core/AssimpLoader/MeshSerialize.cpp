#include "MeshSerialize.h"
#include "../Core.h"
#include <fstream>
namespace FlawedEngine {

	#define VERSION 1

	static void write_u32(std::ofstream& out, std::uint32_t v)
	{
		out.write((char*)&v, sizeof(v));
	}

	static bool read_u32(std::ifstream& in, std::uint32_t& v) {
		return static_cast<bool>(in.read(reinterpret_cast<char*>(&v), sizeof(v)));
	}

	static void write_bytes(std::ofstream& out, const void* p, std::size_t n) 
	{
		out.write((char*)p, n);
	}

	static bool read_bytes(std::ifstream& in, void* p, std::size_t n)
	{
		return static_cast<bool>(in.read(reinterpret_cast<char*>(p), n));
	}

	static void write_vec2(std::ofstream& out, const glm::vec2& v)
	{
		float a[2] = { v.x, v.y };
		write_bytes(out, a, sizeof(a));
	}

	static bool read_vec2(std::ifstream& in, glm::vec2& v)
	{
		float a[2];
		if (!read_bytes(in, a, sizeof(a))) return false;
		v = glm::vec2(a[0], a[1]);
		return true;
	}

	static void write_vec3(std::ofstream& out, const glm::vec3& v)
	{
		float a[3] = { v.x, v.y, v.z };
		write_bytes(out, a, sizeof(a));
	}

	static bool read_vec3(std::ifstream& in, glm::vec3& v)
	{
		float a[3];
		if (!read_bytes(in, a, sizeof(a))) return false;
		v = glm::vec3(a[0], a[1], a[2]);
		return true;
	}

	static void write_vertex(std::ofstream& out, const sVertex& v)
	{
		write_vec3(out, v.Postion);
		write_vec3(out, v.Normal);
		write_vec2(out, v.TexCoords);
		write_vec3(out, v.Tangent);
		write_vec3(out, v.BiTangent);
		write_bytes(out, v.mBoneIDs, sizeof(v.mBoneIDs));
		write_bytes(out, v.mWeights, sizeof(v.mWeights));
	}

	static bool read_vertex(std::ifstream& in, sVertex& v) {
		if (!read_vec3(in, v.Postion)) return false;
		if (!read_vec3(in, v.Normal)) return false;
		if (!read_vec2(in, v.TexCoords)) return false;
		if (!read_vec3(in, v.Tangent)) return false;
		if (!read_vec3(in, v.BiTangent)) return false;

		if (!read_bytes(in, v.mBoneIDs, sizeof(v.mBoneIDs))) return false;
		if (!read_bytes(in, v.mWeights, sizeof(v.mWeights))) return false;

		return true;
	}

	static void write_string(std::ofstream& out, const std::string& str)
	{
		std::uint32_t length = static_cast<std::uint32_t>(str.size());
		write_u32(out, length);
		if (length > 0) {
			write_bytes(out, str.data(), length);
		}
	}


	static bool read_string(std::ifstream& in, std::string& str) {
		std::uint32_t length = 0;
		if (!read_u32(in, length)) return false;

		str.resize(length);
		if (length == 0) return true;

		return read_bytes(in, str.data(), length);
	}

	bool SaveMeshesBinary(const std::string& path, const std::vector<MeshCPUData>& meshes)
	{
		std::ofstream out("Assets/EngineMeshes/" + path + ".FEB", std::ios::binary | std::ios::trunc);
		if (!out) return false;
		// Flawed Engine Mesh Save
		const char magic[4] = { 'F', 'E', 'M', 'S'};
		write_bytes(out, magic, 4);
		write_u32(out, VERSION); // version

		write_u32(out, static_cast<std::uint32_t>(meshes.size()));

		for (const auto& mesh : meshes)
		{
			write_u32(out, static_cast<std::uint32_t>(mesh.vertices.size()));
			write_u32(out, static_cast<std::uint32_t>(mesh.indices.size()));
			write_u32(out, static_cast<std::uint32_t>(mesh.textures.size()));

			for (const auto& vertex : mesh.vertices)
			{
				write_vertex(out, vertex);
			}

			for (const auto& index : mesh.indices)
			{
				write_u32(out, static_cast<uint32_t>(index));
			}


			for (const auto& texture : mesh.textures)
			{
				write_u32(out, texture.ID);
				write_string(out, texture.Path);
				write_string(out, texture.Type);
				write_u32(out, texture.width);
				write_u32(out, texture.height);
				write_u32(out, texture.components);
				write_u32(out, static_cast<std::uint32_t>(texture.pixels.size()));
				if (!texture.pixels.empty())
					write_bytes(out, texture.pixels.data(), texture.pixels.size());

			}

		}

		return static_cast<bool>(out);
	}

	bool LoadMeshesBinary(const std::string& path, std::vector<MeshCPUData>& outMeshes)
	{
		std::ifstream in(path, std::ios::binary);
		if (!in) return false;

		char magic[4];
		if (!read_bytes(in, magic, 4)) return false;
		if (!(magic[0] == 'F' && magic[1] == 'E' && magic[2] == 'M' && magic[3] == 'S')) return false;
	
		std::uint32_t version = 0;
		if (!read_u32(in, version)) return false;
		if (version != VERSION) return false;

		std::uint32_t meshCount = 0;
		if (!read_u32(in, meshCount)) return false;

		outMeshes.clear();
		outMeshes.resize(meshCount);
		
		for (std::uint32_t mi = 0; mi < meshCount; ++mi) {
			auto& mesh = outMeshes[mi];

			std::uint32_t vCount = 0, iCount = 0, tCount = 0;
			if (!read_u32(in, vCount)) return false;
			if (!read_u32(in, iCount)) return false;
			if (!read_u32(in, tCount)) return false;

			mesh.vertices.resize(vCount);
			mesh.indices.resize(iCount);
			mesh.textures.resize(tCount);

			for (auto& v : mesh.vertices) {
				if (!read_vertex(in, v)) return false;
			}

			for (auto& idx : mesh.indices) {
				std::uint32_t tmp = 0;
				if (!read_u32(in, tmp)) return false;
				idx = static_cast<unsigned int>(tmp);
			}

			for (auto& tex : mesh.textures) {
				std::uint32_t savedID = 0;
				if (!read_u32(in, savedID)) return false; 
				tex.ID = 0; // important: ID should be re-created on load

				if (!read_string(in, tex.Path)) return false;
				if (!read_string(in, tex.Type)) return false;
				if (!read_u32(in, tex.width)) return false;
				if (!read_u32(in, tex.height)) return false;
				if (!read_u32(in, tex.components)) return false;
				std::uint32_t byteCount = 0;
				if (!read_u32(in, byteCount)) return false;
				const std::uint64_t expected =
					static_cast<std::uint64_t>(tex.width) *
					static_cast<std::uint64_t>(tex.height) *
					static_cast<std::uint64_t>(tex.components);

				if (expected != byteCount) return false;
				tex.pixels.resize(byteCount);
				if (byteCount > 0 && !read_bytes(in, tex.pixels.data(), byteCount)) return false;
			}
		}

		return static_cast<bool>(in);
	}
}