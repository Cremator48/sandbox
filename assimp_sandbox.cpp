#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <assert.h>
#include <map>
#include <string>
#include <vector>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define MAX_NUM_BONES_PER_VERTEX 10

void parse_scene(const aiScene* pScene);
void parse_meshes(const aiScene* pScene);
void parse_mesh_bones(int mesh_index, const aiMesh* pMesh);
void parse_single_bone(int bone_index, const aiBone* pBone);
int get_bone_id(const aiBone* pBone);

struct VertexBoneData // Структура содержащая массивы индексов костей и весов влияющих на конкретную вершину
{
    unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 }; // Массив ID костей  влияющих на вершину (максимальное количество определенно как 4)
    float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };    // Массив весов костей влияющих на вершину (максимальное количество определенно как 4)

    VertexBoneData()    // дефолтный конструктор
    {

    }

    void AddBoneData(unsigned int BoneID, float Weight) // Метод добавляеят данные об кости из аргументов в члены структуры
    {
        for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++)
        {
            if (Weights[i] == 0.0f)
            {
                BoneIDs[i] = BoneID;
                Weights[i] = Weight;
                printf("bone %d weight %f index %i\n", BoneID, Weight, i);
                return;
            }
        }

        assert(0); // Если что то пошло не по плану
    }
};

std::vector<VertexBoneData> vertex_to_bones; // Массив структур весов и индексов костей для каждой вершины
std::vector<int> mesh_base_vertex;
std::map<std::string, unsigned int> bone_name_to_index_map; // Ключ-значение: имя_кости-ID_кости

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

    if (argc != 2)
    {
        printf("Usage: %s <modle filename>\n", argv[0]);
        return 1;
    }

    char* filename = argv[1];

    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);

    if (!pScene)
    {
        printf("Error parsing '%s' : '%s'\n", filename, Importer.GetErrorString());
    }

    parse_scene(pScene);

    std::cout << "That's fine!\n";
    return 0;
}


void parse_scene(const aiScene* pScene)
{
    parse_meshes(pScene);
}

void parse_meshes(const aiScene* pScene)
{
    printf("*********************************************");
    printf("Parsing %d meshes\n\n", pScene->mNumMeshes);
    int total_vertices = 0;
    int total_indices = 0;
    int total_bones = 0;

    mesh_base_vertex.resize(pScene->mNumMeshes);

    for (int i = 0; i < pScene->mNumMeshes; i++)
    {
        const aiMesh* pMesh = pScene->mMeshes[i];
        int num_vertices = pMesh->mNumVertices;
        int num_indices = pMesh->mNumFaces * 3;
        int num_bones = pMesh->mNumBones;
        mesh_base_vertex[i] = total_vertices;

        printf(" Mesh %d '%s : vertices %d indices %d bones %d\n\n", i, pMesh->mName.C_Str(), num_vertices, num_indices, num_bones);

        total_vertices += num_vertices;
        total_indices += num_indices;
        total_bones += num_bones;

        vertex_to_bones.resize(total_vertices);
        
        if (pMesh->HasBones())
        {
            parse_mesh_bones(i, pMesh);
        }
        

        printf("\n");
    }
}

void parse_mesh_bones(int mesh_index, const aiMesh* pMesh)
{
    for (int i = 0; i < pMesh->mNumBones; i++)
    {
        parse_single_bone(mesh_index, pMesh->mBones[i]);
    }
}

void parse_single_bone(int mesh_index, const aiBone* pBone) // для разбора кадой кости в функцию передаётся индекс разбираемого меша и саму кость.
{
    printf("Bone '%s': num vertices affected  by this bone: %d\n", pBone->mName.C_Str(), pBone->mNumWeights);

    int bone_id = get_bone_id(pBone);
    printf("bone id %d\n", bone_id);

    for (int i = 0; i < pBone->mNumWeights; i++)
    {
        if (i == 0)
        {
            printf("\n");
        }

        const aiVertexWeight& vw = pBone->mWeights[i];

        unsigned int global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
        printf("Vertex id %d ", global_vertex_id);

        assert(global_vertex_id < vertex_to_bones.size());
        vertex_to_bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
    }

    printf("\n");
}

int get_bone_id(const aiBone* pBone) // возращает id кости
{
    int bone_id = 0;
    std::string bone_name(pBone->mName.C_Str());

    if (bone_name_to_index_map.find(bone_name) == bone_name_to_index_map.end()) // Если ключ не нашёлся в словаре, то find указывает на конец словаря
    {
        // и в этом случае 
        bone_id = bone_name_to_index_map.size(); // .size возварщает количество элементов в словаре и это значение присваивается ID новой кости
        bone_name_to_index_map[bone_name] = bone_id; // Создаём новое связку ключ-значение: имя_новой_кости-ID_новой_кости
    }
    else // если же в словаре есть такое значение - вернуть имя
    {
        bone_id = bone_name_to_index_map[bone_name];
    }

    return bone_id;
}