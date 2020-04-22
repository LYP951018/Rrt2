#include <Rrt2/SceneObjLoader.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gsl/gsl_util>
#include <gsl/span>
#include <Rrt2/Geometries/TriangleMesh.hpp>

namespace rrt
{
    static Vec3f aiVecToRrtVec(const aiVector3D& vec3)
    {
        return Vec3f{vec3.x, vec3.y, vec3.z};
    }

    std::unique_ptr<TriangleMesh> MeshFromShape(const aiMesh& assimpMesh)
    {
        gsl::span<const aiFace> modelFaces =
            gsl::make_span(assimpMesh.mFaces, assimpMesh.mNumFaces);

        std::vector<TriangleIndices> meshIndices;
        for (const aiFace& modelFace : modelFaces)
        {
            // triangles, right?
            assert(modelFace.mNumIndices == 3);
            // 目前只支持 16 位的 index
            meshIndices.push_back(TriangleIndices{
                gsl::narrow_cast<std::uint16_t>(modelFace.mIndices[0]),
                gsl::narrow_cast<std::uint16_t>(modelFace.mIndices[1]),
                gsl::narrow_cast<std::uint16_t>(modelFace.mIndices[2]),
            });
        }

        gsl::span<const aiVector3D> modelVertices =
            gsl::make_span(assimpMesh.mVertices, assimpMesh.mNumVertices);
        std::vector<Vec3f> meshVertices;

        for (const aiVector3D& modelVertex : modelVertices)
        {
            meshVertices.push_back(aiVecToRrtVec(modelVertex));
        }

        std::vector<Vec3f> meshNormals;
        gsl::span<const aiVector3D> modelNormals =
            gsl::make_span(assimpMesh.mNormals, assimpMesh.mNumVertices);

        for (const aiVector3D& modelNormal : modelNormals)
        {
            meshNormals.push_back(aiVecToRrtVec(modelNormal));
        }

        std::unique_ptr<TriangleMesh> triangleMesh =
            std::make_unique<TriangleMesh>(std::move(meshVertices),
                                           std::move(modelNormals),
                                           std::move(meshIndices));
        triangleMesh->SetName(std::string{assimpMesh.mName.C_Str()});
        return triangleMesh;
    }

    bool AddMeshesFromObj(Scene& scene, const char* objFilePath)
    {
        using namespace Assimp;
        Importer importer;
        const aiScene* modelScene =
            importer.ReadFile(objFilePath, aiProcess_Triangulate);
        if (!modelScene)
        {
            return false;
        }
        gsl::span<const aiMesh* const> modelMeshes =
            gsl::make_span(modelScene->mMeshes, modelScene->mNumMeshes);
        for (const aiMesh* modelMesh : modelMeshes)
        {
            scene.AddTriangleMesh(MeshFromShape(*modelMesh));
        }
        return true;
    }
} // namespace rrt
