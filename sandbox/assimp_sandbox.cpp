#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)

void parse_scene(const aiScene* pScene);
void parse_meshes(const aiScene* pScene);
void parse_mesh_bones(const aiMesh* pMesh);
void parse_single_bone(int bone_index, const aiBone* pBone);

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

    for (int i = 0; i < pScene->mNumMeshes; i++)
    {
        const aiMesh* pMesh = pScene->mMeshes[i];
        int num_vertices = pMesh->mNumVertices;
        int num_indices = pMesh->mNumFaces * 3;
        int num_bones = pMesh->mNumBones;

        printf(" Mesh %d '%s : vertices %d indices %d bones %d\n\n", i, pMesh->mName.C_Str(), num_vertices, num_indices, num_bones);

        total_vertices += num_vertices;
        total_indices += num_indices;
        total_bones += num_bones;

        
        if (pMesh->HasBones())
        {
            parse_mesh_bones(pMesh);
        }
        

        printf("\n");
    }
}

void parse_mesh_bones(const aiMesh* pMesh)
{
    for (int i = 0; i < pMesh->mNumBones; i++)
    {
        parse_single_bone(i, pMesh->mBones[i]);
    }
}

void parse_single_bone(int bone_index, const aiBone* pBone)
{
    printf("Bone %d: '%s' num vertices affected  by this bone: %d\n", bone_index, pBone->mName.C_Str(), pBone->mNumWeights);

    for (int i = 0; i < pBone->mNumWeights; i++)
    {
        if (i == 0)
            printf("\n");

        const aiVertexWeight& vw = pBone->mWeights[i];

        printf("    %d: vertex id %d weight %.2f\n", i, vw.mVertexId, vw.mWeight);
    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
