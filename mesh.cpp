#include <glad/glad.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>
#include "shaders.h"
#include "geometry.h"


Mesh::Mesh(const aiMesh* m, const aiScene* scene,const std::string& path, bool withTextures){
    fsPath = path;
    vertexData.resize(m->mNumVertices);
    for (unsigned int i = 0; i < m->mNumVertices; i++){
        auto& pos = m->mVertices[i];
        vertexData[i].position = glm::vec3(pos.x,pos.y, pos.z);
        
        auto& n = m->mNormals[i];
        vertexData[i].normal = glm::vec3(n.x,n.y,n.z);
        
        if(m->HasTextureCoords(0)){
            auto& uv = m->mTextureCoords[0][i];
            vertexData[i].texCoord = glm::vec3(uv.x,uv.y, 0.f);
        }else{
            vertexData[i].texCoord = glm::vec3(0.f);
        }

        if(m->HasTangentsAndBitangents()){   
            auto& tan = m->mTangents[i];
            vertexData[i].tan = glm::vec3(tan.x,tan.y,tan.z);
        }

        if(m->HasVertexColors(0)){
            auto& col = m->mColors[0][i];
            vertexData[i].vCol = glm::vec4(col.r, col.g, col.b, col.a);
        }
    }

    for (unsigned int i=0; i < m->mNumFaces; i++){
        for(unsigned int j = 0; j < m->mFaces[i].mNumIndices; j++){
            indices.push_back(m->mFaces[i].mIndices[j]);
        }
    }
    texturesEnabled = withTextures;
    if(withTextures){
        LoadMaterialData(m, scene);
    }
    AllocateBindBuffers();

    indexCount = indices.size();
    vertexData.clear();
    indices.clear();
}

void Mesh::Draw(Shader* s){
    s->SetBool("b_packedMR", texturesEnabled);
    if (texturesEnabled){ // Not actually necessary as textures would be empty given they don't get loaded. Added for clarity.
        for(auto t: textures){
            t->Bind(s);
        }
    }
    glBindVertexArray(vertArrObj);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::AllocateBindBuffers(){
    glGenVertexArrays(1, &vertArrObj);
    glGenBuffers(1, &vertBuffObj);
    glGenBuffers(1,  &elemBuffObj);

    glBindVertexArray(vertArrObj);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(VertexData), &vertexData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex Pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) 0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 3));
    // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 6));
    // Tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 8));
    // Vertex Color
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (sizeof(float) * 11));
    
    glBindVertexArray(0);
}

void Mesh::LoadMaterialData(const aiMesh* m, const aiScene* s){
    assert(texturesEnabled);
    if(m->mMaterialIndex < 0){
        std::cerr << "[MESH][MATERIALS][WARN] Mesh "<< m->mName.C_Str() <<" in Scene " << s->mName.C_Str() << " has no materials that can be loaded" << std::endl;
        return;
    }

    aiMaterial* mat = s->mMaterials[m->mMaterialIndex];
    LoadTexture(mat, TexType::Albedo);
    LoadTexture(mat, TexType::MR);
    LoadTexture(mat, TexType::Normal);
    LoadTexture(mat, TexType::AO, false);
    LoadTexture(mat, TexType::Emissive, false);
}

void Mesh::LoadTexture(const aiMaterial* m, TexType tt, bool warnOnFailure){
    assert(texturesEnabled);
    aiTextureType aiTT;
    bool srgb = false;
    switch (tt){
        case TexType::Albedo:
            aiTT = aiTextureType_DIFFUSE;
            srgb = true;
            break;
        case TexType::MR:
            aiTT = aiTextureType_DIFFUSE_ROUGHNESS;
            break;
        case TexType::Normal:
            aiTT = aiTextureType_NORMALS;
            break;
        case TexType::AO:
            aiTT = aiTextureType_AMBIENT_OCCLUSION;
            break;
        case TexType::Emissive:
            aiTT = aiTextureType_EMISSIVE;
            break;
        case TexType::Metallic:
        case TexType::Roughness:
            assert(false && "Mesh does not support separate texture for Metallic and Roughness");
    }

    if (m->GetTextureCount(aiTT) > 0){
        aiString assetFileName;
        m->GetTexture(aiTT, 0, &assetFileName);

        std::string assetDirectory = fsPath.substr(0, fsPath.find_last_of('/'));
        std::string assetPath = assetDirectory + "/" + assetFileName.C_Str();
        Texture* t = new Texture(assetPath, tt, srgb);
        textures.push_back(t);
    }else{
        textures.push_back(new BlackTexture(tt));
        if(warnOnFailure){
            std::cerr << "[MESH][TEXTURE][LOAD][WARN] Requested texture (" << tex_name(tt) <<") is not present in model. ("<< fsPath << ")" << std::endl;
        }
    }
}