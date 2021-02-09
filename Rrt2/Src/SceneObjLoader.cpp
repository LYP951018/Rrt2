#include <Rrt2/SceneObjLoader.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gsl/gsl_util>
#include <span>
#include <Rrt2/Geometries/TriangleMesh.hpp>
#include <stdexcept>
#include <string_view>
#include <fmt/format.h>
#include <Rrt2/Materials/MattMaterial.hpp>
#include <Rrt2/Textures/ConstantTexture.hpp>

namespace rrt
{
    class AssimpLoadException : public std::runtime_error
    {
      public:
        AssimpLoadException(std::string_view errMsg, aiReturn assimpErrCode)
            : std::runtime_error{std::string{errMsg}}, m_assimpErrCode{assimpErrCode}
        {}
        AssimpLoadException(aiReturn assimpErrCode)
            : std::runtime_error{GetDefaultExceptionMessage(assimpErrCode)},
              m_assimpErrCode{assimpErrCode}
        {}

      private:
        aiReturn m_assimpErrCode;

        std::string GetDefaultExceptionMessage(aiReturn errCode)
        {
            return fmt::format("assimp failed with error code {}", m_assimpErrCode);
        }
    };

    static Vec3f aiVecToRrtVec(const aiVector3D& vec3)
    {
        return Vec3f{vec3.x, vec3.y, vec3.z};
    }

    static Vec3f aiColorToRrtVec(const aiColor3D& vec3)
    {
        return Vec3f{vec3.r, vec3.g, vec3.b};
    }

    static void TryAssimp(aiReturn assimpRetCode)
    {
        if (assimpRetCode != AI_SUCCESS)
        {
            throw AssimpLoadException(assimpRetCode);
        }
    }

    class AssimpLoader
    {
      public:
        bool AddMeshesFromObj(Scene& scene, const char* objFilePath)
        {
            using namespace Assimp;
            Importer importer;
            m_assimpScene = importer.ReadFile(objFilePath, aiProcess_Triangulate);
            if (!m_assimpScene)
            {
                auto str = importer.GetErrorString();
                return false;
            }
            LoadMaterials();
            std::span<const aiMesh* const> modelMeshes =
                std::span(m_assimpScene->mMeshes, m_assimpScene->mNumMeshes);

            for (const aiMesh* modelMesh : modelMeshes)
            {
                scene.AddTriangleMesh(MeshFromShape(*modelMesh));
            }
            return true;
        }

      private:
        std::vector<std::shared_ptr<MaterialBase>> m_materials;
        const aiScene* m_assimpScene;

        void LoadMaterials()
        {
            std::span<const aiMaterial* const> modelMaterials =
                std::span(m_assimpScene->mMaterials, m_assimpScene->mNumMaterials);

            for (const aiMaterial* const assimpMaterial : modelMaterials)
            {
                aiColor3D ka, kd, ks;
                aiReturn errCode = AI_SUCCESS;

                TryAssimp(assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, kd));
                TryAssimp(assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ka));
                TryAssimp(assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, ks));
                m_materials.push_back(std::make_shared<DiffuseOnlyMaterial>(
                    std::make_shared<ConstantTexture<Vec3f>>(aiColorToRrtVec(kd))));
            }
        }

        std::unique_ptr<TriangleMesh> MeshFromShape(const aiMesh& assimpMesh)
        {
            std::span<const aiFace> modelFaces =
                std::span(assimpMesh.mFaces, assimpMesh.mNumFaces);

            std::vector<std::uint16_t> meshIndices;
            meshIndices.reserve(modelFaces.size() * 3);
            for (const aiFace& modelFace : modelFaces)
            {
                // triangles, right?
                assert(modelFace.mNumIndices == 3);
                // 目前只支持 16 位的 index
                meshIndices.insert(
                    meshIndices.end(),
                    {
                        gsl::narrow_cast<std::uint16_t>(modelFace.mIndices[0]),
                        gsl::narrow_cast<std::uint16_t>(modelFace.mIndices[1]),
                        gsl::narrow_cast<std::uint16_t>(modelFace.mIndices[2]),
                    });
            }

            std::span<const aiVector3D> modelVertices =
                std::span(assimpMesh.mVertices, assimpMesh.mNumVertices);
            std::vector<Vec3f> meshVertices;

            for (const aiVector3D& modelVertex : modelVertices)
            {
                meshVertices.push_back(aiVecToRrtVec(modelVertex));
            }

            std::vector<Vec3f> meshNormals;
            std::span<const aiVector3D> modelNormals =
                std::span(assimpMesh.mNormals, assimpMesh.mNumVertices);

            for (const aiVector3D& modelNormal : modelNormals)
            {
                meshNormals.push_back(aiVecToRrtVec(modelNormal));
            }

            std::unique_ptr<TriangleMesh> triangleMesh = std::make_unique<TriangleMesh>(
                std::move(meshVertices), std::move(modelNormals), std::move(meshIndices));
            triangleMesh->SetName(std::string{assimpMesh.mName.C_Str()});
            triangleMesh->material = m_materials[assimpMesh.mMaterialIndex];
            return triangleMesh;
        }
    };

    bool AddMeshesFromObj(Scene& scene, const char* objFilePath)
    {
        return AssimpLoader{}.AddMeshesFromObj(scene, objFilePath);
    }
} // namespace rrt
